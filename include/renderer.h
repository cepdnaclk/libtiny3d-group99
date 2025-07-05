#ifndef RENDERER_H
#define RENDERER_H

#include "canvas.h"
#include "math3d.h"

typedef struct {
    vec3_t position;
    vec3_t rotation;
    vec3_t scale;
    mat4_t transform;
} object3d_t;

typedef struct {
    vec3_t* vertices;
    int* edges; // Pairs of vertex indices
    int vertex_count;
    int edge_count;
} mesh_t;

// Projection functions
vec3_t project_vertex(mat4_t view_proj, vec3_t vertex, int screen_width, int screen_height);
bool clip_to_circular_viewport(canvas_t* canvas, float x, float y, float radius);

// Rendering functions
void render_wireframe(canvas_t* canvas, mat4_t view_proj, mesh_t* mesh);

// Mesh generation
mesh_t generate_cube();
mesh_t generate_soccer_ball();

#endif // RENDERER_H