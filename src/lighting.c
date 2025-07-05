#include "lighting.h"

float lambert_intensity(vec3_t edge_dir, vec3_t light_dir) {
    // Normalize both vectors
    edge_dir = vec3_normalize_fast(edge_dir);
    light_dir = vec3_normalize_fast(light_dir);
    
    // Calculate dot product and clamp between 0 and 1
    float dot = vec3_dot(edge_dir, light_dir);
    return fmaxf(0.0f, dot);
}