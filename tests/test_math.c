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

void draw_ascii_wireframe(float x2d[8], float y2d[8], int disp_size) {
    char grid[40][41];
    for (int y = 0; y < disp_size; y++) {
        for (int x = 0; x < disp_size; x++) grid[y][x] = ' ';
        grid[y][disp_size] = '\0';
    }
    // Draw points
    for (int i = 0; i < 8; i++) {
        int px = (int)(x2d[i] + disp_size/2);
        int py = (int)(y2d[i] + disp_size/2);
        if (px >= 0 && px < disp_size && py >= 0 && py < disp_size)
            grid[py][px] = 'O';
    }
    // Draw edges
    int edges[12][2] = {
        {0,1},{1,2},{2,3},{3,0}, // back
        {4,5},{5,6},{6,7},{7,4}, // front
        {0,4},{1,5},{2,6},{3,7} // sides
    };
    for (int e = 0; e < 12; e++) {
        int a = edges[e][0], b = edges[e][1];
        int x0 = (int)(x2d[a] + disp_size/2), y0 = (int)(y2d[a] + disp_size/2);
        int x1 = (int)(x2d[b] + disp_size/2), y1 = (int)(y2d[b] + disp_size/2);
        int dx = abs(x1-x0), dy = abs(y1-y0), sx = x0<x1?1:-1, sy = y0<y1?1:-1, err = dx-dy;
        while (x0 != x1 || y0 != y1) {
            if (x0 >= 0 && x0 < disp_size && y0 >= 0 && y0 < disp_size && grid[y0][x0] == ' ') grid[y0][x0] = '.';
            int e2 = 2*err;
            if (e2 > -dy) { err -= dy; x0 += sx; }
            if (e2 < dx) { err += dx; y0 += sy; }
        }
    }
    for (int y = 0; y < disp_size; y++) printf("%s\n", grid[y]);
}

void test_cube_transform() {
    printf("\n=== Testing Cube Transformation ===\n");
    vec3_t cube[] = {
        {-1,-1,-1}, {1,-1,-1}, {1,1,-1}, {-1,1,-1},
        {-1,-1,1}, {1,-1,1}, {1,1,1}, {-1,1,1}
    };
    mat4_t S = mat4_scale(0.8f, 0.8f, 0.8f);
    mat4_t R = mat4_rotate_xyz(0.3f, 0.5f, 0.2f);
    mat4_t T = mat4_translate(0.0f, 0.0f, -3.0f);
    mat4_t mvp = mat4_mul(mat4_mul(T, R), S);
    float x2d[8], y2d[8];
    printf("Transformed Cube Vertices (3D and projected 2D):\n");
    for (int i = 0; i < 8; i++) {
        vec3_t v = mat4_mul_vec3(mvp, cube[i]);
        float d = 5.0f;
        float scale = d / (d + v.z);
        x2d[i] = v.x * scale * 10; // scale for display
        y2d[i] = v.y * scale * 10;
        printf("v%d: (%6.2f, %6.2f, %6.2f) -> 2D: (%6.2f, %6.2f)\n", i, v.x, v.y, v.z, x2d[i], y2d[i]);
    }
    printf("\nASCII wireframe (O=vertex, .=edge):\n");
    draw_ascii_wireframe(x2d, y2d, 40);
}

int main() {
    test_vector_operations();
    test_matrix_operations();
    test_cube_transform();
    return 0;
}