#ifndef TYPES_H_
#define TYPES_H_

#if defined __GNUG__ && !(defined __clang__)
#define GCC
#warning Using GCC, swizzles will be slow, you should try clang!
    #endif
#if defined __clang__
#define CLANG
    #endif

typedef float v16sf __attribute__ ((vector_size (sizeof(float)*16)));
typedef float v4sf __attribute__ ((vector_size (sizeof(float)*4)));
typedef int v4si __attribute__ ((vector_size (sizeof(int)*4)));
typedef union matrix_data
{
    v16sf full_matrix;
    v4sf matrix_vectors[4];
    float matrix_floats[16];
} matrix_data;

typedef union vector_data
{
    v4sf full_vector;
    float vector_floats[4];
} vector_data;

static const double PI = 3.14159265358979323846;
#define FLOAT_TOLERANCE 0.00001f
#define STOPPING_SPEED_TOLERANCE 1.0f
#define INERTIA 0.1f
#define TARGET_RADIUS 1.0f
#endif
