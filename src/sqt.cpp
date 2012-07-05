#include "sqt.h"
#include <math.h>
#include <iostream>

using std::cout;
using std::endl;

namespace topaz
{
    sqt::sqt()
    {
        parent = NULL;
        reset();
    }

    sqt::sqt(sqt* _parent)
    {
        parent = _parent;
        reset();
    }

    sqt::~sqt()
    {

    }

    matrix sqt::to_matrix()
    {
        matrix ret;
        matrix rotate_matrix = q.to_matrix();
        ret.translateXYZ(t[0],t[1],t[2]);
        ret = ret * rotate_matrix;
        ret.scale(s);
        if (parent != NULL)
            ret = parent->to_matrix() * ret;
        return ret;
    }

    sqt& sqt::translateX(const float x)
    {
        t[0] += x;
        return *this;
    }

    sqt& sqt::translateY(const float y)
    {
        t[1] += y;
        return *this;
    }

    sqt& sqt::translateZ(const float z)
    {
        t[2] += z;
        return *this;
    }

    sqt& sqt::rotateH(const float h)
    {
        quaternion quat;
        quat.x() = sin(h/2);
        quat.w() = cos(h/2);
        quat.normalize();
        q = q * quat;
        return *this;
    }

    sqt& sqt::rotateP(const float p)
    {
        quaternion quat;
        quat.y() = sin(p/2);
        quat.w() = cos(p/2);
        quat.normalize();
        q = q * quat;
        return *this;
    }

    sqt& sqt::rotateR(const float r)
    {
        quaternion quat;
        quat.z() = sin(r/2);
        quat.w() = cos(r/2);
        quat.normalize();
        q = q * quat;
        return *this;
    }

    sqt& sqt::rotateHPR(const float h, const float p, const float r)
    {
        rotateH(h);
        rotateP(p);
        rotateR(r);
        return *this;
    }

    sqt& sqt::scale(const float sc)
    {
        s *= sc;
        return *this;
    }

    sqt& sqt::translateXYZ(const float x, const float y, const float z)
    {
        t[0] = t[0] + x;
        t[1] = t[1] + y;
        t[2] = t[2] + z;
        return *this;
    }

    sqt& sqt::translateXYZ(const vec & vec)
    {
        t[0] = t[0] + vec[0];
        t[1] = t[1] + vec[1];
        t[2] = t[2] + vec[2];
        return *this;
    }

    sqt& sqt::transformXYZHPRS(const float x, const float y, const float z, const float h, const float p, const float r, const float sc)
    {
        rotateH(h);
        rotateP(p);
        rotateR(r);
        scale(sc);
        t[0] = t[0] + x;
        t[1] = t[1] + y;
        t[2] = t[2] + z;
        return *this;
    }

    sqt& sqt::reset()
    {
        s = 1.0f;
        q.x() = 0.0f;
        q.y() = 0.0f;
        q.z() = 0.0f;
        q.w() = 1.0f;
        t[0] = 0;
        t[1] = 0;
        t[2] = 0;
        return *this;
    }

    float sqt::get_world_s()
    {
        if (parent == NULL)
            return s;
        else
            return s * parent->get_world_s();
    }
}
