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
#ifndef RIGIDBODY_H_
#define RIGIDBODY_H_
#include "def.h"
#include "topaz.h"
#include "vector.h"
#include "gameobject.h"
#include "point.h"
#include "quaternion.h"
#include <utility>
#include "sphere_collider.h"
#include "collision.h"
#include "sqt.h"

using std::pair;
using std::make_pair;

/*
 * Meters, Kilograms, Seconds... use it
 */

namespace topaz
{
    class unit;
    class collider;

    enum TENSOR {SPHERE_TENSOR};

    class rigidbody : gameobject
    {
      public:
        rigidbody(sqt* _transform, unit* parent_unit, TENSOR tensor = SPHERE_TENSOR, const string & type = "SPHERE");
        ~rigidbody();

        void update(int milliseconds);
        void calculate_center_of_mass(unit* parent);
        float calculate_farthest_dimension(unit* parent);
        void calculate_axis_aligned_bounding_box(unit* parent);
        void handle_collisions(int milliseconds);
        vec handle_linear_collision(const collision & other);
        void handle_angular_collision(const collision & other, const vec & collision_force);
        void immovable_collision(const vec & norm);
        void calculate_tensor(TENSOR tensor);

        vec velocity;
        point center_of_mass;
        sqt* transform;
        bool use_gravity;
        float mass;
        float elasticity;
        vector<collider*> colliders;
        list<pair<int, vec> > forces;
        list<pair<int, vec> > torques;
        list<vec> constant_forces;
        list<vec> constant_torques;
        float farthest_dimension;
        vector<rigidbody*> handled_collisions;
        vec last_position;
        matrix I;
        vec angular_momentum;   /**< L */
        int rigidbody_id;

        point get_world_center_of_mass();
        vector<collision> check_for_collision();
        pair<bool, collision> is_colliding_with(rigidbody* other);
        void push_to_cl();
        void pull_from_cl();
        void populate_acceleration(float* accelerations, int milliseconds);
        void check_bounds();
        
      private:
        void add_force_to_acceleration(vec & acceleration, const vec & force);
        void add_torque_to_angular_momentum(const vec & torque);
        void explicit_euler_numerical_integration(const vec & acceleration, const double & seconds);
        void maths(const vec & acceleration, const double & seconds);
    };
}
#endif
