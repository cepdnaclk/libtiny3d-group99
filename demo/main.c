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
#include <sys/select.h>
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
    *vcount = 32;
    *ecount = 90;
    *vertices = malloc(*vcount * sizeof(vec3_t));
    *edges = malloc(*ecount * 2 * sizeof(int));

    for (int i = 0; i < *vcount; i++) {
        float lat = (i < 12) ? asinf(2.0f/3.0f) : asinf(1.0f/3.0f);
        float lon = 2 * M_PI * (i % 12) / 12;
        (*vertices)[i] = vec3_from_spherical(1.0f, lat, lon);
    }

    int edge_idx = 0;
    for (int i = 0; i < 12; i++) {
        for (int j = 0; j < 5; j++) {
            (*edges)[edge_idx++] = i;
            (*edges)[edge_idx++] = 12 + (i*5 + j) % 20;
        }
    }
}

int main() {
    enable_raw_mode();
    canvas_t* canvas = create_canvas(WIDTH, HEIGHT);
    
    vec3_t *ball_verts, *cube_verts;
    int *ball_edges, *cube_edges;
    int ball_vcount, ball_ecount, cube_vcount, cube_ecount;
    
    generate_soccer_ball(&ball_verts, &ball_edges, &ball_vcount, &ball_ecount);
    create_cube(&cube_verts, &cube_edges, &cube_vcount, &cube_ecount);

    mat4_t view = mat4_translate(0, 0, -8);
    mat4_t proj = mat4_frustum_asymmetric(-1, 1, -1, 1, 1, 100);

    float angle = 0;
    float time = 0;
    int demo_phase = 0;
    const int NUM_PHASES = 4;
    
    while (1) {
        clear_canvas(canvas, 0.0f);
        
        if (demo_phase == 0) {
            int disp_size = 40;
            float center_x = WIDTH / 2.0f;
            float center_y = HEIGHT / 2.0f;
            float radius = (WIDTH < HEIGHT ? WIDTH : HEIGHT) * 0.4f;

            for (int i = 0; i < 24; i++) {
                float line_angle = i * 15.0f * M_PI / 180.0f;
                float end_x = center_x + cosf(line_angle) * radius;
                float end_y = center_y + sinf(line_angle) * radius;
                draw_line_f(canvas, center_x, center_y, end_x, end_y, 3.0f);
            }

            for (float a = 0; a < 2 * M_PI; a += 0.1f) {
                float x = center_x + cosf(a) * 4.0f;
                float y = center_y + sinf(a) * 4.0f;
                set_pixel_f(canvas, x, y, 1.0f);
            }
        } 
        
        else if (demo_phase == 1) {
            // Task 2: 3D Cube Transformation using math3d
            int disp_size = 40;
            char grid[40][41];
            for (int y = 0; y < disp_size; y++) {
                for (int x = 0; x < disp_size; x++) grid[y][x] = ' ';
                grid[y][disp_size] = '\0';
            }
            mat4_t scale_mat = mat4_scale(0.8f, 0.8f, 0.8f);
            mat4_t rot_mat = mat4_rotate_xyz(angle * 0.5f, angle * 0.7f, angle * 0.3f);
            mat4_t trans_mat = mat4_translate(0.0f, 0.0f, -3.0f);
            mat4_t model = mat4_mul(trans_mat, mat4_mul(rot_mat, scale_mat));
            float x2d[8], y2d[8];
            for (int i = 0; i < cube_vcount; i++) {
                vec3_t transformed = mat4_mul_vec3(model, cube_verts[i]);
                float distance = 5.0f;
                float scale = distance / (distance + transformed.z);
                // Project to 2D and scale to fit grid
                x2d[i] = transformed.x * scale * 12 + disp_size/2;
                y2d[i] = transformed.y * scale * 12 + disp_size/2;
                // Draw vertex
                int px = (int)(x2d[i]);
                int py = (int)(y2d[i]);
                if (px >= 0 && px < disp_size && py >= 0 && py < disp_size)
                    grid[py][px] = 'O';
            }
            // Draw edges
            for (int i = 0; i < cube_ecount; i++) {
                int v1 = cube_edges[i * 2];
                int v2 = cube_edges[i * 2 + 1];
                int x0 = (int)x2d[v1], y0 = (int)y2d[v1];
                int x1 = (int)x2d[v2], y1 = (int)y2d[v2];
                int dx = abs(x1-x0), dy = abs(y1-y0), sx = x0<x1?1:-1, sy = y0<y1?1:-1, err = dx-dy;
                while (x0 != x1 || y0 != y1) {
                    if (x0 >= 0 && x0 < disp_size && y0 >= 0 && y0 < disp_size && grid[y0][x0] == ' ')
                        grid[y0][x0] = '.';
                    int e2 = 2*err;
                    if (e2 > -dy) { err -= dy; x0 += sx; }
                    if (e2 < dx) { err += dx; y0 += sy; }
                }
            }
            // Print grid
            for (int y = 0; y < disp_size; y++) printf("%s\n", grid[y]);
            // Skip the normal canvas rendering for this phase
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
            time += 1.0f / FPS;
#ifdef _WIN32
            Sleep(FRAME_DELAY / 1000);
#else
            usleep(FRAME_DELAY);
#endif
            continue;
        } 
        
        else if (demo_phase == 2) {
            mat4_t ball_model = mat4_rotate_xyz(angle * 0.7f, angle, angle * 0.3f);
            mat4_t ball_mvp = mat4_mul(mat4_mul(proj, view), ball_model);
            
            render_wireframe(canvas, ball_mvp, ball_verts, ball_vcount, 
                             ball_edges, ball_ecount, 1.5f);
        } 
        
        else if (demo_phase == 3) {
            mat4_t ball_model = mat4_rotate_xyz(angle * 0.7f, angle, angle * 0.3f);
            mat4_t ball_mvp = mat4_mul(mat4_mul(proj, view), ball_model);
            render_wireframe(canvas, ball_mvp, ball_verts, ball_vcount, 
                             ball_edges, ball_ecount, 1.5f);

            mat4_t cube2_model = mat4_translate(-2.0f, 0.0f, -4);
            cube2_model = mat4_mul(cube2_model, mat4_rotate_xyz(angle * 1.2f, angle * 0.8f, angle * 0.4f));
            mat4_t cube2_mvp = mat4_mul(mat4_mul(proj, view), cube2_model);
            render_wireframe(canvas, cube2_mvp, cube_verts, cube_vcount,
                             cube_edges, cube_ecount, 1.2f);
        }

#ifdef _WIN32
        system("cls");
#else
        system("clear");
#endif
        
        if (demo_phase == 0) {
            printf("=== Task 1: Clock-like Line Demo ===\n");
        } else if (demo_phase == 1) {
            printf("=== Task 2: 3D Cube Transformation Demo ===\n");
        } else if (demo_phase == 2) {
            printf("=== Task 3: Rotating Soccer Ball Wireframe ===\n");
        } else if (demo_phase == 3) {
            printf("=== Task 4: Lit & Animated Scene ===\n");
        }

        for (int y = 0; y < 40; y++) {
            for (int x = 0; x < 40; x++) {
                float px = x * WIDTH / 40.0f;
                float py = y * HEIGHT / 40.0f;
                float b = canvas->pixels[(int)py][(int)px];
                printf(b > 0.8f ? "##" : b > 0.5f ? "++" : b > 0.2f ? ".." : "  ");
            }
            printf("\n");
        }
        
        printf("\nTime: %.1fs | Angle: %.1f° | Phase: %d/%d\n", time, angle * 180 / M_PI, demo_phase + 1, NUM_PHASES);
        printf("Press Q to quit, SPACE to change task\n");

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
        time += 1.0f / FPS;
#ifdef _WIN32
        Sleep(FRAME_DELAY / 1000);
#else
        usleep(FRAME_DELAY);
#endif
    }
    
    free(ball_verts);
    free(ball_edges);
    free(cube_verts);
    free(cube_edges);
    free_canvas(canvas);
    return 0;
}
