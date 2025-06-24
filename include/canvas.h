#ifndef CANVAS_H
#define CANVAS_H

// Structure to represent a floating-point canvas for grayscale rendering
typedef struct {
    int width;       // Width of the canvas in pixels
    int height;      // Height of the canvas in pixels
    float** pixels;  // 2D array of float values [0.0, 1.0] representing pixel brightness
} canvas_t;

/**
 * Creates a canvas of given width and height.
 * Allocates memory for the pixel buffer and initializes all pixels to 0.
 */
canvas_t* create_canvas(int width, int height);

/**
 * Frees all memory associated with the canvas.
 */
void destroy_canvas(canvas_t* canvas);

/**
 * Sets the pixel intensity at floating-point position (x, y) using bilinear filtering.
 * Intensity is spread across 4 nearby pixels based on the fractional position.
 *
 * @param canvas The canvas to modify
 * @param x X-coordinate (float)
 * @param y Y-coordinate (float)
 * @param intensity Brightness in range [0.0, 1.0]
 */
void set_pixel_f(canvas_t* canvas, float x, float y, float intensity);

/**
 * Draws a smooth line from (x0, y0) to (x1, y1) using the DDA algorithm.
 * The line supports variable thickness and smooth anti-aliased output.
 *
 * @param canvas The canvas to draw on
 * @param x0 Starting X coordinate (float)
 * @param y0 Starting Y coordinate (float)
 * @param x1 Ending X coordinate (float)
 * @param y1 Ending Y coordinate (float)
 * @param thickness Thickness of the line (float)
 */
void draw_line_f(canvas_t* canvas, float x0, float y0, float x1, float y1, float thickness);

/**
 * (Optional utility) Saves the canvas to a grayscale PGM image file for testing.
 *
 * @param canvas The canvas to export
 * @param filename Output filename (.pgm)
 */
void save_canvas_to_pgm(canvas_t* canvas, const char* filename);

#endif // CANVAS_H
