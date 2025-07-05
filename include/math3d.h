#ifndef MATH3D_H
#define MATH3D_H

#include <stdint.h>
#include <math.h>

/* 3D Vector structure with both Cartesian and Spherical coordinates */
typedef struct {
    float x, y, z;      // Cartesian coordinates
    float r, theta, phi; // Spherical coordinates
} vec3_t;

/* 4x4 Matrix structure (column-major) */
typedef struct {
    float m[4][4];  // [column][row] format
} mat4_t;

/* Vector operations */
vec3_t vec3_from_spherical(float r, float theta, float phi);
void vec3_normalize_fast(vec3_t* v);
vec3_t vec3_slerp(vec3_t a, vec3_t b, float t);

/* Matrix operations */
mat4_t mat4_identity(void);
mat4_t mat4_translate(float tx, float ty, float tz);
mat4_t mat4_scale(float sx, float sy, float sz);
mat4_t mat4_rotate_xyz(float rx, float ry, float rz);
mat4_t mat4_mul(mat4_t a, mat4_t b);
mat4_t mat4_frustum_asymmetric(float left, float right, float bottom, float top, float near, float far);

/* Vector math utilities */
float vec3_dot(vec3_t a, vec3_t b);
vec3_t vec3_cross(vec3_t a, vec3_t b);
vec3_t vec3_add(vec3_t a, vec3_t b);
vec3_t vec3_sub(vec3_t a, vec3_t b);
vec3_t vec3_scale(vec3_t a, float s);

/* Matrix-vector operations */
vec3_t mat4_mul_vec3(mat4_t m, vec3_t v);

/* Model creation functions */
void create_cube(vec3_t** vertices, int** edges, int* vertex_count, int* edge_count);

#endif // MATH3D_H