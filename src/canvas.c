#include "canvas.h"
#include <stdlib.h>
#include <math.h>

canvas_t* create_canvas(int width, int height) {
    canvas_t* canvas = (canvas_t*)malloc(sizeof(canvas_t));
    canvas->width = width;
    canvas->height = height;
    
    // Allocate pixel rows
    canvas->pixels = (float**)malloc(height * sizeof(float*));
    
    // Allocate each row
    for (int y = 0; y < height; y++) {
        canvas->pixels[y] = (float*)calloc(width, sizeof(float));
    }
    
    return canvas;
}

void free_canvas(canvas_t* canvas) {
    if (!canvas) return;
    
    // Free each row
    for (int y = 0; y < canvas->height; y++) {
        free(canvas->pixels[y]);
    }
    
    // Free row pointers
    free(canvas->pixels);
    free(canvas);
}

void clear_canvas(canvas_t* canvas, float brightness) {
    for (int y = 0; y < canvas->height; y++) {
        for (int x = 0; x < canvas->width; x++) {
            canvas->pixels[y][x] = brightness;
        }
    }
}

void set_pixel_f(canvas_t* canvas, float x, float y, float intensity) {
    if (!canvas) return;
    
    // Bilinear filtering implementation
    int x0 = (int)floor(x);
    int y0 = (int)floor(y);
    float dx = x - x0;
    float dy = y - y0;
    
    // Distribute intensity to 4 neighboring pixels
    for (int i = 0; i <= 1; i++) {
        for (int j = 0; j <= 1; j++) {
            int px = x0 + i;
            int py = y0 + j;
            
            if (px >= 0 && px < canvas->width && py >= 0 && py < canvas->height) {
                float weight = (i ? dx : 1-dx) * (j ? dy : 1-dy);
                canvas->pixels[py][px] += intensity * weight;
                
                // Clamp to [0,1] range
                if (canvas->pixels[py][px] > 1.0f) canvas->pixels[py][px] = 1.0f;
            }
        }
    }
}

void draw_line_f(canvas_t* canvas, float x0, float y0, float x1, float y1, float thickness) {
    // DDA line algorithm implementation
    float dx = x1 - x0;
    float dy = y1 - y0;
    float steps = fmax(fabs(dx), fabs(dy));
    
    float xInc = dx / steps;
    float yInc = dy / steps;
    
    float x = x0;
    float y = y0;
    
    for (int i = 0; i <= steps; i++) {
        // Draw with thickness by drawing multiple pixels around the line
        for (float t = -thickness/2; t <= thickness/2; t += 0.5f) {
            for (float s = -thickness/2; s <= thickness/2; s += 0.5f) {
                float dist = sqrt(t*t + s*s);
                if (dist <= thickness/2) {
                    set_pixel_f(canvas, x + t, y + s, 1.0f);
                }
            }
        }
        
        x += xInc;
        y += yInc;
    }
}