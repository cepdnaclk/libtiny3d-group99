#include "lighting.h"
#include <math.h>

/* Lambertian lighting calculation */
float lambert_lighting(vec3_t edge_dir, vec3_t light_dir) {
    // Normalize both vectors
    vec3_normalize_fast(&edge_dir);
    vec3_normalize_fast(&light_dir);
    
    // Calculate dot product and clamp to [0,1]
    float intensity = vec3_dot(edge_dir, light_dir);
    return fmaxf(0.0f, intensity);
}

/* Compute lighting from multiple sources */
float compute_lighting(vec3_t normal, light_t* lights, int light_count) {
    vec3_normalize_fast(&normal);
    float total = 0.0f;
    
    for (int i = 0; i < light_count; i++) {
        vec3_t light_dir = lights[i].direction;
        vec3_normalize_fast(&light_dir);
        
        float contribution = vec3_dot(normal, light_dir);
        if (contribution > 0) {
            total += contribution * lights[i].intensity;
        }
    }
    
    // Clamp total intensity to [0,1] range
    return fminf(1.0f, fmaxf(0.0f, total));
}

/* Light management functions */
void add_light(light_t* lights, int* light_count, vec3_t direction, float intensity) {
    if (*light_count >= MAX_LIGHTS) return;
    
    lights[*light_count].direction = direction;
    lights[*light_count].intensity = intensity;
    vec3_normalize_fast(&lights[*light_count].direction);
    
    (*light_count)++;
}

void remove_light(light_t* lights, int* light_count, int index) {
    if (index < 0 || index >= *light_count) return;
    
    // Shift remaining lights down
    for (int i = index; i < *light_count - 1; i++) {
        lights[i] = lights[i+1];
    }
    
    (*light_count)--;
}

/* Edge lighting calculation for wireframes */
float calculate_edge_lighting(vec3_t v0, vec3_t v1, light_t* lights, int light_count) {
    vec3_t edge_dir = vec3_sub(v1, v0);
    vec3_t edge_normal = {edge_dir.y, -edge_dir.x, 0}; // Perpendicular in screen space
    return compute_lighting(edge_normal, lights, light_count);
}