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
#include "aabb_collider.h"
#include "point.h"
#include "vector.h"
#if DRAW_COLLISION_SOLIDS == 1
#include "box_primitive.h"
#endif

namespace topaz
{
    aabb_collider::aabb_collider(sqt* _parent_transform, const point & _lesser_corner, const point & _greater_corner)
    {
        type = AABB;
        transform = new sqt(_parent_transform);
        lesser_corner = _lesser_corner;
        greater_corner = _greater_corner;
        #if DRAW_COLLISION_SOLIDS == 1
        display_box = new box_primitive(transform, vec(1,1,1,0.5), lesser_corner, greater_corner);
        #endif
        #if USE_OPENCL == 1
        collider_id = global_accelerator->num_of_colliders;
        global_accelerator->increase_collider_size_to(global_accelerator->num_of_colliders+1);
        global_accelerator->colliders.push_back(this);
        push_to_cl();
        #endif
    }

    aabb_collider::~aabb_collider()
    {
        delete transform;
        #if DRAW_COLLISION_SOLIDS == 1
        delete display_box;
        #endif
    }

    point* aabb_collider::is_colliding_with(collider* other)
    {
        switch (other->type)
        {
          default:
            break;
          case AABB:
            return AABB_collision((aabb_collider*) other);
            break;
        }
        return NULL;
    }

    point* aabb_collider::AABB_collision(aabb_collider* other)
    {
        matrix this_to_world = transform->to_matrix();
        matrix world_to_this = this_to_world.inverse();
        matrix other_to_world = other->transform->to_matrix();
        matrix world_to_other = other_to_world.inverse();
        
        vec other_diag = other->greater_corner - other->lesser_corner;
        vec other_diag_world = other_to_world * other_diag;
        vec other_diag_this = world_to_this * other_diag_world;
        
        point other_lesser_corner_world = other_to_world * other->lesser_corner;
        point other_lesser_corner_this = world_to_this * other_lesser_corner_world;

        point* ret = new point();

        if ((other_lesser_corner_this.x()+other_diag_this.x() > lesser_corner.x() && other_lesser_corner_this.x()+other_diag_this.x() < greater_corner.x()) && (other_lesser_corner_this.x() > lesser_corner.x() && other_lesser_corner_this.x() < greater_corner.x()))
        {
            //other fully contained
            ret->x() = other_lesser_corner_this.x()+other_diag_this.x()/2.0f;
        } else if (other_lesser_corner_this.x() > lesser_corner.x() && other_lesser_corner_this.x() < greater_corner.x())
        {
            //lesser contained
            ret->x() = other_lesser_corner_this.x() + (greater_corner.x()-other_lesser_corner_this.x())/2.0f;
        } else if (other_lesser_corner_this.x()+other_diag_this.x() > lesser_corner.x() && other_lesser_corner_this.x()+other_diag_this.x() < greater_corner.x())
        {
            //greater contained
            ret->x() = lesser_corner.x() + (other_lesser_corner_this.x()+other_diag_this.x()-lesser_corner.x())/2.0f;
        } else {
            delete ret;
            return NULL;
        }

        if ((other_lesser_corner_this.y()+other_diag_this.y() > lesser_corner.y() && other_lesser_corner_this.y()+other_diag_this.y() < greater_corner.y()) && (other_lesser_corner_this.y() > lesser_corner.y() && other_lesser_corner_this.y() < greater_corner.y()))
        {
            //other fully contained
            ret->y() = other_lesser_corner_this.y()+other_diag_this.y()/2.0f;
        } else if (other_lesser_corner_this.y() > lesser_corner.y() && other_lesser_corner_this.y() < greater_corner.y())
        {
            //lesser contained
            ret->y() = other_lesser_corner_this.y() + (greater_corner.y()-other_lesser_corner_this.y())/2.0f;
        } else if (other_lesser_corner_this.y()+other_diag_this.y() > lesser_corner.y() && other_lesser_corner_this.y()+other_diag_this.y() < greater_corner.y())
        {
            //greater contained
            ret->y() = lesser_corner.y() + (other_lesser_corner_this.y()+other_diag_this.y()-lesser_corner.y())/2.0f;
        } else {
            delete ret;
            return NULL;
        }

        if ((other_lesser_corner_this.z()+other_diag_this.z() > lesser_corner.z() && other_lesser_corner_this.z()+other_diag_this.z() < greater_corner.z()) && (other_lesser_corner_this.z() > lesser_corner.z() && other_lesser_corner_this.z() < greater_corner.z()))
        {
            //other fully contained
            ret->z() = other_lesser_corner_this.z()+other_diag_this.z()/2.0f;
        } else if (other_lesser_corner_this.z() > lesser_corner.z() && other_lesser_corner_this.z() < greater_corner.z())
        {
            //lesser contained
            ret->z() = other_lesser_corner_this.z() + (greater_corner.z()-other_lesser_corner_this.z())/2.0f;
        } else if (other_lesser_corner_this.z()+other_diag_this.z() > lesser_corner.z() && other_lesser_corner_this.z()+other_diag_this.z() < greater_corner.z())
        {
            //greater contained
            ret->z() = lesser_corner.z() + (other_lesser_corner_this.z()+other_diag_this.z()-lesser_corner.z())/2.0f;
        } else {
            delete ret;
            return NULL;
        }


        *ret = this_to_world * (*ret);
        
        return ret;
    }

    point aabb_collider::get_world_position()
    {
        point origin(0,0,0);
        return transform->to_matrix() * origin;
    }

    #if DRAW_COLLISION_SOLIDS == 1
    void aabb_collider::display_as_colliding(bool colliding)
    {
        if (colliding)
        {
            display_box->color = vec(1,0,0,0.5);
        } else {
            display_box->color = vec(1,1,1,0.5);
        }
    }
    #endif
}
