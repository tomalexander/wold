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
#ifndef OPENCL_SQT_H_
#define OPENCL_SQT_H_

#include "quaternion.h"
#include "matrix.h"
#include "vector.h"

namespace topaz {
    class opencl;

    class opencl_sqt {
      public:
        opencl_sqt();
        opencl_sqt(opencl* _accelerator);
        opencl_sqt(opencl_sqt* _parent);
        ~opencl_sqt();

        matrix to_matrix();
        opencl_sqt& translateX(const float x);
        opencl_sqt& translateY(const float y);
        opencl_sqt& translateZ(const float z);
        opencl_sqt& rotateH(const float h);
        opencl_sqt& rotateP(const float p);
        opencl_sqt& rotateR(const float r);
        opencl_sqt& rotateHPR(const float h, const float p, const float r);
        opencl_sqt& scale(const float sc);
        opencl_sqt& translateXYZ(const float x, const float y, const float z);
        opencl_sqt& translateXYZ(const vec & vec);
        opencl_sqt& transformXYZHPRS(const float x, const float y, const float z, const float h, const float p, const float r, const float sc);
        opencl_sqt& reset();
        float get_world_s();
        quaternion get_q();
        opencl_sqt& set_q(const quaternion & new_q);
        vec get_t();
        opencl_sqt& set_t(const vec & pos);

        float& get_x();
        float& get_y();
        float& get_z();
        float& get_s();
        float& get_qx();
        float& get_qy();
        float& get_qz();
        float& get_qw();


        int sqt_id;
        opencl* accelerator;

      private:
    };

}

#endif
