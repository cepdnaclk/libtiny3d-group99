#include "tiny3d.h"
#include "canvas.h"
#include "math3d.h"
#include "renderer.h"
#include <stdio.h>
#include <math.h>

#define PI 3.14159265358979323846f
#define WIDTH 800
#define HEIGHT 600

/* Create a cube model */
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

/* Create a pyramid model */
void create_pyramid(vec3_t** vertices, int** edges, int* vertex_count, int* edge_count) {
    *vertex_count = 5;
    *edge_count = 8;
    
    *vertices = (vec3_t*)malloc(*vertex_count * sizeof(vec3_t));
    *edges = (int*)malloc(*edge_count * 2 * sizeof(int));
    
    // Pyramid vertices
    vec3_t pyramid[] = {
        {-1,-1,-1}, {1,-1,-1}, {1,1,-1}, {-1,1,-1},  // Base
        {0,0,1}                                      // Apex
    };
    memcpy(*vertices, pyramid, sizeof(pyramid));
    
    // Pyramid edges
    int pyramid_edges[] = {
        0,1, 1,2, 2,3, 3,0,  // Base
        0,4, 1,4, 2,4, 3,4    // Sides
    };
    memcpy(*edges, pyramid_edges, sizeof(pyramid_edges));
}

void test_wireframe_rendering() {
    printf("=== Testing 3D Rendering Pipeline ===\n");
    
    // Create canvas
    canvas_t* canvas = create_canvas(WIDTH, HEIGHT);
    clear_canvas(canvas, 0.0f);
    
    // Create models
    vec3_t *cube_verts, *pyramid_verts;
    int *cube_edges, *pyramid_edges;
    int cube_vcount, cube_ecount, pyramid_vcount, pyramid_ecount;
    
    create_cube(&cube_verts, &cube_edges, &cube_vcount, &cube_ecount);
    create_pyramid(&pyramid_verts, &pyramid_edges, &pyramid_vcount, &pyramid_ecount);
    
    // Set up transforms
    mat4_t model = mat4_rotate_xyz(0.5f, 0.8f, 0.3f);
    mat4_t view = mat4_translate(0.0f, 0.0f, -5.0f);
    mat4_t proj = mat4_frustum_asymmetric(-1.0f, 1.0f, -1.0f, 1.0f, 1.0f, 100.0f);
    mat4_t mvp = mat4_mul(mat4_mul(proj, view), model);
    
    // Render cube
    render_wireframe(canvas, mvp, cube_verts, cube_vcount, cube_edges, cube_ecount, 1.5f);
    
    // Move pyramid and render
    mat4_t pyramid_model = mat4_translate(2.0f, 0.0f, 0.0f);
    mat4_t pyramid_mvp = mat4_mul(mat4_mul(proj, view), pyramid_model);
    render_wireframe(canvas, pyramid_mvp, pyramid_verts, pyramid_vcount, pyramid_edges, pyramid_ecount, 1.5f);
    
    printf("Rendered cube and pyramid to canvas\n");
    printf("Canvas dimensions: %dx%d\n", canvas->width, canvas->height);
    
    // Cleanup
    free(cube_verts);
    free(cube_edges);
    free(pyramid_verts);
    free(pyramid_edges);
    free_canvas(canvas);
}

void test_circular_clipping() {
    printf("\n=== Testing Circular Viewport Clipping ===\n");
    
    canvas_t* canvas = create_canvas(400, 400);
    clear_canvas(canvas, 0.0f);
    
    // Create a test pattern that extends beyond circular viewport
    for (float angle = 0; angle < 2*PI; angle += PI/12) {
        float x = 200 + cosf(angle) * 250;
        float y = 200 + sinf(angle) * 250;
        draw_line_f(canvas, 200, 200, x, y, 1.0f);
    }
    
    printf("Created test pattern with clipping (should see only lines within circle)\n");
    free_canvas(canvas);
}

int main() {
    test_wireframe_rendering();
    test_circular_clipping();
    return 0;
}