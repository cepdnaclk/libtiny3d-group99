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

#define WIDTH 800
#define HEIGHT 600
#define FPS 60
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

/* Soccer ball model */
void generate_soccer_ball(vec3_t** vertices, int** edges, int* vcount, int* ecount) {
    *vcount = 32; // 12 pentagon centers + 20 hexagon centers
    *ecount = 90; // 90 edges in a truncated icosahedron
    *vertices = malloc(*vcount * sizeof(vec3_t));
    *edges = malloc(*ecount * 2 * sizeof(int));

    // Golden ratio constants
    const float phi = (1.0f + sqrtf(5.0f)) / 2.0f;
    const float radius = sqrtf(phi*phi + 1.0f);

    // Generate vertices (simplified pattern)
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

/* Main demo */
int main() {
    enable_raw_mode();
    canvas_t* canvas = create_canvas(WIDTH, HEIGHT);
    
    // Create models
    vec3_t *ball_verts, *cube_verts;
    int *ball_edges, *cube_edges;
    int ball_vcount, ball_ecount, cube_vcount, cube_ecount;
    
    generate_soccer_ball(&ball_verts, &ball_edges, &ball_vcount, &ball_ecount);
    create_cube(&cube_verts, &cube_edges, &cube_vcount, &cube_ecount);

    // Setup lights
    light_t lights[2] = {
        {{1,1,1}, 0.7f},  // Main light
        {{-1,-1,-1}, 0.3f} // Fill light
    };

    // Camera setup
    mat4_t view = mat4_translate(0, 0, -8);
    mat4_t proj = mat4_frustum_asymmetric(-1,1, -1,1, 1,100);

    float angle = 0;
    float time = 0;
    int demo_phase = 0;
    
    while (1) {
        clear_canvas(canvas, 0.1f);
        
        // Animate lights
        lights[0].direction = vec3_from_spherical(1.0f, 
                                M_PI/4 + sinf(time*0.5f)*0.3f, 
                                time*0.7f);
        
        // Rotate models
        mat4_t ball_model = mat4_rotate_xyz(angle*0.7f, angle, angle*0.3f);
        mat4_t cube_model = mat4_translate(3*cosf(angle), 2*sinf(angle*0.7f), 0);
        cube_model = mat4_mul(cube_model, mat4_rotate_xyz(angle, angle*1.3f, 0));
        
        // Render soccer ball (with lighting)
        render_wireframe(canvas, 
                        mat4_mul(mat4_mul(proj, view), ball_model),
                        ball_verts, ball_vcount, 
                        ball_edges, ball_ecount, 
                        1.5f);
        
        // Render cube (with different lighting)
        render_wireframe(canvas,
                        mat4_mul(mat4_mul(proj, view), cube_model),
                        cube_verts, cube_vcount,
                        cube_edges, cube_ecount,
                        1.2f);
        
        // Simple terminal display
#ifdef _WIN32
        system("cls");
#else
        system("clear");
#endif
        for (int y = 0; y < 30; y++) {
            for (int x = 0; x < 60; x++) {
                float px = x * WIDTH/60.0f;
                float py = y * HEIGHT/30.0f;
                float b = canvas->pixels[(int)py][(int)px];
                printf(b > 0.8f ? "##" : b > 0.5f ? "++" : b > 0.2f ? ".." : "  ");
            }
            printf("\n");
        }
        printf("3D Renderer Demo - Press Q to quit\n");
        printf("Time: %.1fs | Angle: %.1f°\n", time, angle*180/M_PI);
        
        // Handle input
        if (kbhit()) {
            char c = getchar();
            if (c == 'q' || c == 'Q') break;
        }
        
        // Update animation
        angle += 0.03f;
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