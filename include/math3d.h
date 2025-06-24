#ifndef MATH3D_H
#define MATH3D_H

#include <math.h>

// === 3D Vector ===
typedef struct {
    union {
        struct { float x, y, z; };         // Cartesian coordinates
        struct { float r, theta, phi; };   // Spherical coordinates (r, θ, φ)
    };
} vec3_t;

/**
 * Creates a 3D vector from spherical coordinates.
 * θ is the polar angle from the +Z axis, φ is the azimuthal angle in the X-Y plane.
 */
vec3_t vec3_from_spherical(float r, float theta, float phi);

/**
 * Normalizes a vector to unit length using a fast inverse square root approximation.
 */
vec3_t vec3_normalize_fast(vec3_t v);

/**
 * Performs spherical linear interpolation (slerp) between two direction vectors.
 * 't' must be between 0 and 1.
 */
vec3_t vec3_slerp(vec3_t a, vec3_t b, float t);

/**
 * Computes the dot product of two vectors.
 */
float vec3_dot(vec3_t a, vec3_t b);

/**
 * Computes the cross product of two vectors.
 */
vec3_t vec3_cross(vec3_t a, vec3_t b);

/**
 * Subtracts two vectors: result = a - b
 */
vec3_t vec3_sub(vec3_t a, vec3_t b);

/**
 * Adds two vectors: result = a + b
 */
vec3_t vec3_add(vec3_t a, vec3_t b);

/**
 * Multiplies a vector by a scalar.
 */
vec3_t vec3_scale(vec3_t v, float s);

/**
 * Computes the length of a vector.
 */
float vec3_length(vec3_t v);

// === 4x4 Matrix ===
typedef struct {
    float m[16];  // Column-major layout: m[col * 4 + row]
} mat4_t;

/**
 * Returns the identity matrix.
 */
mat4_t mat4_identity();

/**
 * Creates a translation matrix.
 */
mat4_t mat4_translate(float tx, float ty, float tz);

/**
 * Creates a scaling matrix.
 */
mat4_t mat4_scale(float sx, float sy, float sz);

/**
 * Creates a rotation matrix around X, Y, Z axes (in radians).
 */
mat4_t mat4_rotate_xyz(float rx, float ry, float rz);

/**
 * Creates an asymmetric frustum projection matrix.
 */
mat4_t mat4_frustum_asymmetric(float left, float right, float bottom, float top, float near, float far);

/**
 * Multiplies two 4x4 matrices: result = a * b
 */
mat4_t mat4_mul(mat4_t a, mat4_t b);

/**
 * Transforms a vec3 by a matrix (assumes w = 1).
 */
vec3_t mat4_transform_point(mat4_t m, vec3_t v);

/**
 * Transforms a vec3 direction (assumes w = 0).
 */
vec3_t mat4_transform_direction(mat4_t m, vec3_t v);

#endif // MATH3D_H
