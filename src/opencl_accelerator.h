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
#ifndef OPENCL_ACCELERATOR_H_
#define OPENCL_ACCELERATOR_H_

#include "def.h"
#include "topaz.h"
#include "CL/opencl.h"
#include <vector>

#define OPENCL_SQT_SIZE 8
#define OPENCL_RIGIDBODY_SIZE 28
#define OPENCL_COLLIDER_SIZE 8

using std::vector;

namespace topaz
{
    class opencl_sqt;
    class rigidbody;
    class collider;
    class opencl
    {
      public:
        opencl();
        opencl(std::initializer_list<string> files, std::initializer_list<string> kernels);
        ~opencl();

        void print_build_log();

        void update(int milliseconds);
        void increase_size_to(size_t new_size);
        void increase_rigidbody_size_to(size_t new_size);
        void increase_collider_size_to(size_t new_size);
        float& get_s(int sqt_id);
        float& get_x(int sqt_id);
        float& get_y(int sqt_id);
        float& get_z(int sqt_id);
        float& get_qx(int sqt_id);
        float& get_qy(int sqt_id);
        float& get_qz(int sqt_id);
        float& get_qw(int sqt_id);
        float& get_use_gravity(int rigidbody_id);
        float* get_center_of_mass(int rigidbody_id);
        float* get_velocity(int rigidbody_id);
        float& get_mass(int rigidbody_id);
        float& get_elasticity(int rigidbody_id);
        float* get_I(int rigidbody_id);
        float* get_angular_momentum(int rigidbody_id);
        float& get_radius(int collider_id);
        float& get_collider_type(int collider_id);
        float* get_lesser_corner(int collider_id);
        float* get_greater_corner(int collider_id);

        int& get_parent(int sqt_id);
        int& get_rigidbody_parent(int rigidbody_id);
        int& get_collider_parent(int collider_id);
        int& get_collider_rigidbody(int collider_id);
        bool has_parent(int sqt_id);
        opencl_sqt* get_sqt(int sqt_id);
        rigidbody* get_rigidbody(int rigidbody_id);
        collider* get_collider(int collider_id);

        cl_platform_id platforms[10];
        cl_uint num_platforms;
        cl_int error_code;

        cl_device_id devices[10];
        cl_uint num_devices;

        cl_context context;

        cl_command_queue command_queue;

        size_t num_of_sqts;
        size_t num_of_rigidbodies;
        size_t num_of_colliders;
        float* data;
        float* rigidbody_data;
        float* collider_data;
        int* parents;
        int* rigidbody_parents;
        int* collider_parents;
        int* collider_rigidbodies;
        vector<opencl_sqt*> sqts;
        vector<rigidbody*> rigidbodies;
        vector<collider*> colliders;
        cl_program program;
        map<string, cl_kernel> kernels;
      private:
    };

    #ifndef OPENCL_ACCELERATOR_CPP_
    extern opencl* global_accelerator;
    #endif
}

#endif
