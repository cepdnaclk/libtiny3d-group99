#ifndef RENDERER_H
#define RENDERER_H

#include "canvas.h"
#include "math3d.h"

// === Vertex Projection ===

/**
 * Applies all necessary transformations to project a 3D point into 2D screen space.
 * This includes:
 *  - Model → World
 *  - World → Camera
 *  - Camera → Projection
 *  - Projection → Screen
 *
 * @param model_point The original 3D vertex
 * @param model_matrix Model transformation matrix
 * @param view_matrix Camera/view matrix
 * @param proj_matrix Projection matrix
 * @param screen_width Width of canvas
 * @param screen_height Height of canvas
 * @return 2D point on the screen (as vec3, where z is depth)
 */
vec3_t project_vertex(vec3_t model_point, mat4_t model_matrix, mat4_t view_matrix, mat4_t proj_matrix, int screen_width, int screen_height);


// === Circular Viewport Clipping ===

/**
 * Checks whether a 2D point lies inside a circular viewport centered in the canvas.
 * Used to clip pixels/lines outside the desired drawing region.
 *
 * @param canvas The target canvas
 * @param x X coordinate (float)
 * @param y Y coordinate (float)
 * @return 1 if the point is inside the circular viewport, 0 otherwise
 */
int clip_to_circular_viewport(canvas_t* canvas, float x, float y);


// === Wireframe Rendering ===

/**
 * Renders a wireframe model by projecting its vertices and drawing lines between connected edges.
 * Uses draw_line_f() from canvas and sorts lines by depth (z) from back to front.
 *
 * @param canvas The target canvas
 * @param vertices Array of 3D vertex positions
 * @param edges Array of index pairs (each pair represents an edge: {i1, i2})
 * @param vertex_count Number of vertices in the array
 * @param edge_count Number of edges in the array
 * @param model_matrix Model transformation matrix
 * @param view_matrix View/camera transformation matrix
 * @param proj_matrix Projection matrix
 */
void render_wireframe(
    canvas_t* canvas,
    vec3_t* vertices,
    int (*edges)[2],
    int vertex_count,
    int edge_count,
    mat4_t model_matrix,
    mat4_t view_matrix,
    mat4_t proj_matrix
);

#endif // RENDERER_H
