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
#include "opencl_sqt.h"
#include <math.h>
#include <iostream>
#include "opencl_accelerator.h"

using std::cout;
using std::endl;

namespace topaz
{
    opencl_sqt::opencl_sqt()
    {
        accelerator = global_accelerator;
        sqt_id = accelerator->num_of_sqts;
        accelerator->increase_size_to(sqt_id+1);
        accelerator->sqts.push_back(this);
        reset();
    }

    opencl_sqt::opencl_sqt(opencl* _accelerator)
    {
        accelerator = _accelerator;
        sqt_id = accelerator->num_of_sqts;
        accelerator->increase_size_to(sqt_id+1);
        accelerator->sqts.push_back(this);
        reset();
    }

    opencl_sqt::opencl_sqt(opencl_sqt* _parent)
    {
        accelerator = _parent->accelerator;
        sqt_id = accelerator->num_of_sqts;
        accelerator->increase_size_to(sqt_id+1);
        accelerator->sqts.push_back(this);
        accelerator->get_parent(sqt_id) = _parent->sqt_id;
        reset();
    }

    opencl_sqt::~opencl_sqt()
    {

    }

    matrix opencl_sqt::to_matrix()
    {
        matrix ret;
        quaternion tmp_q = get_q();
        matrix rotate_matrix = tmp_q.to_matrix();
        ret.translateXYZ(accelerator->get_x(sqt_id), accelerator->get_y(sqt_id), accelerator->get_z(sqt_id));
        ret = ret * rotate_matrix;
        ret.scale(accelerator->get_s(sqt_id));
        if (accelerator->has_parent(sqt_id))
            ret = accelerator->get_sqt(accelerator->get_parent(sqt_id))->to_matrix() * ret;
        return ret;
    }

    opencl_sqt& opencl_sqt::translateX(const float x)
    {
        accelerator->get_x(sqt_id) += x;
        return *this;
    }

    opencl_sqt& opencl_sqt::translateY(const float y)
    {
        accelerator->get_y(sqt_id) += y;
        return *this;
    }

    opencl_sqt& opencl_sqt::translateZ(const float z)
    {
        accelerator->get_z(sqt_id) += z;
        return *this;
    }

    opencl_sqt& opencl_sqt::rotateH(const float h)
    {
        quaternion quat;
        quat.x() = sin(h/2);
        quat.w() = cos(h/2);
        quat.normalize();
        quaternion tmp_q = get_q();
        tmp_q = tmp_q * quat;
        set_q(tmp_q);
        return *this;
    }

    opencl_sqt& opencl_sqt::rotateP(const float p)
    {
        quaternion quat;
        quat.y() = sin(p/2);
        quat.w() = cos(p/2);
        quat.normalize();
        quaternion tmp_q = get_q();
        tmp_q = tmp_q * quat;
        set_q(tmp_q);
        return *this;
    }

    opencl_sqt& opencl_sqt::rotateR(const float r)
    {
        quaternion quat;
        quat.z() = sin(r/2);
        quat.w() = cos(r/2);
        quat.normalize();
        quaternion tmp_q = get_q();
        tmp_q = tmp_q * quat;
        set_q(tmp_q);
        return *this;
    }

    opencl_sqt& opencl_sqt::rotateHPR(const float h, const float p, const float r)
    {
        rotateH(h);
        rotateP(p);
        rotateR(r);
        return *this;
    }

    opencl_sqt& opencl_sqt::scale(const float sc)
    {
        accelerator->get_s(sqt_id) *= sc;
        return *this;
    }

    opencl_sqt& opencl_sqt::translateXYZ(const float x, const float y, const float z)
    {
        accelerator->get_x(sqt_id) = accelerator->get_x(sqt_id) + x;
        accelerator->get_y(sqt_id) = accelerator->get_y(sqt_id) + y;
        accelerator->get_z(sqt_id) = accelerator->get_z(sqt_id) + z;
        return *this;
    }

    opencl_sqt& opencl_sqt::translateXYZ(const vec & vec)
    {
        accelerator->get_x(sqt_id) = accelerator->get_x(sqt_id) + vec[0];
        accelerator->get_y(sqt_id) = accelerator->get_y(sqt_id) + vec[1];
        accelerator->get_z(sqt_id) = accelerator->get_z(sqt_id) + vec[2];
        return *this;
    }

    opencl_sqt& opencl_sqt::transformXYZHPRS(const float x, const float y, const float z, const float h, const float p, const float r, const float sc)
    {
        rotateH(h);
        rotateP(p);
        rotateR(r);
        scale(sc);
        accelerator->get_x(sqt_id) = accelerator->get_x(sqt_id) + x;
        accelerator->get_y(sqt_id) = accelerator->get_y(sqt_id) + y;
        accelerator->get_z(sqt_id) = accelerator->get_z(sqt_id) + z;
        return *this;
    }

    opencl_sqt& opencl_sqt::reset()
    {
        accelerator->get_s(sqt_id) = 1.0f;
        accelerator->get_qx(sqt_id) = 0.0f;
        accelerator->get_qy(sqt_id) = 0.0f;
        accelerator->get_qz(sqt_id) = 0.0f;
        accelerator->get_qw(sqt_id) = 1.0f;
        accelerator->get_x(sqt_id) = 0.0f;
        accelerator->get_y(sqt_id) = 0.0f;
        accelerator->get_z(sqt_id) = 0.0f;
        return *this;
    }

    float opencl_sqt::get_world_s()
    {
        float ret = accelerator->get_s(sqt_id);

        if (accelerator->has_parent(sqt_id))
        {
            ret *= accelerator->get_sqt(accelerator->get_parent(sqt_id))->get_world_s();
        }
        return ret;
    }

    quaternion opencl_sqt::get_q()
    {
        quaternion ret;
        ret.x() = accelerator->get_qx(sqt_id);
        ret.y() = accelerator->get_qy(sqt_id);
        ret.z() = accelerator->get_qz(sqt_id);
        ret.w() = accelerator->get_qw(sqt_id);
        return ret;
    }

    opencl_sqt& opencl_sqt::set_q(const quaternion & new_q)
    {
        accelerator->get_qx(sqt_id) = new_q.x();
        accelerator->get_qy(sqt_id) = new_q.y();
        accelerator->get_qz(sqt_id) = new_q.z();
        accelerator->get_qw(sqt_id) = new_q.w();
        return *this;
    }

    vec opencl_sqt::get_t()
    {
        vec ret;
        ret.x() = accelerator->get_x(sqt_id);
        ret.y() = accelerator->get_y(sqt_id);
        ret.z() = accelerator->get_z(sqt_id);
        return ret;
    }

    opencl_sqt& opencl_sqt::set_t(const vec & pos)
    {
        accelerator->get_x(sqt_id) = pos.x();
        accelerator->get_y(sqt_id) = pos.y();
        accelerator->get_z(sqt_id) = pos.z();
        return *this;
    }

    float& opencl_sqt::get_x() {return accelerator->get_x(sqt_id);}
    float& opencl_sqt::get_y() {return accelerator->get_y(sqt_id);}
    float& opencl_sqt::get_z() {return accelerator->get_z(sqt_id);}
    float& opencl_sqt::get_s() {return accelerator->get_s(sqt_id);}
    float& opencl_sqt::get_qx() {return accelerator->get_qx(sqt_id);}
    float& opencl_sqt::get_qy() {return accelerator->get_qy(sqt_id);}
    float& opencl_sqt::get_qz() {return accelerator->get_qz(sqt_id);}
    float& opencl_sqt::get_qw() {return accelerator->get_qw(sqt_id);}
}
