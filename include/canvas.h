#ifndef CANVAS_H
#define CANVAS_H

#include <stdint.h>

/* Canvas structure */
typedef struct {
    int width;
    int height;
    float** pixels;  // 2D array of brightness values (0.0 to 1.0)
} canvas_t;

/* Canvas creation/destruction */
canvas_t* create_canvas(int width, int height);
void free_canvas(canvas_t* canvas);

/* Pixel operations */
void set_pixel_f(canvas_t* canvas, float x, float y, float intensity);

/* Drawing operations */
void draw_line_f(canvas_t* canvas, float x0, float y0, float x1, float y1, float thickness);

/* Helper functions */
void clear_canvas(canvas_t* canvas, float brightness);

#endif // CANVAS_H