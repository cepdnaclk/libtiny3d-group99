#include <stdio.h>
#include <math.h>
#include "../include/math3d.h"

void print_vec3(const char* label, vec3_t v) {
    printf("%s: (%.3f, %.3f, %.3f)\n", label, v.x, v.y, v.z);
}

void test_spherical_conversion() {
    printf("== Spherical to Cartesian Test ==\n");
    float r = 1.0f;
    float theta = M_PI / 4.0f;
    float phi = M_PI / 6.0f;
    vec3_t v = vec3_from_spherical(r, theta, phi);
    print_vec3("vec3_from_spherical", v);
}

void test_normalization() {
    printf("\n== Normalization Test ==\n");
    vec3_t v = {3.0f, 4.0f, 0.0f};
    vec3_t n = vec3_normalize_fast(v);
    print_vec3("Original", v);
    print_vec3("Normalized", n);
    printf("Length: %.3f\n", vec3_length(n));
}

void test_slerp() {
    printf("\n== SLERP Test ==\n");
    vec3_t a = {1.0f, 0.0f, 0.0f};
    vec3_t b = {0.0f, 1.0f, 0.0f};
    for (float t = 0; t <= 1.0f; t += 0.25f) {
        vec3_t result = vec3_slerp(a, b, t);
        printf("t = %.2f -> ", t);
        print_vec3("slerp", result);
    }
}

void test_matrix_transform() {
    printf("\n== Matrix Transform Test ==\n");
    vec3_t point = {1.0f, 2.0f, 3.0f};
    mat4_t translate = mat4_translate(5.0f, 0.0f, 0.0f);
    mat4_t scale = mat4_scale(2.0f, 2.0f, 2.0f);
    mat4_t rotate = mat4_r
