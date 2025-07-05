#include <math.h>
#include <string.h>
#include "../include/math3d.h"

vec3_t vec3_from_spherical(float r, float theta, float phi) {
    vec3_t v;
    v.x = r * sinf(theta) * cosf(phi);
    v.y = r * sinf(theta) * sinf(phi);
    v.z = r * cosf(theta);
    return v;
}

vec3_t vec3_normalize_fast(vec3_t v) {
    float length_sq = v.x * v.x + v.y * v.y + v.z * v.z;
    if (length_sq == 0) return v;

    // Standard-compliant fast inverse sqrt approximation
    float y = length_sq;
    long i;
    memcpy(&i, &y, sizeof(long));
    i = 0x5f3759df - (i >> 1);
    memcpy(&y, &i, sizeof(long));
    y = y * (1.5f - (length_sq * 0.5f * y * y));

    v.x *= y;
    v.y *= y;
    v.z *= y;
    return v;
}

vec3_t vec3_slerp(vec3_t a, vec3_t b, float t) {
    a = vec3_normalize_fast(a);
    b = vec3_normalize_fast(b);
    float dot = vec3_dot(a, b);
    if (dot > 0.9995f) {
        return vec3_add(vec3_scale(a, 1.0f - t), vec3_scale(b, t));
    }

    dot = fmaxf(-1.0f, fminf(1.0f, dot));
    float theta = acosf(dot);
    float sin_theta = sinf(theta);

    float w1 = sinf((1.0f - t) * theta) / sin_theta;
    float w2 = sinf(t * theta) / sin_theta;

    vec3_t result = vec3_add(vec3_scale(a, w1), vec3_scale(b, w2));
    return result;
}

float vec3_dot(vec3_t a, vec3_t b) {
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

vec3_t vec3_cross(vec3_t a, vec3_t b) {
    vec3_t r = {{
        a.y * b.z - a.z * b.y,
        a.z * b.x - a.x * b.z,
        a.x * b.y - a.y * b.x
    }};
    return r;
}

vec3_t vec3_sub(vec3_t a, vec3_t b) {
    vec3_t r = {{ a.x - b.x, a.y - b.y, a.z - b.z }};
    return r;
}

vec3_t vec3_add(vec3_t a, vec3_t b) {
    vec3_t r = {{ a.x + b.x, a.y + b.y, a.z + b.z }};
    return r;
}

vec3_t vec3_scale(vec3_t v, float s) {
    vec3_t r = {{ v.x * s, v.y * s, v.z * s }};
    return r;
}

float vec3_length(vec3_t v) {
    return sqrtf(v.x * v.x + v.y * v.y + v.z * v.z);
}

mat4_t mat4_identity() {
    mat4_t m = {0};
    for (int i = 0; i < 4; i++) {
        m.m[i * 4 + i] = 1.0f;
    }
    return m;
}

mat4_t mat4_translate(float tx, float ty, float tz) {
    mat4_t m = mat4_identity();
    m.m[12] = tx;
    m.m[13] = ty;
    m.m[14] = tz;
    return m;
}

mat4_t mat4_scale(float sx, float sy, float sz) {
    mat4_t m = {0};
    m.m[0] = sx;
    m.m[5] = sy;
    m.m[10] = sz;
    m.m[15] = 1.0f;
    return m;
}

mat4_t mat4_rotate_xyz(float rx, float ry, float rz) {
    mat4_t mx = mat4_identity();
    mat4_t my = mat4_identity();
    mat4_t mz = mat4_identity();

    float sx = sinf(rx), cx = cosf(rx);
    float sy = sinf(ry), cy = cosf(ry);
    float sz = sinf(rz), cz = cosf(rz);

    mx.m[5] = cx; mx.m[6] = -sx;
    mx.m[9] = sx; mx.m[10] = cx;

    my.m[0] = cy; my.m[2] = sy;
    my.m[8] = -sy; my.m[10] = cy;

    mz.m[0] = cz; mz.m[1] = -sz;
    mz.m[4] = sz; mz.m[5] = cz;

    return mat4_mul(mz, mat4_mul(my, mx));
}

mat4_t mat4_frustum_asymmetric(float left, float right, float bottom, float top, float near, float far) {
    mat4_t m = {0};
    m.m[0] = (2 * near) / (right - left);
    m.m[5] = (2 * near) / (top - bottom);
    m.m[8] = (right + left) / (right - left);
    m.m[9] = (top + bottom) / (top - bottom);
    m.m[10] = -(far + near) / (far - near);
    m.m[11] = -1;
    m.m[14] = -(2 * far * near) / (far - near);
    return m;
}

mat4_t mat4_mul(mat4_t a, mat4_t b) {
    mat4_t r = {0};
    for (int row = 0; row < 4; row++) {
        for (int col = 0; col < 4; col++) {
            for (int k = 0; k < 4; k++) {
                r.m[col * 4 + row] += a.m[k * 4 + row] * b.m[col * 4 + k];
            }
        }
    }
    return r;
}

vec3_t mat4_transform_point(mat4_t m, vec3_t v) {
    float x = v.x, y = v.y, z = v.z;
    float w = m.m[3] * x + m.m[7] * y + m.m[11] * z + m.m[15];

    vec3_t r;
    r.x = (m.m[0] * x + m.m[4] * y + m.m[8] * z + m.m[12]) / w;
    r.y = (m.m[1] * x + m.m[5] * y + m.m[9] * z + m.m[13]) / w;
    r.z = (m.m[2] * x + m.m[6] * y + m.m[10] * z + m.m[14]) / w;
    return r;
}

vec3_t mat4_transform_direction(mat4_t m, vec3_t v) {
    vec3_t r;
    r.x = m.m[0] * v.x + m.m[4] * v.y + m.m[8] * v.z;
    r.y = m.m[1] * v.x + m.m[5] * v.y + m.m[9] * v.z;
    r.z = m.m[2] * v.x + m.m[6] * v.y + m.m[10] * v.z;
    return r;
}