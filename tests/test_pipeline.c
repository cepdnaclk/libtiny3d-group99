#include <stdio.h>
#include <math.h>
#include "../include/tiny3d.h"

#define WIDTH 500
#define HEIGHT 500

// Simple wireframe cube
vec3_t cube_vertices[] = {
{-1, -1, -1},
{ 1, -1, -1},
{ 1, 1, -1},
{-1, 1, -1},
{-1, -1, 1},
{ 1, -1, 1},
{ 1, 1, 1},
{-1, 1, 1}
};

int cube_edges[][2] = {
{0, 1}, {1, 2}, {2, 3}, {3, 0},
{4, 5}, {5, 6}, {6, 7}, {7, 4},
{0, 4}, {1, 5}, {2, 6}, {3, 7}
};

int main() {
canvas_t* canvas = create_canvas(WIDTH, HEIGHT);
// Transformation setup
mat4_t model = mat4_identity();
mat4_t view = mat4_translate(0, 0, -5); // Camera back
mat4_t proj = mat4_frustum_asymmetric(-1, 1, -1, 1, 1, 10);

// Rotate the model to test transformation
model = mat4_mul(mat4_rotate_xyz(0.5f, 0.5f, 0.0f), model);
model = mat4_mul(mat4_scale(1.0f, 1.0f, 1.0f), model);

// Render the cube
render_wireframe(canvas, cube_vertices, cube_edges, 8, 12, model, view, proj);

// Save output to PGM file
save_canvas_to_pgm(canvas, "test_pipeline_output.pgm");

destroy_canvas(canvas);

printf("Pipeline test complete. Output saved to test_pipeline_output.pgm\n");
return 0;
