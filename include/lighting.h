#ifndef LIGHTING_H
#define LIGHTING_H

#include "math3d.h"

#define MAX_LIGHTS 8

/* Light structure */
typedef struct {
    vec3_t direction;  // Normalized light direction
    float intensity;   // Light strength (0.0 to 1.0)
} light_t;

/* Lighting calculation */
float lambert_lighting(vec3_t edge_dir, vec3_t light_dir);

/* Multi-light support */
float compute_lighting(vec3_t normal, light_t* lights, int light_count);

/* Light management */
void add_light(light_t* lights, int* light_count, vec3_t direction, float intensity);
void remove_light(light_t* lights, int* light_count, int index);

#endif // LIGHTING_H