#include <stdio.h>
#include <math.h>
#include "../../include/tiny3d.h"

#define WIDTH 500
#define HEIGHT 500

int main() {
    canvas_t* canvas = create_canvas(WIDTH, HEIGHT);
    vec3_t light_dir = {1.0f, 1.0f, -1.0f};
    light_dir = vec3_normalize_fast(light_dir);

    float centerX = WIDTH / 2.0f;
    float centerY = HEIGHT / 2.0f;
    float length = 200.0f;

    for (int angle = 0; angle < 360; angle += 15) {
        float rad = angle * M_PI / 180.0f;
        float x1 = centerX + length * cosf(rad);
        float y1 = centerY + length * sinf(rad);

        vec3_t edge_dir = { x1 - centerX, y1 - centerY, 0.0f };
        float intensity = compute_lambert_intensity(edge_dir, light_dir);

        draw_line_f(canvas, centerX, centerY, x1, y1, 1.0f * intensity);
    }

    save_canvas_to_pgm(canvas, "visual_tests/test_lighting_output.pgm");
    destroy_canvas(canvas);
    printf("test_lighting complete. Output saved to visual_tests/test_lighting_output.pgm\n");
    return 0;
}
