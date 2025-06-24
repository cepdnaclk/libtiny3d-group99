#include <stdlib.h>
#include <math.h>
#include "../include/renderer.h"

// === PROJECT 3D VERTEX TO SCREEN SPACE ===

vec3_t project_vertex(
    vec3_t model_point,
    mat4_t model_matrix,
    mat4_t view_matrix,
    mat4_t proj_matrix,
    int screen_width,
    int screen_height
) {
    // Step 1: Transform through Model → View → Projection
    mat4_t mvp = mat4_mul(proj_matrix, mat4_mul(view_matrix, model_matrix));
    vec3_t projected = mat4_transform_point(mvp, model_point);

    // Step 2: Map to screen coordinates
    vec3_t screen;
    screen.x = (projected.x * 0.5f + 0.5f) * screen_width;
    screen.y = (1.0f - (projected.y * 0.5f + 0.5f)) * screen_height; // Flip Y
    screen.z = projected.z; // Keep Z for depth
    return screen;
}

// === CLIP POINT TO CIRCULAR VIEWPORT ===

int clip_to_circular_viewport(canvas_t* canvas, float x, float y) {
    float cx = canvas->width / 2.0f;
    float cy = canvas->height / 2.0f;
    float radius = fminf(cx, cy);
    float dx = x - cx;
    float dy = y - cy;
    return (dx * dx + dy * dy) <= (radius * radius);
}

// === RENDER WIREFRAME MODEL ===

typedef struct {
    float depth;
    int v1, v2;
} depth_sorted_edge;

static int compare_edges(const void* a, const void* b) {
    float z1 = ((depth_sorted_edge*)a)->depth;
    float z2 = ((depth_sorted_edge*)b)->depth;
    return (z1 < z2) - (z1 > z2);  // Sort back-to-front (descending)
}

void render_wireframe(
    canvas_t* canvas,
    vec3_t* vertices,
    int (*edges)[2],
    int vertex_count,
    int edge_count,
    mat4_t model_matrix,
    mat4_t view_matrix,
    mat4_t proj_matrix
) {
    vec3_t* projected = malloc(vertex_count * sizeof(vec3_t));
    for (int i = 0; i < vertex_count; ++i) {
        projected[i] = project_vertex(vertices[i], model_matrix, view_matrix, proj_matrix,
                                      canvas->width, canvas->height);
    }

    // Depth sort edges by average Z
    depth_sorted_edge* sorted = malloc(edge_count * sizeof(depth_sorted_edge));
    for (int i = 0; i < edge_count; ++i) {
        int v1 = edges[i][0];
        int v2 = edges[i][1];
        float z_avg = (projected[v1].z + projected[v2].z) / 2.0f;
        sorted[i] = (depth_sorted_edge){ .depth = z_avg, .v1 = v1, .v2 = v2 };
    }

    qsort(sorted, edge_count, sizeof(depth_sorted_edge), compare_edges);

    // Draw edges from back to front
    for (int i = 0; i < edge_count; ++i) {
        vec3_t p1 = projected[sorted[i].v1];
        vec3_t p2 = projected[sorted[i].v2];

        // Optionally clip: both endpoints must be in viewport
        if (clip_to_circular_viewport(canvas, p1.x, p1.y) &&
            clip_to_circular_viewport(canvas, p2.x, p2.y)) {
            draw_line_f(canvas, p1.x, p1.y, p2.x, p2.y, 1.0f);
        }
    }

    free(projected);
    free(sorted);
}
