#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include "../../include/tiny3d.h"

#define WIDTH 500
#define HEIGHT 500
#define NUM_LINES 50

int main() {
srand(time(NULL));
canvas_t* canvas = create_canvas(WIDTH, HEIGHT);  
float centerX = WIDTH / 2.0f;  
float centerY = HEIGHT / 2.0f;  

for (int i = 0; i < NUM_LINES; ++i) {  
    float angle = ((float)rand() / RAND_MAX) * 2.0f * M_PI;  
    float length = 100 + ((float)rand() / RAND_MAX) * 150;  
    float thickness = 0.5f + ((float)rand() / RAND_MAX) * 3.0f;  

    float x1 = centerX + length * cosf(angle);  
    float y1 = centerY + length * sinf(angle);  

    draw_line_f(canvas, centerX, centerY, x1, y1, thickness);  
}  

save_canvas_to_pgm(canvas, "visual_tests/test_lines_output.pgm");  
destroy_canvas(canvas);  

printf("test_lines complete. Output saved to visual_tests/test_lines_output.pgm\n");  
return 0;  
