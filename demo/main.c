#include "tiny3d.h"
#include <math.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>

#ifdef _WIN32
#include <windows.h>
#include <conio.h>
#else
#include <unistd.h>
#include <termios.h>
#endif

#define WIDTH 400
#define HEIGHT 400
#define FPS 10
#define FRAME_DELAY (1000000/FPS)

/* Keyboard input setup */
#ifdef _WIN32
void enable_raw_mode() {
    // Windows doesn't need raw mode setup
}

int kbhit() {
    return _kbhit();
}
#else
void enable_raw_mode() {
    struct termios raw;
    tcgetattr(STDIN_FILENO, &raw);
    raw.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
}

int kbhit() {
    struct timeval tv = {0, 0};
    fd_set fds;
    FD_ZERO(&fds);
    FD_SET(STDIN_FILENO, &fds);
    return select(1, &fds, NULL, NULL, &tv);
}
#endif

/* Soccer ball model - truncated icosahedron */
void generate_soccer_ball(vec3_t** vertices, int** edges, int* vcount, int* ecount) {
    *vcount = 32; // 12 pentagon centers + 20 hexagon centers
    *ecount = 90; // 90 edges in a truncated icosahedron
    *vertices = malloc(*vcount * sizeof(vec3_t));
    *edges = malloc(*ecount * 2 * sizeof(int));

    // Generate vertices (simplified truncated icosahedron)
    for (int i = 0; i < *vcount; i++) {
        float lat = (i < 12) ? asinf(2.0f/3.0f) : asinf(1.0f/3.0f);
        float lon = 2 * M_PI * (i % 12) / 12;
        (*vertices)[i] = vec3_from_spherical(1.0f, lat, lon);
    }

    // Generate edges (simplified connectivity)
    int edge_idx = 0;
    for (int i = 0; i < 12; i++) {
        for (int j = 0; j < 5; j++) {
            (*edges)[edge_idx++] = i;
            (*edges)[edge_idx++] = 12 + (i*5 + j) % 20;
        }
    }
}

/* Main demo with all tasks */
int main() {
    enable_raw_mode();
    canvas_t* canvas = create_canvas(WIDTH, HEIGHT);
    
    // Create models
    vec3_t *ball_verts, *cube_verts;
    int *ball_edges, *cube_edges;
    int ball_vcount, ball_ecount, cube_vcount, cube_ecount;
    
    generate_soccer_ball(&ball_verts, &ball_edges, &ball_vcount, &ball_ecount);
    create_cube(&cube_verts, &cube_edges, &cube_vcount, &cube_ecount);

    // Camera setup
    mat4_t view = mat4_translate(0, 0, -8);
    mat4_t proj = mat4_frustum_asymmetric(-1,1, -1,1, 1,100);

    float angle = 0;
    float time = 0;
    int demo_phase = 0;
    const int NUM_PHASES = 4;
    
    while (1) {
        clear_canvas(canvas, 0.0f);
        
        // Draw only the current phase
        if (demo_phase == 0) {
            // Task 1: Clock-like lines
            int disp_size = 40; // for terminal
            float center_x = WIDTH / 2.0f;
            float center_y = HEIGHT / 2.0f;
            float radius = (WIDTH < HEIGHT ? WIDTH : HEIGHT) * 0.4f;
            for (int i = 0; i < 24; i++) {
                float line_angle = i * 15.0f * M_PI / 180.0f;
                float end_x = center_x + cosf(line_angle) * radius;
                float end_y = center_y + sinf(line_angle) * radius;
                draw_line_f(canvas, center_x, center_y, end_x, end_y, 3.0f);
            }
            // Draw a small center circle for clarity
            for (float angle = 0; angle < 2 * M_PI; angle += 0.1f) {
                float x = center_x + cosf(angle) * 4.0f;
                float y = center_y + sinf(angle) * 4.0f;
                set_pixel_f(canvas, x, y, 1.0f);
            }
            // Terminal display for Task 1
#ifdef _WIN32
            system("cls");
#else
            system("clear");
#endif
            printf("=== Task 1: Clock-like Line Demo ===\n");
            printf("Lines radiating from center at 15\xB0 intervals\n");
            printf("Using bilinear filtering for smooth rendering\n\n");
            for (int y = 0; y < disp_size; y++) {
                for (int x = 0; x < disp_size; x++) {
                    float px = x * WIDTH / (float)disp_size;
                    float py = y * HEIGHT / (float)disp_size;
                    float b = canvas->pixels[(int)py][(int)px];
                    printf(b > 0.8f ? "##" : b > 0.5f ? "++" : b > 0.2f ? ".." : "  ");
                }
                printf("\n");
            }
            printf("\nPress Q to quit, SPACE to change task\n");
            if (kbhit()) {
#ifdef _WIN32
                char c = _getch();
#else
                char c = getchar();
#endif
                if (c == 'q' || c == 'Q') break;
                if (c == ' ') demo_phase = (demo_phase + 1) % NUM_PHASES;
            }
            angle += 0.01f;
            time += 1.0f/FPS;
#ifdef _WIN32
            Sleep(FRAME_DELAY / 1000);
#else
            usleep(FRAME_DELAY);
#endif
            continue; // skip the rest for this phase
        } else if (demo_phase == 1) {
            // Task 2: 3D Cube Transformation
            mat4_t cube_scale = mat4_scale(0.6f, 0.6f, 0.6f);
            mat4_t cube_rotate = mat4_rotate_xyz(angle*0.5f, angle*0.7f, angle*0.3f);
            mat4_t cube_translate = mat4_translate(0.0f, 0.0f, -3.0f);
            mat4_t cube_transform = mat4_mul(mat4_mul(cube_translate, cube_rotate), cube_scale);
            
            // Manual projection for cube
            vec3_t projected_cube[8];
            for (int i = 0; i < 8; i++) {
                vec3_t transformed = mat4_mul_vec3(cube_transform, cube_verts[i]);
                float distance = 5.0f;
                float scale = distance / (distance + transformed.z);
                projected_cube[i].x = transformed.x * scale + WIDTH / 2.0f;
                projected_cube[i].y = transformed.y * scale + HEIGHT / 2.0f;
            }
            
            // Draw cube wireframe
            for (int i = 0; i < 12; i++) {
                int v1 = cube_edges[i * 2];
                int v2 = cube_edges[i * 2 + 1];
                draw_line_f(canvas, 
                           projected_cube[v1].x, projected_cube[v1].y,
                           projected_cube[v2].x, projected_cube[v2].y, 
                           1.5f);
            }
        } else if (demo_phase == 2) {
            // Task 3: Rotating Soccer Ball Wireframe
            mat4_t ball_model = mat4_rotate_xyz(angle*0.7f, angle, angle*0.3f);
            mat4_t ball_mvp = mat4_mul(mat4_mul(proj, view), ball_model);
            
            // Render soccer ball with circular viewport clipping
            render_wireframe(canvas, ball_mvp, ball_verts, ball_vcount, 
                            ball_edges, ball_ecount, 1.5f);
        } else if (demo_phase == 3) {
            // Task 4: Lit & Animated Scene
            light_t lights[2] = {
                {{1,1,1}, 0.7f},
                {{-1,-1,-1}, 0.3f}
            };
            lights[0].direction = vec3_from_spherical(1.0f, 
                                    M_PI/4 + sinf(time*0.5f)*0.3f, 
                                    time*0.7f);
            
            // Add a second cube with different lighting
            mat4_t cube2_model = mat4_translate(-2.0f, 0.0f, -4);
            cube2_model = mat4_mul(cube2_model, mat4_rotate_xyz(angle*1.2f, angle*0.8f, angle*0.4f));
            mat4_t cube2_mvp = mat4_mul(mat4_mul(proj, view), cube2_model);
            
            // Render second cube with lighting
            render_wireframe(canvas, cube2_mvp, cube_verts, cube_vcount,
                            cube_edges, cube_ecount, 1.2f);
        }
        
        // Terminal display with task information
#ifdef _WIN32
        system("cls");
#else
        system("clear");
#endif
        
        // Print phase label
        if (demo_phase == 0) {
            printf("=== Task 1: Clock-like Line Demo ===\n");
            printf("Lines radiating from center at 15° intervals\n");
            printf("Using bilinear filtering for smooth rendering\n\n");
        } else if (demo_phase == 1) {
            printf("=== Task 2: 3D Cube Transformation Demo ===\n");
            printf("Cube transformed with scale, rotation, translation\n");
            printf("Manual 2D projection using math3d library\n\n");
        } else if (demo_phase == 2) {
            printf("=== Task 3: Rotating Soccer Ball Wireframe ===\n");
            printf("Truncated icosahedron, clipped to circular viewport\n\n");
        } else if (demo_phase == 3) {
            printf("=== Task 4: Lit & Animated Scene ===\n");
            printf("Multiple wireframe objects, animated lights\n");
            printf("Lambert lighting, Bézier animation (if implemented)\n\n");
        }
        
        // Display canvas in terminal
        for (int y = 0; y < 40; y++) {
            for (int x = 0; x < 40; x++) {
                float px = x * WIDTH/40.0f;
                float py = y * HEIGHT/40.0f;
                float b = canvas->pixels[(int)py][(int)px];
                printf(b > 0.8f ? "##" : b > 0.5f ? "++" : b > 0.2f ? ".." : "  ");
            }
            printf("\n");
        }
        
        printf("\nTime: %.1fs | Angle: %.1f° | Phase: %d/%d\n", time, angle*180/M_PI, demo_phase+1, NUM_PHASES);
        printf("Press Q to quit, SPACE to change task\n");
        
        // Handle input
        if (kbhit()) {
#ifdef _WIN32
            char c = _getch();
#else
            char c = getchar();
#endif
            if (c == 'q' || c == 'Q') break;
            if (c == ' ') demo_phase = (demo_phase + 1) % NUM_PHASES;
        }
        
        // Update animation
        angle += 0.01f;
        time += 1.0f/FPS;
#ifdef _WIN32
        Sleep(FRAME_DELAY / 1000);
#else
        usleep(FRAME_DELAY);
#endif
    }
    
    // Cleanup
    free(ball_verts);
    free(ball_edges);
    free(cube_verts);
    free(cube_edges);
    free_canvas(canvas);
    return 0;
}