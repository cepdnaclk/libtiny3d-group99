#include "math3d.h"
#include <math.h>

// Fast inverse square root approximation (from Quake III)
float Q_rsqrt(float number) {
    long i;
    float x2, y;
    const float threehalfs = 1.5F;

    x2 = number * 0.5F;
    y = number;
    i = *(long*)&y;                         // evil floating point bit level hacking
    i = 0x5f3759df - (i >> 1);               // what the fuck?
    y = *(float*)&i;
    y = y * (threehalfs - (x2 * y * y));   // 1st iteration
    // y = y * (threehalfs - (x2 * y * y)); // 2nd iteration, this can be removed

    return y;
}

vec3_t vec3_create(float x, float y, float z) {
    vec3_t v = {x, y, z, 0, 0, 0};
    vec3_update_spherical(&v);
    return v;
}

vec3_t vec3_from_spherical(float r, float theta, float phi) {
    vec3_t v = {0, 0, 0, r, theta, phi};
    vec3_update_cartesian(&v);
    return v;
}

void vec3_update_spherical(vec3_t* v) {
    v->r = sqrtf(v->x * v->x + v->y * v->y + v->z * v->z);
    if (v->r > 0) {
        v->theta = atan2f(v->y, v->x);
        v->phi = acosf(v->z / v->r);
    } else {
        v->theta = 0;
        v->phi = 0;
    }
}

void vec3_update_cartesian(vec3_t* v) {
    float sin_phi = sinf(v->phi);
    v->x = v->r * cosf(v->theta) * sin_phi;
    v->y = v->r * sinf(v->theta) * sin_phi;
    v->z = v->r * cosf(v->phi);
}

vec3_t vec3_normalize_fast(vec3_t v) {
    float inv_length = Q_rsqrt(v.x*v.x + v.y*v.y + v.z*v.z);
    v.x *= inv_length;
    v.y *= inv_length;
    v.z *= inv_length;
    vec3_update_spherical(&v);
    return v;
}

vec3_t vec3_slerp(vec3_t a, vec3_t b, float t) {
    // Normalize inputs
    a = vec3_normalize_fast(a);
    b = vec3_normalize_fast(b);
    
    float dot = vec3_dot(a, b);
    
    // Clamp dot product to handle floating point inaccuracies
    dot = fmaxf(-1.0f, fminf(1.0f, dot));
    
    float theta = acosf(dot) * t;
    vec3_t relative = vec3_create(
        b.x - a.x * dot,
        b.y - a.y * dot,
        b.z - a.z * dot
    );
    relative = vec3_normalize_fast(relative);
    
    vec3_t result = vec3_create(
        a.x * cosf(theta) + relative.x * sinf(theta),
        a.y * cosf(theta) + relative.y * sinf(theta),
        a.z * cosf(theta) + relative.z * sinf(theta)
    );
    
    return vec3_normalize_fast(result);
}

float vec3_dot(vec3_t a, vec3_t b) {
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

vec3_t vec3_cross(vec3_t a, vec3_t b) {
    return vec3_create(
        a.y * b.z - a.z * b.y,
        a.z * b.x - a.x * b.z,
        a.x * b.y - a.y * b.x
    );
}

mat4_t mat4_identity() {
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
    mat4_t m = {0};
    m.m[0][0] = sx;
    m.m[1][1] = sy;
    m.m[2][2] = sz;
    m.m[3][3] = 1.0f;
    return m;
}

mat4_t mat4_rotate_xyz(float rx, float ry, float rz) {
    mat4_t mx = mat4_identity();
    mat4_t my = mat4_identity();
    mat4_t mz = mat4_identity();
    
    // Rotation around X axis
    if (rx != 0) {
        float cosX = cosf(rx);
        float sinX = sinf(rx);
        mx.m[1][1] = cosX;
        mx.m[1][2] = -sinX;
        mx.m[2][1] = sinX;
        mx.m[2][2] = cosX;
    }
    
    // Rotation around Y axis
    if (ry != 0) {
        float cosY = cosf(ry);
        float sinY = sinf(ry);
        my.m[0][0] = cosY;
        my.m[0][2] = sinY;
        my.m[2][0] = -sinY;
        my.m[2][2] = cosY;
    }
    
    // Rotation around Z axis
    if (rz != 0) {
        float cosZ = cosf(rz);
        float sinZ = sinf(rz);
        mz.m[0][0] = cosZ;
        mz.m[0][1] = -sinZ;
        mz.m[1][0] = sinZ;
        mz.m[1][1] = cosZ;
    }
    
    // Combine rotations in ZYX order
    return mat4_multiply(mat4_multiply(mz, my), mx);
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

mat4_t mat4_multiply(mat4_t a, mat4_t b) {
    mat4_t result = {0};
    
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            result.m[i][j] = 0;
            for (int k = 0; k < 4; k++) {
                result.m[i][j] += a.m[k][j] * b.m[i][k];
            }
        }
    }
    
    return result;
}

vec3_t mat4_transform_vec3(mat4_t m, vec3_t v) {
    vec3_t result;
    float w;
    
    result.x = m.m[0][0] * v.x + m.m[1][0] * v.y + m.m[2][0] * v.z + m.m[3][0];
    result.y = m.m[0][1] * v.x + m.m[1][1] * v.y + m.m[2][1] * v.z + m.m[3][1];
    result.z = m.m[0][2] * v.x + m.m[1][2] * v.y + m.m[2][2] * v.z + m.m[3][2];
    w = m.m[0][3] * v.x + m.m[1][3] * v.y + m.m[2][3] * v.z + m.m[3][3];
    
    if (w != 0.0f) {
        result.x /= w;
        result.y /= w;
        result.z /= w;
    }
    
    vec3_update_spherical(&result);
    return result;
}