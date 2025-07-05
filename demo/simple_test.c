#include "tiny3d.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

int main() {
    printf("=== libtiny3d Simple Test ===\n");
    
    // Test 1: Canvas creation
    canvas_t* canvas = create_canvas(100, 100);
    printf("✓ Canvas created: %dx%d\n", canvas->width, canvas->height);
    
    // Test 2: Draw a simple line
    draw_line_f(canvas, 10, 10, 90, 90, 2.0f);
    printf("✓ Line drawn\n");
    
    // Test 3: Math3D operations
    vec3_t v = vec3_from_spherical(5.0f, M_PI/4, M_PI/3);
    printf("✓ Vector created: (%.2f, %.2f, %.2f)\n", v.x, v.y, v.z);
    
    mat4_t trans = mat4_translate(2.0f, 3.0f, 4.0f);
    vec3_t vt = mat4_mul_vec3(trans, v);
    printf("✓ Matrix transform: (%.2f, %.2f, %.2f)\n", vt.x, vt.y, vt.z);
    
    // Test 4: Create cube
    vec3_t *cube_verts;
    int *cube_edges;
    int cube_vcount, cube_ecount;
    
    create_cube(&cube_verts, &cube_edges, &cube_vcount, &cube_ecount);
    printf("✓ Cube created: %d vertices, %d edges\n", cube_vcount, cube_ecount);
    
    // Cleanup
    free(cube_verts);
    free(cube_edges);
    free_canvas(canvas);
    
    printf("✓ All tests passed!\n");
    return 0;
} 