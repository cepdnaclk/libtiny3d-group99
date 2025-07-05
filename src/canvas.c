#include "canvas.h"
#include <stdlib.h>
#include <math.h>
#include <stdio.h>

canvas_t* create_canvas(int width, int height) {
    canvas_t* canvas = (canvas_t*)malloc(sizeof(canvas_t));
    canvas->width = width;
    canvas->height = height;
    
    // Allocate memory for the pixel array
    canvas->pixels = (float**)malloc(height * sizeof(float*));
    for (int i = 0; i < height; i++) {
        canvas->pixels[i] = (float*)calloc(width, sizeof(float));
    }
    
    return canvas;
}

void destroy_canvas(canvas_t* canvas) {
    if (canvas) {
        for (int i = 0; i < canvas->height; i++) {
            free(canvas->pixels[i]);
        }
        free(canvas->pixels);
        free(canvas);
    }
}

void set_pixel_f(canvas_t* canvas, float x, float y, float intensity) {
    if (!canvas || x < 0 || y < 0 || x >= canvas->width || y >= canvas->height) {
        return;
    }
    
    // Bilinear filtering implementation
    int x0 = (int)floor(x);
    int y0 = (int)floor(y);
    int x1 = x0 + 1;
    int y1 = y0 + 1;
    
    float x_frac = x - x0;
    float y_frac = y - y0;
    
    // Distribute intensity to surrounding pixels
    if (x0 >= 0 && x0 < canvas->width && y0 >= 0 && y0 < canvas->height) {
        canvas->pixels[y0][x0] += intensity * (1 - x_frac) * (1 - y_frac);
    }
    if (x1 >= 0 && x1 < canvas->width && y0 >= 0 && y0 < canvas->height) {
        canvas->pixels[y0][x1] += intensity * x_frac * (1 - y_frac);
    }
    if (x0 >= 0 && x0 < canvas->width && y1 >= 0 && y1 < canvas->height) {
        canvas->pixels[y1][x0] += intensity * (1 - x_frac) * y_frac;
    }
    if (x1 >= 0 && x1 < canvas->width && y1 >= 0 && y1 < canvas->height) {
        canvas->pixels[y1][x1] += intensity * x_frac * y_frac;
    }
}

void draw_line_f(canvas_t* canvas, float x0, float y0, float x1, float y1, float thickness) {
    // DDA algorithm implementation
    float dx = x1 - x0;
    float dy = y1 - y0;
    float steps = fmaxf(fabsf(dx), fabsf(dy));
    
    if (steps == 0) {
        // Single point
        set_pixel_f(canvas, x0, y0, 1.0f);
        return;
    }
    
    float xInc = dx / steps;
    float yInc = dy / steps;
    
    float x = x0;
    float y = y0;
    
    // Handle thickness by drawing multiple parallel lines
    if (thickness > 1.0f) {
        float perpX = -dy / steps;
        float perpY = dx / steps;
        float halfThickness = thickness / 2.0f;
        
        for (int i = 0; i <= steps; i++) {
            // Draw a perpendicular line segment at each point
            float startX = x - perpX * halfThickness;
            float startY = y - perpY * halfThickness;
            float endX = x + perpX * halfThickness;
            float endY = y + perpY * halfThickness;
            
            // Draw the perpendicular segment
            float segSteps = fmaxf(fabsf(endX - startX), fabsf(endY - startY));
            if (segSteps > 0) {
                float segX = startX;
                float segY = startY;
                float segXInc = (endX - startX) / segSteps;
                float segYInc = (endY - startY) / segSteps;
                
                for (int j = 0; j <= segSteps; j++) {
                    set_pixel_f(canvas, segX, segY, 1.0f);
                    segX += segXInc;
                    segY += segYInc;
                }
            }
            
            x += xInc;
            y += yInc;
        }
    } else {
        // Thin line
        for (int i = 0; i <= steps; i++) {
            set_pixel_f(canvas, x, y, 1.0f);
            x += xInc;
            y += yInc;
        }
    }
}