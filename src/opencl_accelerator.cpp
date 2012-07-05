/*
 * Copyright (c) 2012 Tom Alexander
 * 
 * This software is provided 'as-is', without any express or implied
 * warranty. In no event will the authors be held liable for any damages
 * arising from the use of this software.
 * 
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 * 
 *    1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software
 *    in a product, an acknowledgment in the product documentation would be
 *    appreciated but is not required.
 * 
 *    2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 * 
 *    3. This notice may not be removed or altered from any source
 *    distribution.
 */
#define OPENCL_ACCELERATOR_CPP_
#include "opencl_accelerator.h"
#include "opencl_sqt.h"
#include "rigidbody.h"
#include "util.h"
#include "collider.h"

/*
 * struct rigidbody
 * {
 * 1 use_gravity (1 true, 0 false)
 * 3 center_of_mass
 * 3 velocity
 * 1 mass
 * 1 elasticity
 * 16 I
 * 3 angular_momentum
 * }
 */

/*
 * struct sqt
 * {
 * 1 scale
 * 4 rotation
 * 3 position
 * }
 */

/*
 * struct collider
 * {
 * 1 type (SPHERE = 0, AABB = 1)
 * 1 radius
 * 3 lesser_corner
 * 3 greater_corner
 * }
 */

namespace topaz
{
    opencl* global_accelerator = NULL;

    opencl::opencl()
    {
        error_code = clGetPlatformIDs(10, platforms, &num_platforms);
        std::cout << "Num Platforms: " << num_platforms << "\n";

        error_code = clGetDeviceIDs(platforms[0], CL_DEVICE_TYPE_ALL, 10, devices, &num_devices);
        std::cout << "Num Devices: " << num_devices << "\n";

        context = clCreateContext(0, num_devices, devices, NULL, NULL, &error_code);

        command_queue = clCreateCommandQueue(context, devices[0], 0, &error_code);

        num_of_sqts = 0;
        num_of_rigidbodies = 0;
        num_of_colliders = 0;
        data = NULL;
        parents = NULL;
        rigidbody_data = NULL;
        rigidbody_parents = NULL;
        collider_data = NULL;
        collider_parents = NULL;
        add_pre_draw_function(std::bind(&topaz::opencl::update, this, std::placeholders::_1));
    }

    opencl::opencl(std::initializer_list<string> files, std::initializer_list<string> _kernels)
    {
        error_code = clGetPlatformIDs(10, platforms, &num_platforms);
        std::cout << "Num Platforms: " << num_platforms << "\n";

        error_code = clGetDeviceIDs(platforms[0], CL_DEVICE_TYPE_ALL, 10, devices, &num_devices);
        std::cout << "Num Devices: " << num_devices << "\n";

        context = clCreateContext(0, num_devices, devices, NULL, NULL, &error_code);

        command_queue = clCreateCommandQueue(context, devices[0], 0, &error_code);

        num_of_sqts = 0;
        num_of_rigidbodies = 0;
        num_of_colliders = 0;
        data = NULL;
        parents = NULL;
        rigidbody_data = NULL;
        rigidbody_parents = NULL;
        collider_data = NULL;
        collider_parents = NULL;
        
        const char** file_list = new const char*[files.size()];
        size_t* file_sizes = new size_t[files.size()];
        {
            size_t x = 0;
            for (const string & file : files)
            {
                long file_size;
                file_list[x] = read_fully_string(("programs/" + file).c_str(), file_size);
                file_sizes[x] = file_size;
                ++x;
            }
        }
        cl_program new_program = clCreateProgramWithSource(context, files.size(), file_list, file_sizes, &error_code);
        if (error_code != CL_SUCCESS)
        {
            std::cout << "ERROR COMPILING OPENCL PROGRAMS STAGE 1\n";
        }
        cl_int compile_result = clBuildProgram(new_program, 1, devices, NULL, NULL, NULL);
        if (compile_result != CL_SUCCESS)
        {
            std::cout << "ERROR COMPILING OPENCL PROGRAMS STAGE 2\n";
            print_build_log();
        }
        program = new_program;
        
        for (const string & kernel_name : _kernels)
        {
            cl_kernel new_kernel = clCreateKernel(program, kernel_name.c_str(), &error_code);
            if (error_code != CL_SUCCESS)
            {
                std::cout << "ERROR GETTING KERNEL " << kernel_name << "\n";
            }
            kernels.insert(make_pair(kernel_name, new_kernel));
        }

        add_pre_draw_function(std::bind(&topaz::opencl::update, this, std::placeholders::_1));
    }

    opencl::~opencl()
    {
        for (pair<string, cl_kernel> kernel : kernels)
        {
            clReleaseKernel(kernel.second);
        }
        clReleaseCommandQueue(command_queue);
        clReleaseContext(context);
    }

    void opencl::print_build_log()
    {
        char* build_log;
        size_t log_size;
        clGetProgramBuildInfo(program, devices[0], CL_PROGRAM_BUILD_LOG, 0, NULL, &log_size);
        build_log = new char[log_size+1];
        clGetProgramBuildInfo(program, devices[0], CL_PROGRAM_BUILD_LOG, log_size, build_log, NULL);
        build_log[log_size] = '\0';
        std::cout << "BUILD LOG:\n  " << build_log << "\n";
        delete[] build_log;
    }

    size_t shrRoundUp(size_t small, size_t large)
    {
        size_t multiply = 1;
        while (small*multiply < large)
        {
            multiply++;
        }
        return small*multiply;
    }

    void opencl::update(int milliseconds)
    {
        float* accelerations = new float[rigidbodies.size()*3];
        float seconds = ((float)milliseconds) / ((float)1000.0);
        for (rigidbody* rigid : rigidbodies)
        {
            rigid->last_position = rigid->transform->get_t();
            rigid->push_to_cl();
            rigid->populate_acceleration(accelerations, milliseconds);
        }

        vector<float> torques;
        vector<int> torque_targets;
        for (rigidbody* rigid : rigidbodies)
        {
            for (pair<int, vec> & torque : rigid->torques)
            {
                torque_targets.push_back(rigid->rigidbody_id);
                torques.push_back(torque.second.x() * seconds);
                torques.push_back(torque.second.y() * seconds);
                torques.push_back(torque.second.z() * seconds);
                torque.first -= milliseconds;
            }
            for (const vec & torque : rigid->constant_torques)
            {
                torque_targets.push_back(rigid->rigidbody_id);
                torques.push_back(torque.x() * seconds);
                torques.push_back(torque.y() * seconds);
                torques.push_back(torque.z() * seconds);
            }
            rigid->torques.remove_if([](const pair<int, vec> & torque){return torque.first <= 0;});
        }

        cl_mem src_data = clCreateBuffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, 8*num_of_sqts*sizeof(float), data, &error_code);
        cl_mem src_rigidbody_data = clCreateBuffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, 28*num_of_rigidbodies*sizeof(float), rigidbody_data, &error_code);
        cl_mem src_accel = clCreateBuffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, 3*num_of_rigidbodies*sizeof(float), accelerations, &error_code);
        cl_mem src_rigidbody_parents = clCreateBuffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, num_of_rigidbodies*sizeof(int), rigidbody_parents, &error_code);
        cl_mem src_torques = clCreateBuffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(float)*3*torque_targets.size(), &(torques[0]), &error_code);
        cl_mem src_torque_targets = clCreateBuffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(int)*torque_targets.size(), &(torque_targets[0]), &error_code);
        cl_mem ret_data = clCreateBuffer(context, CL_MEM_WRITE_ONLY | CL_MEM_COPY_HOST_PTR, 8*num_of_sqts*sizeof(float), data, &error_code);
        cl_mem ret_rigidbody_data = clCreateBuffer(context, CL_MEM_COPY_HOST_PTR, 28*num_of_rigidbodies*sizeof(float), rigidbody_data, &error_code);

        int _num_of_rigidbodies = num_of_rigidbodies;
        int num_of_torques = torque_targets.size();
        const size_t local_ws = 512;
        const size_t global_ws = shrRoundUp(local_ws, num_of_rigidbodies);

        #if PHYSICS_METHOD == 0
        cl_kernel func = kernels["explicit_euler_numerical_integration"];
        #endif
        #if PHYSICS_METHOD == 1
        cl_kernel func = kernels["maths"];
        #endif

        error_code = clSetKernelArg(func, 0, sizeof(cl_mem), &src_data);
        error_code |= clSetKernelArg(func, 1, sizeof(cl_mem), &src_rigidbody_data);
        error_code |= clSetKernelArg(func, 2, sizeof(cl_mem), &src_accel);
        error_code |= clSetKernelArg(func, 3, sizeof(cl_mem), &src_rigidbody_parents);
        error_code |= clSetKernelArg(func, 4, sizeof(cl_mem), &ret_data);
        error_code |= clSetKernelArg(func, 5, sizeof(cl_mem), &ret_rigidbody_data);
        error_code |= clSetKernelArg(func, 6, sizeof(float), &seconds);
        error_code |= clSetKernelArg(func, 7, sizeof(int), &_num_of_rigidbodies);
        error_code |= clSetKernelArg(func, 8, sizeof(cl_mem), &src_torques);
        error_code |= clSetKernelArg(func, 9, sizeof(cl_mem), &src_torque_targets);
        error_code |= clSetKernelArg(func, 10, sizeof(int), &num_of_torques);

        if (error_code != CL_SUCCESS)
            std::cout << "Failed to assign arguments\n";

        error_code = clEnqueueNDRangeKernel(command_queue, func, 1, NULL, &global_ws, &local_ws, 0, NULL, NULL);
        if (error_code != CL_SUCCESS)
            std::cout << "Failed to clEnqueueNDRangeKernel\n";

        error_code = clEnqueueReadBuffer(command_queue, ret_data, CL_TRUE, 0, 8*num_of_sqts*sizeof(float), data, 0, NULL, NULL);
        if (error_code != CL_SUCCESS)
            std::cout << "Failed to read sqt data\n";

        float* local_to_world_matricies = new float[16*sizeof(float)*num_of_sqts];
        float* world_to_local_matricies = new float[16*sizeof(float)*num_of_sqts];
        for (int x = 0; x < num_of_sqts; ++x)
        {
            opencl_sqt* cur_sqt = get_sqt(x);
            matrix l_to_w = cur_sqt->to_matrix();
            memcpy(&local_to_world_matricies[16*x], &l_to_w[0], 16*sizeof(float));
            matrix w_to_l = l_to_w.inverse();
            memcpy(&world_to_local_matricies[16*x], &w_to_l[0], 16*sizeof(float));
        }
        cl_mem src_w_to_l = clCreateBuffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, 16*num_of_sqts*sizeof(float), world_to_local_matricies, &error_code);
        cl_mem src_l_to_w = clCreateBuffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, 16*num_of_sqts*sizeof(float), local_to_world_matricies, &error_code);
        cl_mem src_colliders = clCreateBuffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, 8*num_of_colliders*sizeof(float), collider_data, &error_code);
        cl_mem src_collider_sqts = clCreateBuffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, num_of_colliders*sizeof(int), collider_parents, &error_code);
        cl_mem src_collider_rigidbodies = clCreateBuffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, num_of_colliders*sizeof(int), collider_rigidbodies, &error_code);
        int* is_colliding = new int[num_of_colliders];
        for (int x = 0; x < num_of_colliders; ++x) is_colliding[x] = 0;
        cl_mem ret_is_colliding = clCreateBuffer(context, CL_MEM_WRITE_ONLY | CL_MEM_COPY_HOST_PTR, num_of_colliders*sizeof(int), is_colliding, &error_code);

        cl_kernel collision_func = kernels["check_for_collisions"];
        error_code = clSetKernelArg(collision_func, 0, sizeof(cl_mem), &src_w_to_l);
        error_code |= clSetKernelArg(collision_func, 1, sizeof(cl_mem), &src_l_to_w);
        int _num_of_sqts = num_of_sqts;
        error_code |= clSetKernelArg(collision_func, 2, sizeof(int), &_num_of_sqts);
        error_code |= clSetKernelArg(collision_func, 3, sizeof(cl_mem), &src_colliders);
        error_code |= clSetKernelArg(collision_func, 4, sizeof(cl_mem), &src_collider_sqts);
        error_code |= clSetKernelArg(collision_func, 5, sizeof(cl_mem), &src_collider_rigidbodies);
        error_code |= clSetKernelArg(collision_func, 6, sizeof(cl_mem), &src_rigidbody_parents);
        error_code |= clSetKernelArg(collision_func, 7, sizeof(cl_mem), &ret_rigidbody_data);
        error_code |= clSetKernelArg(collision_func, 8, sizeof(cl_mem), &ret_is_colliding);
        if (error_code != CL_SUCCESS)
            std::cout << "Failed to assign arguments\n";

        const size_t sub_global_ws = shrRoundUp(local_ws, num_of_colliders);
        error_code = clEnqueueNDRangeKernel(command_queue, collision_func, 1, NULL, &sub_global_ws, &local_ws, 0, NULL, NULL);
        if (error_code != CL_SUCCESS)
            std::cout << "Failed to clEnqueueNDRangeKernel\n";

        error_code = clEnqueueReadBuffer(command_queue, ret_rigidbody_data, CL_TRUE, 0, 28*num_of_rigidbodies*sizeof(float), rigidbody_data, 0, NULL, NULL);
        if (error_code != CL_SUCCESS)
            std::cout << "Failed to read rigidbody data\n";

        error_code = clEnqueueReadBuffer(command_queue, ret_is_colliding, CL_TRUE, 0, num_of_colliders*sizeof(int), is_colliding, 0, NULL, NULL);
        if (error_code != CL_SUCCESS)
            std::cout << "Failed to read rigidbody data\n";

        for (int x = 0; x < num_of_colliders; ++x)
        {
            colliders[x]->display_as_colliding(is_colliding[x] == 1);
            if (is_colliding[x] == 1)
            {
                rigidbodies[collider_rigidbodies[x]]->transform->set_t(rigidbodies[collider_rigidbodies[x]]->last_position);
            }
        }

        clReleaseMemObject(src_data);
        clReleaseMemObject(src_rigidbody_data);
        clReleaseMemObject(src_accel);
        clReleaseMemObject(src_rigidbody_parents);
        clReleaseMemObject(ret_data);
        clReleaseMemObject(ret_rigidbody_data);
        clReleaseMemObject(src_w_to_l);
        clReleaseMemObject(src_l_to_w);
        clReleaseMemObject(src_colliders);
        clReleaseMemObject(src_collider_sqts);
        clReleaseMemObject(src_collider_rigidbodies);
        clReleaseMemObject(src_torques);
        clReleaseMemObject(src_torque_targets);

        delete [] accelerations;
        delete [] local_to_world_matricies;
        delete [] world_to_local_matricies;
        delete [] is_colliding;

        for (rigidbody* rigid : rigidbodies)
        {
            rigid->pull_from_cl();
            //rigid->check_bounds();
        }
    }

    void opencl::increase_size_to(size_t new_size)
    {
        float* new_data = new float[new_size * OPENCL_SQT_SIZE];
        if (data != NULL)
            memcpy(new_data, data, num_of_sqts * OPENCL_SQT_SIZE * sizeof(float));
        int* new_parents = new int[new_size];
        if (parents != NULL)
            memcpy(new_parents, parents, num_of_sqts * sizeof(int));
        
        for (size_t x = num_of_sqts; x < new_size; ++x)
        {
            size_t offset = x * OPENCL_SQT_SIZE;
            new_data[offset] = 1.0f;
            new_data[offset+1] = 0.0f;
            new_data[offset+2] = 0.0f;
            new_data[offset+3] = 0.0f;
            new_data[offset+4] = 1.0f;
            new_data[offset+5] = 0.0f;
            new_data[offset+6] = 0.0f;
            new_data[offset+7] = 0.0f;
            new_parents[x] = -1;
        }

        num_of_sqts = new_size;
        delete [] data;
        delete [] parents;
        data = new_data;
        parents = new_parents;
    }

    void opencl::increase_rigidbody_size_to(size_t new_size)
    {
        float* new_rigidbody_data = new float[new_size * OPENCL_RIGIDBODY_SIZE];
        if (rigidbody_data != NULL)
            memcpy(new_rigidbody_data, rigidbody_data, num_of_rigidbodies * OPENCL_RIGIDBODY_SIZE * sizeof(float));
        int* new_rigidbody_parents = new int[new_size];
        if (rigidbody_parents != NULL)
            memcpy(new_rigidbody_parents, rigidbody_parents, num_of_rigidbodies * sizeof(int));
        
        for (size_t x = num_of_rigidbodies; x < new_size; ++x)
        {
            size_t offset = x * OPENCL_SQT_SIZE;
            // new_data[offset] = 1.0f;
            // new_data[offset+1] = 0.0f;
            // new_data[offset+2] = 0.0f;
            // new_data[offset+3] = 0.0f;
            // new_data[offset+4] = 1.0f;
            // new_data[offset+5] = 0.0f;
            // new_data[offset+6] = 0.0f;
            // new_data[offset+7] = 0.0f;
            new_rigidbody_parents[x] = -1;
        }

        num_of_rigidbodies = new_size;
        delete [] rigidbody_data;
        delete [] rigidbody_parents;
        rigidbody_data = new_rigidbody_data;
        rigidbody_parents = new_rigidbody_parents;
    }

    void opencl::increase_collider_size_to(size_t new_size)
    {
        float* new_collider_data = new float[new_size * OPENCL_COLLIDER_SIZE];
        if (collider_data != NULL)
            memcpy(new_collider_data, collider_data, num_of_colliders * OPENCL_COLLIDER_SIZE * sizeof(float));
        int* new_collider_parents = new int[new_size];
        if (collider_parents != NULL)
            memcpy(new_collider_parents, collider_parents, num_of_colliders * sizeof(int));
        
        int* new_collider_rigidbodies = new int[new_size];
        if (collider_rigidbodies != NULL)
            memcpy(new_collider_rigidbodies, collider_rigidbodies, num_of_colliders * sizeof(int));
        
        for (size_t x = num_of_rigidbodies; x < new_size; ++x)
        {
            size_t offset = x * OPENCL_SQT_SIZE;
            // new_data[offset] = 1.0f;
            // new_data[offset+1] = 0.0f;
            // new_data[offset+2] = 0.0f;
            // new_data[offset+3] = 0.0f;
            // new_data[offset+4] = 1.0f;
            // new_data[offset+5] = 0.0f;
            // new_data[offset+6] = 0.0f;
            // new_data[offset+7] = 0.0f;
            new_collider_parents[x] = -1;
        }

        num_of_colliders = new_size;
        delete [] collider_data;
        delete [] collider_parents;
        delete [] collider_rigidbodies;
        collider_data = new_collider_data;
        collider_parents = new_collider_parents;
        collider_rigidbodies = new_collider_rigidbodies;
    }

    float& opencl::get_s(int sqt_id)
    {
        return data[sqt_id*OPENCL_SQT_SIZE];
    }

    float& opencl::get_x(int sqt_id)
    {
        return data[sqt_id*OPENCL_SQT_SIZE+5];
    }

    float& opencl::get_y(int sqt_id)
    {
        return data[sqt_id*OPENCL_SQT_SIZE+6];
    }

    float& opencl::get_z(int sqt_id)
    {
        return data[sqt_id*OPENCL_SQT_SIZE+7];
    }

    float& opencl::get_qx(int sqt_id)
    {
        return data[sqt_id*OPENCL_SQT_SIZE+2];
    }

    float& opencl::get_qy(int sqt_id)
    {
        return data[sqt_id*OPENCL_SQT_SIZE+3];
    }

    float& opencl::get_qz(int sqt_id)
    {
        return data[sqt_id*OPENCL_SQT_SIZE+4];
    }

    float& opencl::get_qw(int sqt_id)
    {
        return data[sqt_id*OPENCL_SQT_SIZE+1];
    }

    int& opencl::get_parent(int sqt_id)
    {
        return parents[sqt_id];
    }

    int& opencl::get_rigidbody_parent(int rigidbody_id)
    {
        return rigidbody_parents[rigidbody_id];
    }

    int& opencl::get_collider_parent(int collider_id)
    {
        return collider_parents[collider_id];
    }

    int& opencl::get_collider_rigidbody(int collider_id)
    {
        return collider_rigidbodies[collider_id];
    }

    bool opencl::has_parent(int sqt_id)
    {
        return get_parent(sqt_id) != -1;
    }

    opencl_sqt* opencl::get_sqt(int sqt_id)
    {
        return sqts[sqt_id];
    }

    rigidbody* opencl::get_rigidbody(int rigidbody_id)
    {
        return rigidbodies[rigidbody_id];
    }

    collider* opencl::get_collider(int collider_id)
    {
        return colliders[collider_id];
    }

    float& opencl::get_use_gravity(int rigidbody_id)
    {
        return rigidbody_data[rigidbody_id*OPENCL_RIGIDBODY_SIZE+0];
    }

    float* opencl::get_center_of_mass(int rigidbody_id)
    {
        return &rigidbody_data[rigidbody_id*OPENCL_RIGIDBODY_SIZE+1];
    }

    float* opencl::get_velocity(int rigidbody_id)
    {
        return &rigidbody_data[rigidbody_id*OPENCL_RIGIDBODY_SIZE+4];
    }

    float& opencl::get_mass(int rigidbody_id)
    {
        return rigidbody_data[rigidbody_id*OPENCL_RIGIDBODY_SIZE+7];
    }

    float& opencl::get_elasticity(int rigidbody_id)
    {
        return rigidbody_data[rigidbody_id*OPENCL_RIGIDBODY_SIZE+8];
    }

    float* opencl::get_I(int rigidbody_id)
    {
        return &rigidbody_data[rigidbody_id*OPENCL_RIGIDBODY_SIZE+9];
    }

    float* opencl::get_angular_momentum(int rigidbody_id)
    {
        return &rigidbody_data[rigidbody_id*OPENCL_RIGIDBODY_SIZE+25];
    }

    float& opencl::get_radius(int collider_id)
    {
        return collider_data[collider_id*OPENCL_COLLIDER_SIZE+1];
    }

    float& opencl::get_collider_type(int collider_id)
    {
        return collider_data[collider_id*OPENCL_COLLIDER_SIZE];
    }

    float* opencl::get_lesser_corner(int collider_id)
    {
        return &collider_data[collider_id*OPENCL_COLLIDER_SIZE+2];
    }

    float* opencl::get_greater_corner(int collider_id)
    {
        return &collider_data[collider_id*OPENCL_COLLIDER_SIZE+5];
    }
}
