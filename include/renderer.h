#ifndef RENDERER_H
#define RENDERER_H

#include "tiny3d.h"
#include "math3d.h"

/* Vertex projection */
void project_vertex(mat4_t mvp, vec3_t vertex, float* screen_x, float* screen_y);

/* Viewport clipping */
int clip_to_circular_viewport(canvas_t* canvas, float x, float y);

/* Wireframe rendering */
void render_wireframe(
    canvas_t* canvas,
    mat4_t mvp,
    vec3_t* vertices,
    int vertex_count,
    int* edges,
    int edge_count,
    float thickness
);

/* Depth buffer (optional for bonus) */
typedef struct {
    float* buffer;
    int width;
    int height;
} z_buffer_t;

void init_z_buffer(z_buffer_t* zbuf, int width, int height);
void free_z_buffer(z_buffer_t* zbuf);
int z_buffer_test(z_buffer_t* zbuf, int x, int y, float depth);

#endif // RENDERER_H