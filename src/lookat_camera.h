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
#ifndef LOOKAT_CAMERA_H_
#define LOOKAT_CAMERA_H_

#include "camera.h"
#include "point.h"
#include "vector.h"
#include "matrix.h"

namespace topaz
{
    class lookat_camera : public camera
    {
      public:
        lookat_camera();
        ~lookat_camera();

        virtual matrix to_matrix();
        virtual point get_position();

        void set_location(point new_location) {location = new_location;}
        void add_location(const point & other) {location = location + other;}
        void slide(const point & other) {location = location + other; target = target + other;}
        const point& get_target();

      private:
        point location;
        point target;
        vec up;
    };
}

#endif
