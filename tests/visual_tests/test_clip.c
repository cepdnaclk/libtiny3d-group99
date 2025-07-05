#include <stdio.h>
#include <math.h>
#include "../../include/tiny3d.h"

#define WIDTH 500
#define HEIGHT 500

int main() {
    canvas_t* canvas = create_canvas(WIDTH, HEIGHT);

    for (int y = 0; y < HEIGHT; y += 5) {
        for (int x = 0; x < WIDTH; x += 5) {
            if (clip_to_circular_viewport(canvas, x, y)) {
                set_pixel_f(canvas, x, y, 1.0f);
            }
        }
    }

    save_canvas_to_pgm(canvas, "visual_tests/test_clip_output.pgm");
    destroy_canvas(canvas);
    printf("test_clip complete. Output saved to visual_tests/test_clip_output.pgm\n");
    return 0;
}
