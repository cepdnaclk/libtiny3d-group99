#ifndef MATH3D_H
#define MATH3D_H

#include <math.h>
#include <stdbool.h>

typedef struct {
    // Cartesian coordinates
    float x, y, z;
    
    // Spherical coordinates (automatically updated)
    float r, theta, phi;
} vec3_t;

typedef struct {
    float m[4][4]; // Column-major 4x4 matrix
} mat4_t;

// Vector operations
vec3_t vec3_create(float x, float y, float z);
vec3_t vec3_from_spherical(float r, float theta, float phi);
void vec3_update_spherical(vec3_t* v);
void vec3_update_cartesian(vec3_t* v);
vec3_t vec3_normalize_fast(vec3_t v);
vec3_t vec3_slerp(vec3_t a, vec3_t b, float t);
float vec3_dot(vec3_t a, vec3_t b);
vec3_t vec3_cross(vec3_t a, vec3_t b);

// Matrix operations
mat4_t mat4_identity();
mat4_t mat4_translate(float tx, float ty, float tz);
mat4_t mat4_scale(float sx, float sy, float sz);
mat4_t mat4_rotate_xyz(float rx, float ry, float rz);
mat4_t mat4_frustum_asymmetric(float left, float right, float bottom, float top, float near, float far);
mat4_t mat4_multiply(mat4_t a, mat4_t b);
vec3_t mat4_transform_vec3(mat4_t m, vec3_t v);

#endif // MATH3D_H