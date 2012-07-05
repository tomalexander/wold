#ifndef GCC_SHUFFLE_H_
#define GCC_SHUFFLE_H_

#include "types.h"

template <typename B, typename T, typename M>
M manual_swizzle(T _data, M _mask)
{
    M _ret;
    B* mask = (B*) (& _mask);
    B* data = (B*) (& _data);
    B* ret = (B*) (& _ret);
    int num_of_elements = sizeof(M) / sizeof(B);
    for (int x = 0; x < num_of_elements; ++x)
    {
        ret[x] = data[(int) mask[x]];
    }
    return _ret;
}

#endif
