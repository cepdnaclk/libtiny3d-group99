#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "../include/canvas.h"

canvas_t* create_canvas(int width, int height) {
    canvas_t* canvas = malloc(sizeof(canvas_t));
    if (!canvas) return NULL;

    canvas->width = width;
    canvas->height = height;

    canvas->pixels = malloc(height * sizeof(float*));
    for (int y = 0; y < height; ++y) {
        canvas->pixels[y] = calloc(width, sizeof(float));
    }

    return canvas;
}

void destroy_canvas(canvas_t* canvas) {
    if (!canvas) return;

    for (int y = 0; y < canvas->height; ++y) {
        free(canvas->pixels[y]);
    }
    free(canvas->pixels);
    free(canvas);
}

void set_pixel_f(canvas_t* canvas, float x, float y, float intensity) {
    if (!canvas) return;

    if (intensity < 0.0f) intensity = 0.0f;
    if (intensity > 1.0f) intensity = 1.0f;

    int x0 = (int)floorf(x);
    int y0 = (int)floorf(y);
    float dx = x - x0;
    float dy = y - y0;

    for (int i = 0; i <= 1; ++i) {
        for (int j = 0; j <= 1; ++j) {
            int xi = x0 + i;
            int yj = y0 + j;

            if (xi >= 0 && xi < canvas->width && yj >= 0 && yj < canvas->height) {
                float weight = (1 - fabsf(i - dx)) * (1 - fabsf(j - dy));
                canvas->pixels[yj][xi] += intensity * weight;
                if (canvas->pixels[yj][xi] > 1.0f)
                    canvas->pixels[yj][xi] = 1.0f;
            }
        }
    }
}

void draw_line_f(canvas_t* canvas, float x0, float y0, float x1, float y1, float thickness) {
    if (!canvas) return;

    float dx = x1 - x0;
    float dy = y1 - y0;
    float length = sqrtf(dx * dx + dy * dy);
    if (length == 0.0f) return;

    float steps = length * 2.0f;  // More points for smoother lines
    float step_x = dx / steps;
    float step_y = dy / steps;

    for (int i = 0; i <= steps; ++i) {
        float cx = x0 + step_x * i;
        float cy = y0 + step_y * i;

        // Apply thickness by drawing a square around the center point
        for (float tx = -thickness / 2; tx <= thickness / 2; tx += 0.5f) {
            for (float ty = -thickness / 2; ty <= thickness / 2; ty += 0.5f) {
                set_pixel_f(canvas, cx + tx, cy + ty, 1.0f);
            }
        }
    }
}

// Optional: Save canvas to file for testing (PGM format)
void save_canvas_to_pgm(canvas_t* canvas, const char* filename) {
    if (!canvas || !filename) return;

    FILE* f = fopen(filename, "w");
    if (!f) return;

    fprintf(f, "P2\n%d %d\n255\n", canvas->width, canvas->height);
    for (int y = 0; y < canvas->height; ++y) {
        for (int x = 0; x < canvas->width; ++x) {
            int value = (int)(canvas->pixels[y][x] * 255.0f);
            fprintf(f, "%d ", value);
        }
        fprintf(f, "\n");
    }
    fclose(f);
}
