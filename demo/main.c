#include <stdio.h>
#include <math.h>
#include "../include/tiny3d.h"

#define WIDTH 500
#define HEIGHT 500
#define FRAMES 60

// Soccer ball-like truncated icosahedron (simplified)
vec3_t vertices[] = {
    {  0.000f,  0.000f,  1.000f },
    {  0.894f,  0.000f,  0.447f },
    {  0.276f,  0.851f,  0.447f },
    { -0.724f,  0.526f,  0.447f },
    { -0.724f, -0.526f,  0.447f },
    {  0.276f, -0.851f,  0.447f },
    {  0.724f,  0.526f, -0.447f },
    { -0.276f,  0.851f, -0.447f },
    { -0.894f,  0.000f, -0.447f },
    { -0.276f, -0.851f, -0.447f },
    {  0.724f, -0.526f, -0.447f },
    {  0.000f,  0.000f, -1.000f }
};

int edges[][2] = {
    {0, 1}, {0, 2}, {0, 3}, {0, 4}, {0, 5},
    {1, 2}, {2, 3}, {3, 4}, {4, 5}, {5, 1},
    {1, 6}, {2, 7}, {3, 8}, {4, 9}, {5, 10},
    {6, 7}, {7, 8}, {8, 9}, {9, 10}, {10, 6},
    {6, 11}, {7, 11}, {8, 11}, {9, 11}, {10, 11}
};

int main() {
    vec3_t light_dir = {1.0f, 1.0f, -1.0f};
    light_dir = vec3_normalize_fast(light_dir);

    vec3_t start_dir = {0.0f, 0.0f, 1.0f};
    vec3_t end_dir = {1.0f, 0.0f, 0.0f};

    for (int frame = 0; frame < FRAMES; ++frame) {
        canvas_t* canvas = create_canvas(WIDTH, HEIGHT);

        float t = (float)frame / FRAMES;
        vec3_t current_dir = vec3_slerp(start_dir, end_dir, t);

        mat4_t model = mat4_rotate_xyz(t * 2 * M_PI, t * 2 * M_PI, 0.0f);
        mat4_t view = mat4_translate(0, 0, -5);
        mat4_t proj = mat4_frustum_asymmetric(-1, 1, -1, 1, 1, 10);

        for (int i = 0; i < sizeof(edges) / sizeof(edges[0]); ++i) {
            vec3_t v1 = project_vertex(vertices[edges[i][0]], model, view, proj, WIDTH, HEIGHT);
            vec3_t v2 = project_vertex(vertices[edges[i][1]], model, view, proj, WIDTH, HEIGHT);

            vec3_t edge_dir = vec3_sub(v2, v1);
            float intensity = compute_lambert_intensity(edge_dir, light_dir);

            if (clip_to_circular_viewport(canvas, v1.x, v1.y) &&
                clip_to_circular_viewport(canvas, v2.x, v2.y)) {
                draw_line_f(canvas, v1.x, v1.y, v2.x, v2.y, 1.0f * intensity);
            }
        }

        char filename[64];
        sprintf(filename, "demo/frame_%03d.pgm", frame);
        save_canvas_to_pgm(canvas, filename);
        destroy_canvas(canvas);
    }

    printf("Demo complete. Frames saved to demo/frame_###.pgm\n");
    return 0;
}
