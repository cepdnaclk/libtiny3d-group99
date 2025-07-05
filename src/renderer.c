#include "renderer.h"
#include <math.h>
#include <stdlib.h>

/* Project a 3D vertex through the transformation pipeline */
void project_vertex(mat4_t mvp, vec3_t vertex, float* screen_x, float* screen_y) {
    // Transform vertex through MVP matrix
    vec3_t transformed = mat4_mul_vec3(mvp, vertex);
    
    // Perspective divide and viewport transform
    *screen_x = (transformed.x + 1.0f) * 0.5f;
    *screen_y = (1.0f - transformed.y) * 0.5f;
}

/* Check if point is within circular viewport */
int clip_to_circular_viewport(canvas_t* canvas, float x, float y) {
    // Convert to normalized device coordinates
    float nx = 2.0f * x - 1.0f;
    float ny = 2.0f * y - 1.0f;
    
    // Check if point is within unit circle
    return (nx*nx + ny*ny) <= 1.0f;
}

/* Render wireframe model */
void render_wireframe(
    canvas_t* canvas,
    mat4_t mvp,
    vec3_t* vertices,
    int vertex_count,
    int* edges,
    int edge_count,
    float thickness
) {
    // Project all vertices first
    float* screen_x = (float*)malloc(vertex_count * sizeof(float));
    float* screen_y = (float*)malloc(vertex_count * sizeof(float));
    
    for (int i = 0; i < vertex_count; i++) {
        project_vertex(mvp, vertices[i], &screen_x[i], &screen_y[i]);
    }
    
    // Draw each edge
    for (int i = 0; i < edge_count; i++) {
        int idx0 = edges[i*2];
        int idx1 = edges[i*2+1];
        
        if (idx0 >= 0 && idx0 < vertex_count && idx1 >= 0 && idx1 < vertex_count) {
            float x0 = screen_x[idx0] * canvas->width;
            float y0 = screen_y[idx0] * canvas->height;
            float x1 = screen_x[idx1] * canvas->width;
            float y1 = screen_y[idx1] * canvas->height;
            
            // Only draw if both endpoints are in circular viewport
            if (clip_to_circular_viewport(canvas, screen_x[idx0], screen_y[idx0]) ||
                clip_to_circular_viewport(canvas, screen_x[idx1], screen_y[idx1])) {
                draw_line_f(canvas, x0, y0, x1, y1, thickness);
            }
        }
    }
    
    free(screen_x);
    free(screen_y);
}

/* Depth buffer implementation (optional) */
void init_z_buffer(z_buffer_t* zbuf, int width, int height) {
    zbuf->width = width;
    zbuf->height = height;
    zbuf->buffer = (float*)malloc(width * height * sizeof(float));
    
    for (int i = 0; i < width*height; i++) {
        zbuf->buffer[i] = 1.0f; // Initialize to far plane
    }
}

void free_z_buffer(z_buffer_t* zbuf) {
    if (zbuf && zbuf->buffer) {
        free(zbuf->buffer);
        zbuf->buffer = NULL;
    }
}

int z_buffer_test(z_buffer_t* zbuf, int x, int y, float depth) {
    if (x < 0 || x >= zbuf->width || y < 0 || y >= zbuf->height) {
        return 0;
    }
    
    int idx = y * zbuf->width + x;
    if (depth < zbuf->buffer[idx]) {
        zbuf->buffer[idx] = depth;
        return 1;
    }
    return 0;
}