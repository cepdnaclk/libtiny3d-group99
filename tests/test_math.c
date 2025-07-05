#include "math3d.h"
#include <stdio.h>
#include <math.h>

#define PI 3.14159265358979323846f

void test_vector_operations() {
    printf("=== Testing Vector Operations ===\n");
    
    // Test spherical to Cartesian conversion
    vec3_t v = vec3_from_spherical(5.0f, PI/4, PI/3);
    printf("Spherical (5,π/4,π/3) -> Cartesian: (%0.2f, %0.2f, %0.2f)\n", v.x, v.y, v.z);
    
    // Test fast normalization
    vec3_t v1 = {3.0f, 1.0f, 2.0f};
    vec3_normalize_fast(&v1);
    printf("Normalized [3,1,2]: (%0.3f, %0.3f, %0.3f)\n", v1.x, v1.y, v1.z);
    
    // Test slerp
    vec3_t a = {1.0f, 0.0f, 0.0f};
    vec3_t b = {0.0f, 1.0f, 0.0f};
    vec3_t s = vec3_slerp(a, b, 0.5f);
    printf("Slerp halfway between X and Y: (%0.3f, %0.3f, %0.3f)\n", s.x, s.y, s.z);
}

void test_matrix_operations() {
    printf("\n=== Testing Matrix Operations ===\n");
    
    // Test translation
    mat4_t trans = mat4_translate(2.0f, 3.0f, 4.0f);
    vec3_t v = {1.0f, 1.0f, 1.0f};
    vec3_t vt = mat4_mul_vec3(trans, v);
    printf("Translate (2,3,4) * (1,1,1): (%0.1f, %0.1f, %0.1f)\n", vt.x, vt.y, vt.z);
    
    // Test rotation
    mat4_t rot = mat4_rotate_xyz(0.0f, PI/2, 0.0f);
    vec3_t vr = mat4_mul_vec3(rot, v);
    printf("Rotate 90°Y * (1,1,1): (%0.3f, %0.3f, %0.3f)\n", vr.x, vr.y, vr.z);
    
    // Test projection
    mat4_t proj = mat4_frustum_asymmetric(-1.0f, 1.0f, -1.0f, 1.0f, 1.0f, 100.0f);
    vec3_t vp = {0.0f, 0.0f, -5.0f};
    vec3_t v_proj = mat4_mul_vec3(proj, vp);
    printf("Projected (0,0,-5): (%0.3f, %0.3f, %0.3f)\n", v_proj.x, v_proj.y, v_proj.z);
}

void test_cube_transform() {
    printf("\n=== Testing Cube Transformation ===\n");
    
    // Cube vertices
    vec3_t cube[] = {
        {-1,-1,-1}, {1,-1,-1}, {1,1,-1}, {-1,1,-1},  // Back face
        {-1,-1,1}, {1,-1,1}, {1,1,1}, {-1,1,1}       // Front face
    };
    
    // Transform: scale, rotate, translate
    mat4_t S = mat4_scale(0.5f, 0.5f, 0.5f);
    mat4_t R = mat4_rotate_xyz(0.3f, 0.5f, 0.2f);
    mat4_t T = mat4_translate(0.0f, 0.0f, -3.0f);
    mat4_t mvp = mat4_mul(mat4_mul(T, R), S);
    
    printf("Transformed Cube Vertices:\n");
    for (int i = 0; i < 8; i++) {
        vec3_t v = mat4_mul_vec3(mvp, cube[i]);
        printf("v%d: (%6.2f, %6.2f, %6.2f)\n", i, v.x, v.y, v.z);
    }
}

int main() {
    test_vector_operations();
    test_matrix_operations();
    test_cube_transform();
    return 0;
}