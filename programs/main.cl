void handle_torques(
    __global const float* rigidbody_data,
    __global float* ret_rigidbody_data,
    __global const float* torques,
    __global const int* torque_targets,
    const int num,
    const int num_of_torques);
void immovable_collision(const int rigidbody_id, __global float* ret_rigidbody_data, const float* norm);
void immovable_collision_i(const int rigidbody_id, __global float* ret_rigidbody_data, const float norm_x, const float norm_y, const float norm_z);
void check_bounds(__global float* ret_data, __global float* ret_rigidbody_data, const int sqt_id);
float4 matrix_vector_multiply(__global const float* matrix, const int matrix_offset, __global const float* vec, const int vec_offset);
float dot_float_product(__global float* ret_rigidbody_data, const int offset_1, const float4 other);
bool is_colliding_box_box(__global const float* world_to_local,  __global const float* local_to_world, __global const float* colliders, __global const int* collider_sqts, const int a, const int b, float4* collision_location);
bool is_colliding_sphere_sphere(__global const float* world_to_local,  __global const float* local_to_world, __global const float* colliders, __global const int* collider_sqts, const int a, const int b, float4* collision_location);
float4 matrix_float4_multiply(__global const float* matrix, const int matrix_offset, const float4 vec);
float4 handle_linear_collision(const int rigidbody_a, const int rigidbody_b, __global float* ret_rigidbody_data,  __global const float* world_to_local, __global const float* local_to_world, __global const int* rigidbody_parents, const float4 collision_location);

__kernel void explicit_euler_numerical_integration(
    __global const float* src_data,
    __global const float* rigidbody_data,
    __global const float* accelerations,
    __global const int* rigidbody_parents,
    __global float* ret_data,
    __global float* ret_rigidbody_data,
    const float seconds,
    const int num,
    __global const float* torques,
    __global const int* torque_targets,
    const int num_of_torques)
{
    const int idx = get_global_id(0);

    if (idx >= num)
    {
        return;
    }

    //velocity x
    ret_rigidbody_data[idx*28+4] = rigidbody_data[idx*28+4] + accelerations[idx*3+0] * seconds;
    //velocity y
    ret_rigidbody_data[idx*28+5] = rigidbody_data[idx*28+5] + accelerations[idx*3+1] * seconds;
    //velocity z
    ret_rigidbody_data[idx*28+6] = rigidbody_data[idx*28+6] + accelerations[idx*3+2] * seconds;

    int sqt_id = rigidbody_parents[idx];
    //position x
    ret_data[sqt_id*8+5] = src_data[sqt_id*8+5] + ret_rigidbody_data[idx*28+4] * seconds;
    //position y
    ret_data[sqt_id*8+6] = src_data[sqt_id*8+6] + ret_rigidbody_data[idx*28+5] * seconds;
    //position z
    ret_data[sqt_id*8+7] = src_data[sqt_id*8+7] + ret_rigidbody_data[idx*28+6] * seconds;

    handle_torques(rigidbody_data, ret_rigidbody_data, torques, torque_targets, num, num_of_torques);
    check_bounds(ret_data, ret_rigidbody_data, sqt_id);
}

__kernel void maths(
    __global const float* src_data,
    __global const float* rigidbody_data,
    __global const float* accelerations,
    __global const int* rigidbody_parents,
    __global float* ret_data,
    __global float* ret_rigidbody_data,
    const float seconds,
    const int num,
    __global const float* torques,
    __global const int* torque_targets,
    const int num_of_torques)
{
    const int idx = get_global_id(0);

    if (idx >= num)
    {
        return;
    }

    //velocity x
    ret_rigidbody_data[idx*28+4] = rigidbody_data[idx*28+4] + accelerations[idx*3+0] * seconds;
    //velocity y
    ret_rigidbody_data[idx*28+5] = rigidbody_data[idx*28+5] + accelerations[idx*3+1] * seconds;
    //velocity z
    ret_rigidbody_data[idx*28+6] = rigidbody_data[idx*28+6] + accelerations[idx*3+2] * seconds;

    int sqt_id = rigidbody_parents[idx];
    //position x
    ret_data[sqt_id*8+5] = accelerations[idx*3+0] * seconds * seconds * 0.5 + src_data[sqt_id*8+5] + ret_rigidbody_data[idx*28+4] * seconds;
    //position y
    ret_data[sqt_id*8+6] = accelerations[idx*3+1] * seconds * seconds * 0.5 + src_data[sqt_id*8+6] + ret_rigidbody_data[idx*28+5] * seconds;
    //position z
    ret_data[sqt_id*8+7] = accelerations[idx*3+2] * seconds * seconds * 0.5 + src_data[sqt_id*8+7] + ret_rigidbody_data[idx*28+6] * seconds;

    handle_torques(rigidbody_data, ret_rigidbody_data, torques, torque_targets, num, num_of_torques);
    check_bounds(ret_data, ret_rigidbody_data, sqt_id);
}

void handle_torques(
    __global const float* rigidbody_data,
    __global float* ret_rigidbody_data,
    __global const float* torques,
    __global const int* torque_targets,
    const int num,
    const int num_of_torques)
{
    const int idx = get_global_id(0);

    if (idx >= num)
    {
        return;
    }

    for (int x = 0; x < num_of_torques; ++x)
    {
        if (torque_targets[x] != idx)
            continue;

        ret_rigidbody_data[idx*28+25] = rigidbody_data[idx*28+25] + torques[x*3+0];
        ret_rigidbody_data[idx*28+26] = rigidbody_data[idx*28+26] + torques[x*3+1];
        ret_rigidbody_data[idx*28+27] = rigidbody_data[idx*28+27] + torques[x*3+2];
    }
}

__kernel void check_for_collisions(__global const float* world_to_local,  __global const float* local_to_world, const int num_of_colliders, __global const float* colliders, __global const int* collider_sqts, __global const int* collider_rigidbodies, __global const int* rigidbody_parents, __global float* ret_rigidbody_data, __global int* is_colliding)
{
    const int idx = get_global_id(0);

    if (idx >= num_of_colliders)
        return;

    //only higher numbers can collider with lower numbers
    for (int x = 0; x < idx; ++x)
    {
        if (x == idx)
            continue;
        if (colliders[idx*8] == 0 && colliders[x*8] == 0)
        {
            //sphere sphere collision
            float4 collision_location;
            if (is_colliding_sphere_sphere(world_to_local, local_to_world, colliders, collider_sqts, idx, x, &collision_location))
            {
                is_colliding[idx] = 1;
                is_colliding[x] = 1;
                float4 force = handle_linear_collision(collider_rigidbodies[idx], collider_rigidbodies[x], ret_rigidbody_data, world_to_local, local_to_world, rigidbody_parents, collision_location);
            }
        } else if (colliders[idx*8] == 1.0f && colliders[x*8] == 1.0f) {
            //box box collision
            float4 collision_location;
            if (is_colliding_box_box(world_to_local, local_to_world, colliders, collider_sqts, idx, x, &collision_location))
            {
                is_colliding[idx] = 1;
                is_colliding[x] = 1;
                float4 force = handle_linear_collision(collider_rigidbodies[idx], collider_rigidbodies[x], ret_rigidbody_data, world_to_local, local_to_world, rigidbody_parents, collision_location);
            } else if (is_colliding_box_box(world_to_local, local_to_world, colliders, collider_sqts, x, idx, &collision_location)) {
                is_colliding[idx] = 1;
                is_colliding[x] = 1;
                float4 force = handle_linear_collision(collider_rigidbodies[idx], collider_rigidbodies[x], ret_rigidbody_data, world_to_local, local_to_world, rigidbody_parents, collision_location);
            }
        }
        
    }
}

bool is_colliding_box_box(__global const float* world_to_local,  __global const float* local_to_world, __global const float* colliders, __global const int* collider_sqts, const int a, const int b, float4* collision_location)
{
    int sqt_b = collider_sqts[b];
    int sqt_a = collider_sqts[a];
    

    float4 b_lesser_corner;
    b_lesser_corner.x = colliders[8*b+2];
    b_lesser_corner.y = colliders[8*b+3];
    b_lesser_corner.z = colliders[8*b+4];
    float4 b_lesser_corner_world = matrix_float4_multiply(local_to_world, 16*sqt_b, b_lesser_corner);
    float4 b_lesser_corner_a = matrix_float4_multiply(world_to_local, 16*sqt_a, b_lesser_corner_world);

    float4 b_greater_corner;
    b_greater_corner.x = colliders[8*b+5];
    b_greater_corner.y = colliders[8*b+6];
    b_greater_corner.z = colliders[8*b+7];
    float4 b_greater_corner_world = matrix_float4_multiply(local_to_world, 16*sqt_b, b_greater_corner);
    float4 b_greater_corner_a = matrix_float4_multiply(world_to_local, 16*sqt_a, b_greater_corner_world);

    if (b_lesser_corner_a.x > b_greater_corner_a.x)
    {
        float tmp = b_lesser_corner_a.x;
        b_lesser_corner_a.x = b_greater_corner_a.x;
        b_greater_corner_a.x = tmp;
    }

    if (b_lesser_corner_a.y > b_greater_corner_a.y)
    {
        float tmp = b_lesser_corner_a.y;
        b_lesser_corner_a.y = b_greater_corner_a.y;
        b_greater_corner_a.y = tmp;
    }

    if (b_lesser_corner_a.z > b_greater_corner_a.z)
    {
        float tmp = b_lesser_corner_a.z;
        b_lesser_corner_a.z = b_greater_corner_a.z;
        b_greater_corner_a.z = tmp;
    }

    float4 tmp_collision;
    
    if ((b_greater_corner_a.x >= colliders[8*a+2] && b_greater_corner_a.x <= colliders[8*a+5]) && (b_lesser_corner_a.x >= colliders[8*a+2] && b_lesser_corner.x <= colliders[8*a+5]))
    {
        //other fully contained
        tmp_collision.x = b_greater_corner_a.x/2.0f;
    } else if(b_lesser_corner_a.x >= colliders[8*a+2] && b_lesser_corner_a.x <= colliders[8*a+5]) {
        //lesser contained
        tmp_collision.x = b_lesser_corner_a.x + (colliders[8*a+5] - b_lesser_corner_a.x)/2.0f;
    } else if(b_greater_corner_a.x >= colliders[8*a+2] && b_greater_corner_a.x <= colliders[8*a+5]) {
        //greater contained
        tmp_collision.x = colliders[8*a+2] + (b_greater_corner_a.x - colliders[8*a+2])/2.0f;
    } else {
        return false;
    }

    if ((b_greater_corner_a.y >= colliders[8*a+3] && b_greater_corner_a.y <= colliders[8*a+6]) && (b_lesser_corner_a.y >= colliders[8*a+3] && b_lesser_corner.y <= colliders[8*a+6]))
    {
        //other fully contained
        tmp_collision.y = b_greater_corner_a.y/2.0f;
    } else if(b_lesser_corner_a.y >= colliders[8*a+3] && b_lesser_corner_a.y <= colliders[8*a+6]) {
        //lesser contained
        tmp_collision.y = b_lesser_corner_a.y + (colliders[8*a+6] - b_lesser_corner_a.y)/2.0f;
    } else if(b_greater_corner_a.y >= colliders[8*a+3] && b_greater_corner_a.y <= colliders[8*a+6]) {
        //greater contained
        tmp_collision.y = colliders[8*a+3] + (b_greater_corner_a.y - colliders[8*a+3])/2.0f;
    } else {
        return false;
    }

    if ((b_greater_corner_a.z >= colliders[8*a+4] && b_greater_corner_a.z <= colliders[8*a+7]) && (b_lesser_corner_a.z >= colliders[8*a+4] && b_lesser_corner.z <= colliders[8*a+7]))
    {
        //other fully contained
        tmp_collision.z = b_greater_corner_a.z/2.0f;
    } else if(b_lesser_corner_a.z >= colliders[8*a+4] && b_lesser_corner_a.z <= colliders[8*a+7]) {
        //lesser contained
        tmp_collision.z = b_lesser_corner_a.z + (colliders[8*a+7] - b_lesser_corner_a.z)/2.0f;
    } else if(b_greater_corner_a.z >= colliders[8*a+4] && b_greater_corner_a.z <= colliders[8*a+7]) {
        //greater contained
        tmp_collision.z = colliders[8*a+4] + (b_greater_corner_a.z - colliders[8*a+4])/2.0f;
    } else {
        return false;
    }

    float4 collision_in_world = matrix_float4_multiply(local_to_world, 16*sqt_a, tmp_collision);
    (*collision_location) = collision_in_world;

    return true;
}

bool is_colliding_sphere_sphere(__global const float* world_to_local,  __global const float* local_to_world, __global const float* colliders, __global const int* collider_sqts, const int a, const int b, float4* collision_location)
{
    return false;
}

void immovable_collision(const int rigidbody_id, __global float* ret_rigidbody_data, const float* norm)
{
    float scalar = ret_rigidbody_data[rigidbody_id*28+8] + 1.0f;
    float dot = ret_rigidbody_data[rigidbody_id*28+4]*norm[0] + ret_rigidbody_data[rigidbody_id*28+5]*norm[1] + ret_rigidbody_data[rigidbody_id*28+6]*norm[2];
    scalar *= dot;
    ret_rigidbody_data[rigidbody_id*28+4] = ret_rigidbody_data[rigidbody_id*28+4] - norm[0]*scalar;
    ret_rigidbody_data[rigidbody_id*28+5] = ret_rigidbody_data[rigidbody_id*28+5] - norm[1]*scalar;
    ret_rigidbody_data[rigidbody_id*28+6] = ret_rigidbody_data[rigidbody_id*28+6] - norm[2]*scalar;
}
void immovable_collision_i(const int rigidbody_id, __global float* ret_rigidbody_data, const float norm_x, const float norm_y, const float norm_z)
{
    float scalar = ret_rigidbody_data[rigidbody_id*28+8] + 1.0f;
    float dot = ret_rigidbody_data[rigidbody_id*28+4]*norm_x + ret_rigidbody_data[rigidbody_id*28+5]*norm_y + ret_rigidbody_data[rigidbody_id*28+6]*norm_z;
    scalar *= dot;
    ret_rigidbody_data[rigidbody_id*28+4] = ret_rigidbody_data[rigidbody_id*28+4] - norm_x*scalar;
    ret_rigidbody_data[rigidbody_id*28+5] = ret_rigidbody_data[rigidbody_id*28+5] - norm_y*scalar;
    ret_rigidbody_data[rigidbody_id*28+6] = ret_rigidbody_data[rigidbody_id*28+6] - norm_z*scalar;
}

void check_bounds(__global float* ret_data, __global float* ret_rigidbody_data, const int sqt_id)
{
    const int idx = get_global_id(0);

    if (ret_data[sqt_id*8+6] < 0.0f)
    {
        ret_data[sqt_id*8+6] = 0.0f;
        immovable_collision_i(idx, ret_rigidbody_data, 0.0f, 1.0f, 0.0f);
    }
    if (ret_data[sqt_id*8+5] < -25.0f)
    {
        ret_data[sqt_id*8+5] = -25.0f;
        immovable_collision_i(idx, ret_rigidbody_data, 1.0f, 0.0f, 0.0f);
    }
    if (ret_data[sqt_id*8+5] > 25.0f)
    {
        ret_data[sqt_id*8+5] = 25.0f;
        immovable_collision_i(idx, ret_rigidbody_data, -1.0f, 0.0f, 0.0f);
    }
    if (ret_data[sqt_id*8+7] < -25.0f)
    {
        ret_data[sqt_id*8+7] = -25.0f;
        immovable_collision_i(idx, ret_rigidbody_data, 0.0f, 0.0f, 1.0f);
    }
    if (ret_data[sqt_id*8+7] > 25.0f)
    {
        ret_data[sqt_id*8+7] = 25.0f;
        immovable_collision_i(idx, ret_rigidbody_data, 0.0f, 0.0f, -1.0f);
    }
}

float4 handle_linear_collision(const int rigidbody_a, const int rigidbody_b, __global float* ret_rigidbody_data,  __global const float* world_to_local, __global const float* local_to_world, __global const int* rigidbody_parents, const float4 collision_location)
{
    /*
     * Calculate vectors
     */
    int sqt_a = rigidbody_parents[rigidbody_a];
    int sqt_b = rigidbody_parents[rigidbody_b];
    float4 a_com = matrix_vector_multiply(local_to_world, 16*sqt_a, ret_rigidbody_data, 28*rigidbody_a+1);
    float4 b_com = matrix_vector_multiply(local_to_world, 16*sqt_b, ret_rigidbody_data, 28*rigidbody_b+1);
    float4 norm = normalize(a_com - collision_location);

    /*
     * Calculate p hat
     */
    float bottom = 1.0f/ret_rigidbody_data[28*rigidbody_a+7] + 1.0f/ret_rigidbody_data[28*rigidbody_b+7];
    float top = (1.0f + ret_rigidbody_data[28*rigidbody_a+8]) * (dot_float_product(ret_rigidbody_data, 28*rigidbody_b+4, norm) - (dot_float_product(ret_rigidbody_data, 28*rigidbody_a+4, norm)));
    float italic_p_hat = top/bottom;

    /*
     * Calculate new velocities
     */
    ret_rigidbody_data[28*rigidbody_a+4] += norm.x*(italic_p_hat/ret_rigidbody_data[28*rigidbody_a+7]);
    ret_rigidbody_data[28*rigidbody_a+5] += norm.y*(italic_p_hat/ret_rigidbody_data[28*rigidbody_a+7]);
    ret_rigidbody_data[28*rigidbody_a+6] += norm.z*(italic_p_hat/ret_rigidbody_data[28*rigidbody_a+7]);
    ret_rigidbody_data[28*rigidbody_b+4] -= norm.x*(italic_p_hat/ret_rigidbody_data[28*rigidbody_b+7]);
    ret_rigidbody_data[28*rigidbody_b+5] -= norm.y*(italic_p_hat/ret_rigidbody_data[28*rigidbody_b+7]);
    ret_rigidbody_data[28*rigidbody_b+6] -= norm.z*(italic_p_hat/ret_rigidbody_data[28*rigidbody_b+7]);

    float4 ret;
    ret.x = norm.x*italic_p_hat;
    ret.y = norm.y*italic_p_hat;
    ret.z = norm.z*italic_p_hat;
    return ret;
}

float4 handle_angular_collision(const int rigidbody_a, const int rigidbody_b, __global float* ret_rigidbody_data,  __global const float* world_to_local, __global const float* local_to_world, __global const int* rigidbody_parents, const float4 collision_location, const float4 collision_force)
{
    int sqt_a = rigidbody_parents[rigidbody_a];
    int sqt_b = rigidbody_parents[rigidbody_b];
    float4 a_com = matrix_vector_multiply(local_to_world, 16*sqt_a, ret_rigidbody_data, 28*rigidbody_a+1);
    float4 b_com = matrix_vector_multiply(local_to_world, 16*sqt_b, ret_rigidbody_data, 28*rigidbody_b+1);
    
    float4 a_r = collision_location - a_com;
    float4 b_r = collision_location - b_com;

    //TODO: torques
    /* torques.push_back(make_pair(0, this_r.cross(collision_force))); */
    /* other.into->torques.push_back(make_pair(0, other_r.cross(collision_force*-1))); */
}

float4 matrix_vector_multiply(__global const float* matrix, const int matrix_offset, __global const float* vec, const int vec_offset)
{
    float4 ret;
    ret.x = matrix[matrix_offset+0]*vec[vec_offset+0] + matrix[matrix_offset+4]*vec[vec_offset+1] + matrix[matrix_offset+8]*vec[vec_offset+2] + matrix[matrix_offset+12]*vec[vec_offset+3];
    ret.y = matrix[matrix_offset+1]*vec[vec_offset+0] + matrix[matrix_offset+5]*vec[vec_offset+1] + matrix[matrix_offset+9]*vec[vec_offset+2] + matrix[matrix_offset+13]*vec[vec_offset+3];
    ret.z = matrix[matrix_offset+2]*vec[vec_offset+0] + matrix[matrix_offset+6]*vec[vec_offset+1] + matrix[matrix_offset+10]*vec[vec_offset+2] + matrix[matrix_offset+14]*vec[vec_offset+3];
    ret.w = matrix[matrix_offset+3]*vec[vec_offset+0] + matrix[matrix_offset+7]*vec[vec_offset+1] + matrix[matrix_offset+11]*vec[vec_offset+2] + matrix[matrix_offset+15]*vec[vec_offset+3];
    return ret;
}

float4 matrix_float4_multiply(__global const float* matrix, const int matrix_offset, const float4 vec)
{
    float4 ret;
    ret.x = matrix[matrix_offset+0]*vec.x + matrix[matrix_offset+4]*vec.y + matrix[matrix_offset+8]*vec.z + matrix[matrix_offset+12]*vec.w;
    ret.y = matrix[matrix_offset+1]*vec.x + matrix[matrix_offset+5]*vec.y + matrix[matrix_offset+9]*vec.z + matrix[matrix_offset+13]*vec.w;
    ret.z = matrix[matrix_offset+2]*vec.x + matrix[matrix_offset+6]*vec.y + matrix[matrix_offset+10]*vec.z + matrix[matrix_offset+14]*vec.w;
    ret.w = matrix[matrix_offset+3]*vec.x + matrix[matrix_offset+7]*vec.y + matrix[matrix_offset+11]*vec.z + matrix[matrix_offset+15]*vec.w;
    return ret;
}

float dot_product(__global float* ret_rigidbody_data, const int offset_1, const int offset_2)
{
    return ret_rigidbody_data[offset_1]*ret_rigidbody_data[offset_2] + ret_rigidbody_data[offset_1+1]*ret_rigidbody_data[offset_2+1] + ret_rigidbody_data[offset_1+2]*ret_rigidbody_data[offset_2+2];
}
float dot_float_product(__global float* ret_rigidbody_data, const int offset_1, const float4 other)
{
    return ret_rigidbody_data[offset_1]*other.x + ret_rigidbody_data[offset_1+1]*other.y + ret_rigidbody_data[offset_1+2]*other.z;
}

