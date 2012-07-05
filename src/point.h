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
#ifndef POINT_H_
#define POINT_H_

#include "types.h"
#include "vector.h"
#include <iomanip>
#include <iostream>

namespace topaz
{
    class point
    {
      public:
        point() {data.vector_floats[0] = 0.0f;
            data.vector_floats[1] = 0.0f;
            data.vector_floats[2] = 0.0f;
            sanitize();}
        point(const point & other): data(other.data) {}
        point(const vec & other) {data.vector_floats[0] = other[0];
            data.vector_floats[1] = other[1];
            data.vector_floats[2] = other[2];
            sanitize();}
        point(const v4sf & other) {data.full_vector = other;}
        point(float x, float y, float z) {data.vector_floats[0] = x;
            data.vector_floats[1] = y;
            data.vector_floats[2] = z;
            sanitize();}

        inline void sanitize() {data.vector_floats[3] = 1.0f;}
        
        float& operator[](size_t i) {return data.vector_floats[i];}
        const float& operator[](size_t i) const {return data.vector_floats[i];}
        float& x() {return data.vector_floats[0];}
        float& y() {return data.vector_floats[1];}
        float& z() {return data.vector_floats[2];}
        const float& x() const {return data.vector_floats[0];}
        const float& y() const {return data.vector_floats[1];}
        const float& z() const {return data.vector_floats[2];}

        point operator*(const point& other) const {return point(data.full_vector*other.data.full_vector);}
        point operator*(const float & other) const {vector_data tmp = {other, other, other, other};
            return point(data.full_vector*tmp.full_vector);}
        point operator/(const point& other) const {return point(data.full_vector/other.data.full_vector);}
        point operator/(const float other) const {v4sf div = {other, other, other, other};
            return point(data.full_vector / div);}
        vec operator-(const point& other) const {vec ret(data.full_vector-other.data.full_vector);
            ret.sanitize();
            return ret;}
        point operator+(const point& other) const {point ret(data.full_vector+other.data.full_vector);
            ret.sanitize();
            return ret;}
        point& operator+=(const point& other) {data.full_vector += other.data.full_vector;
            sanitize();
            return *this;}
        vector_data data;

        void print(std::ostream & out = std::cout) const
        {
            out << "POINT:" << std::endl;
            for (int y = 0; y < 4; ++y)
            {
                if (y != 0)
                    out << "\t";
                out << std::setw(8) << (*this)[y];
            }
            out << std::endl;
        }
      private:
        
    };
}
#endif
