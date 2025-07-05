#include "math3d.h"
#include <math.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

/* Fast inverse square root approximation */
float Q_rsqrt(float number) {
    union {
        float f;
        uint32_t i;
    } conv = {.f = number};
    
    conv.i = 0x5f3759df - (conv.i >> 1);
    conv.f *= 1.5f - (number * 0.5f * conv.f * conv.f);
    return conv.f;
}

/* Vector operations */
vec3_t vec3_from_spherical(float r, float theta, float phi) {
    vec3_t v;
    v.r = r;
    v.theta = theta;
    v.phi = phi;
    
    v.x = r * sinf(theta) * cosf(phi);
    v.y = r * sinf(theta) * sinf(phi);
    v.z = r * cosf(theta);
    
    return v;
}

void vec3_normalize_fast(vec3_t* v) {
    float inv_sqrt = Q_rsqrt(v->x*v->x + v->y*v->y + v->z*v->z);
    v->x *= inv_sqrt;
    v->y *= inv_sqrt;
    v->z *= inv_sqrt;
    
    // Update spherical coordinates
    v->r = 1.0f;
    v->theta = acosf(v->z);
    v->phi = atan2f(v->y, v->x);
}

vec3_t vec3_slerp(vec3_t a, vec3_t b, float t) {
    // Ensure inputs are normalized
    vec3_normalize_fast(&a);
    vec3_normalize_fast(&b);
    
    float dot = vec3_dot(a, b);
    dot = fmaxf(-1.0f, fminf(1.0f, dot));
    
    float theta = acosf(dot) * t;
    vec3_t relative = vec3_sub(b, vec3_scale(a, dot));
    vec3_normalize_fast(&relative);
    
    vec3_t result = vec3_add(vec3_scale(a, cosf(theta)), vec3_scale(relative, sinf(theta)));
    vec3_normalize_fast(&result);
    return result;
}

/* Matrix operations */
mat4_t mat4_identity(void) {
    mat4_t m = {0};
    m.m[0][0] = 1.0f;
    m.m[1][1] = 1.0f;
    m.m[2][2] = 1.0f;
    m.m[3][3] = 1.0f;
    return m;
}

mat4_t mat4_translate(float tx, float ty, float tz) {
    mat4_t m = mat4_identity();
    m.m[3][0] = tx;
    m.m[3][1] = ty;
    m.m[3][2] = tz;
    return m;
}

mat4_t mat4_scale(float sx, float sy, float sz) {
    mat4_t m = mat4_identity();
    m.m[0][0] = sx;
    m.m[1][1] = sy;
    m.m[2][2] = sz;
    return m;
}

mat4_t mat4_rotate_xyz(float rx, float ry, float rz) {
    mat4_t mx = mat4_identity();
    mat4_t my = mat4_identity();
    mat4_t mz = mat4_identity();
    
    float cx = cosf(rx), sx = sinf(rx);
    float cy = cosf(ry), sy = sinf(ry);
    float cz = cosf(rz), sz = sinf(rz);
    
    mx.m[1][1] = cx; mx.m[2][1] = -sx;
    mx.m[1][2] = sx; mx.m[2][2] = cx;
    
    my.m[0][0] = cy; my.m[2][0] = sy;
    my.m[0][2] = -sy; my.m[2][2] = cy;
    
    mz.m[0][0] = cz; mz.m[1][0] = -sz;
    mz.m[0][1] = sz; mz.m[1][1] = cz;
    
    return mat4_mul(mat4_mul(mz, my), mx);
}

mat4_t mat4_frustum_asymmetric(float left, float right, float bottom, float top, float near, float far) {
    mat4_t m = {0};
    
    m.m[0][0] = 2.0f * near / (right - left);
    m.m[1][1] = 2.0f * near / (top - bottom);
    
    m.m[2][0] = (right + left) / (right - left);
    m.m[2][1] = (top + bottom) / (top - bottom);
    m.m[2][2] = -(far + near) / (far - near);
    m.m[2][3] = -1.0f;
    
    m.m[3][2] = -2.0f * far * near / (far - near);
    
    return m;
}

/* Vector math utilities */
float vec3_dot(vec3_t a, vec3_t b) {
    return a.x*b.x + a.y*b.y + a.z*b.z;
}

vec3_t vec3_cross(vec3_t a, vec3_t b) {
    vec3_t v;
    v.x = a.y*b.z - a.z*b.y;
    v.y = a.z*b.x - a.x*b.z;
    v.z = a.x*b.y - a.y*b.x;
    return v;
}

vec3_t vec3_add(vec3_t a, vec3_t b) {
    vec3_t v;
    v.x = a.x + b.x;
    v.y = a.y + b.y;
    v.z = a.z + b.z;
    return v;
}

vec3_t vec3_sub(vec3_t a, vec3_t b) {
    vec3_t v;
    v.x = a.x - b.x;
    v.y = a.y - b.y;
    v.z = a.z - b.z;
    return v;
}

vec3_t vec3_scale(vec3_t a, float s) {
    vec3_t v;
    v.x = a.x * s;
    v.y = a.y * s;
    v.z = a.z * s;
    return v;
}

/* Matrix-vector operations */
vec3_t mat4_mul_vec3(mat4_t m, vec3_t v) {
    vec3_t result;
    result.x = m.m[0][0]*v.x + m.m[1][0]*v.y + m.m[2][0]*v.z + m.m[3][0];
    result.y = m.m[0][1]*v.x + m.m[1][1]*v.y + m.m[2][1]*v.z + m.m[3][1];
    result.z = m.m[0][2]*v.x + m.m[1][2]*v.y + m.m[2][2]*v.z + m.m[3][2];
    
    float w = m.m[0][3]*v.x + m.m[1][3]*v.y + m.m[2][3]*v.z + m.m[3][3];
    
    if (w != 0.0f && w != 1.0f) {
        result.x /= w;
        result.y /= w;
        result.z /= w;
    }
    
    return result;
}

mat4_t mat4_mul(mat4_t a, mat4_t b) {
    mat4_t m;
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            m.m[i][j] = a.m[i][0]*b.m[0][j] + a.m[i][1]*b.m[1][j] + 
                         a.m[i][2]*b.m[2][j] + a.m[i][3]*b.m[3][j];
        }
    }
    return m;
}

/* Model creation functions */
void create_cube(vec3_t** vertices, int** edges, int* vertex_count, int* edge_count) {
    *vertex_count = 8;
    *edge_count = 12;
    
    *vertices = (vec3_t*)malloc(*vertex_count * sizeof(vec3_t));
    *edges = (int*)malloc(*edge_count * 2 * sizeof(int));
    
    // Cube vertices
    vec3_t cube[] = {
        {-1,-1,-1}, {1,-1,-1}, {1,1,-1}, {-1,1,-1},  // Back face
        {-1,-1,1}, {1,-1,1}, {1,1,1}, {-1,1,1}       // Front face
    };
    memcpy(*vertices, cube, sizeof(cube));
    
    // Cube edges (vertex indices)
    int cube_edges[] = {
        0,1, 1,2, 2,3, 3,0,  // Back face
        4,5, 5,6, 6,7, 7,4,  // Front face
        0,4, 1,5, 2,6, 3,7   // Connecting edges
    };
    memcpy(*edges, cube_edges, sizeof(cube_edges));
}