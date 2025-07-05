#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "canvas.h"
#include "math3d.h"
#include "renderer.h"
#include "lighting.h"
#include "animation.h"

#define WIDTH 800
#define HEIGHT 600

void demo_clock_face(canvas_t* canvas) {
    int center_x = canvas->width / 2;
    int center_y = canvas->height / 2;
    float radius = fminf(canvas->width, canvas->height) * 0.4f;
    
    for (int angle = 0; angle < 360; angle += 15) {
        float rad = angle * M_PI / 180.0f;
        float end_x = center_x + radius * cosf(rad);
        float end_y = center_y + radius * sinf(rad);
        
        draw_line_f(canvas, center_x, center_y, end_x, end_y, 1.5f);
    }
}

void demo_3d_cube() {
    canvas_t* canvas = create_canvas(WIDTH, HEIGHT);
    
    // Create a cube mesh
    mesh_t cube = generate_cube();
    
    // Set up view-projection matrix
    mat4_t view = mat4_translate(0, 0, -5);
    mat4_t proj = mat4_frustum_asymmetric(-1, 1, -1, 1, 1, 100);
    mat4_t view_proj = mat4_multiply(proj, view);
    
    // Animation variables
    float angle = 0;
    const float angle_step = 0.01f;
    
    // Simple animation loop (in a real application, this would be in a proper loop)
    for (int frame = 0; frame < 100; frame++) {
        // Clear canvas
        for (int y = 0; y < canvas->height; y++) {
            for (int x = 0; x < canvas->width; x++) {
                canvas->pixels[y][x] = 0.0f;
            }
        }
        
        // Update rotation
        angle += angle_step;
        mat4_t model = mat4_rotate_xyz(angle, angle * 0.5f, angle * 0.3f);
        mat4_t mvp = mat4_multiply(view_proj, model);
        
        // Render wireframe
        render_wireframe(canvas, mvp, &cube);
        
        // In a real application, you would display the canvas here
        // For this demo, we'll just simulate it by saving one frame
        if (frame == 50) {
            // Save canvas to PPM file (for demonstration)
            FILE* fp = fopen("cube_frame.ppm", "wb");
            if (fp) {
                fprintf(fp, "P6\n%d %d\n255\n", canvas->width, canvas->height);
                for (int y = 0; y < canvas->height; y++) {
                    for (int x = 0; x < canvas->width; x++) {
                        unsigned char val = (unsigned char)(fminf(canvas->pixels[y][x], 1.0f) * 255);
                        fwrite(&val, 1, 1, fp); // R
                        fwrite(&val, 1, 1, fp); // G
                        fwrite(&val, 1, 1, fp); // B
                    }
                }
                fclose(fp);
            }
        }
    }
    
    // Clean up
    free(cube.vertices);
    free(cube.edges);
    destroy_canvas(canvas);
}

void demo_soccer_ball() {
    canvas_t* canvas = create_canvas(WIDTH, HEIGHT);
    
    // Create soccer ball mesh
    mesh_t ball = generate_soccer_ball();
    
    // Set up view-projection matrix
    mat4_t view = mat4_translate(0, 0, -8);
    mat4_t proj = mat4_frustum_asymmetric(-1, 1, -1, 1, 1, 100);
    mat4_t view_proj = mat4_multiply(proj, view);
    
    // Animation variables
    float angle = 0;
    const float angle_step = 0.005f;
    
    // Simple animation loop
    for (int frame = 0; frame < 200; frame++) {
        // Clear canvas
        for (int y = 0; y < canvas->height; y++) {
            for (int x = 0; x < canvas->width; x++) {
                canvas->pixels[y][x] = 0.0f;
            }
        }
        
        // Update rotation
        angle += angle_step;
        mat4_t model = mat4_rotate_xyz(angle, angle * 0.7f, angle * 0.3f);
        mat4_t mvp = mat4_multiply(view_proj, model);
        
        // Render wireframe
        render_wireframe(canvas, mvp, &ball);
        
        // Save one frame for demonstration
        if (frame == 100) {
            FILE* fp = fopen("ball_frame.ppm", "wb");
            if (fp) {
                fprintf(fp, "P6\n%d %d\n255\n", canvas->width, canvas->height);
                for (int y = 0; y < canvas->height; y++) {
                    for (int x = 0; x < canvas->width; x++) {
                        unsigned char val = (unsigned char)(fminf(canvas->pixels[y][x], 1.0f) * 255);
                        fwrite(&val, 1, 1, fp);
                        fwrite(&val, 1, 1, fp);
                        fwrite(&val, 1, 1, fp);
                    }
                }
                fclose(fp);
            }
        }
    }
    
    // Clean up
    free(ball.vertices);
    free(ball.edges);
    destroy_canvas(canvas);
}

void demo_bezier_animation() {
    canvas_t* canvas = create_canvas(WIDTH, HEIGHT);
    
    // Create a cube mesh
    mesh_t cube = generate_cube();
    
    // Set up view-projection matrix
    mat4_t view = mat4_translate(0, 0, -5);
    mat4_t proj = mat4_frustum_asymmetric(-1, 1, -1, 1, 1, 100);
    mat4_t view_proj = mat4_multiply(proj, view);
    
    // Define Bézier control points
    vec3_t p0 = vec3_create(-2, -2, 0);
    vec3_t p1 = vec3_create(-2, 2, 2);
    vec3_t p2 = vec3_create(2, -2, -2);
    vec3_t p3 = vec3_create(2, 2, 0);
    
    // Animation loop
    for (int frame = 0; frame < 100; frame++) {
        float t = frame / 100.0f;
        
        // Clear canvas
        for (int y = 0; y < canvas->height; y++) {
            for (int x = 0; x < canvas->width; x++) {
                canvas->pixels[y][x] = 0.0f;
            }
        }
        
        // Calculate position along Bézier curve
        vec3_t position = vec3_bezier(p0, p1, p2, p3, t);
        
        // Create model matrix
        mat4_t model = mat4_translate(position.x, position.y, position.z);
        mat4_t rotation = mat4_rotate_xyz(t * M_PI, t * M_PI * 0.5f, t * M_PI * 0.3f);
        model = mat4_multiply(model, rotation);
        
        mat4_t mvp = mat4_multiply(view_proj, model);
        
        // Render wireframe
        render_wireframe(canvas, mvp, &cube);
        
        // Save one frame for demonstration
        if (frame == 50) {
            FILE* fp = fopen("bezier_frame.ppm", "wb");
            if (fp) {
                fprintf(fp, "P6\n%d %d\n255\n", canvas->width, canvas->height);
                for (int y = 0; y < canvas->height; y++) {
                    for (int x = 0; x < canvas->width; x++) {
                        unsigned char val = (unsigned char)(fminf(canvas->pixels[y][x], 1.0f) * 255);
                        fwrite(&val, 1, 1, fp);
                        fwrite(&val, 1, 1, fp);
                        fwrite(&val, 1, 1, fp);
                    }
                }
                fclose(fp);
            }
        }
    }
    
    // Clean up
    free(cube.vertices);
    free(cube.edges);
    destroy_canvas(canvas);
}

int main() {
    printf("Running libtiny3d demos...\n");
    
    // Task 1 demo: Clock face
    canvas_t* clock_canvas = create_canvas(WIDTH, HEIGHT);
    demo_clock_face(clock_canvas);
    
    // Save clock face to PPM file
    FILE* fp = fopen("clock.ppm", "wb");
    if (fp) {
        fprintf(fp, "P6\n%d %d\n255\n", clock_canvas->width, clock_canvas->height);
        for (int y = 0; y < clock_canvas->height; y++) {
            for (int x = 0; x < clock_canvas->width; x++) {
                unsigned char val = (unsigned char)(fminf(clock_canvas->pixels[y][x], 1.0f) * 255);
                fwrite(&val, 1, 1, fp);
                fwrite(&val, 1, 1, fp);
                fwrite(&val, 1, 1, fp);
            }
        }
        fclose(fp);
    }
    destroy_canvas(clock_canvas);
    
    // Task 2 & 3 demo: 3D cube
    demo_3d_cube();
    
    // Task 3 demo: Soccer ball
    demo_soccer_ball();
    
    // Task 4 demo: Bézier animation
    demo_bezier_animation();
    
    printf("Demos completed. Check output files.\n");
    return 0;
}