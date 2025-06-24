#include <math.h>
#include "../include/lighting.h"

// === Lambert Lighting (Diffuse) ===

float compute_lambert_intensity(vec3_t edge_dir, vec3_t light_dir) {
    edge_dir = vec3_normalize_fast(edge_dir);
    light_dir = vec3_normalize_fast(light_dir);

    float intensity = vec3_dot(edge_dir, light_dir);
    return fmaxf(0.0f, intensity);  // Clamp to [0, 1]
}

float compute_total_lighting(vec3_t edge_dir, vec3_t* lights, int light_count) {
    float total = 0.0f;

    for (int i = 0; i < light_count; ++i) {
        total += compute_lambert_intensity(edge_dir, lights[i]);
    }

    if (total > 1.0f) total = 1.0f;
    return total;
}

// === Cubic Bézier Animation ===

vec3_t bezier(vec3_t p0, vec3_t p1, vec3_t p2, vec3_t p3, float t) {
    float u = 1.0f - t;
    float tt = t * t;
    float uu = u * u;
    float uuu = uu * u;
    float ttt = tt * t;

    vec3_t result = {0};

    result = vec3_add(result, vec3_scale(p0, uuu));           // u^3 * p0
    result = vec3_add(result, vec3_scale(p1, 3 * uu * t));     // 3u^2t * p1
    result = vec3_add(result, vec3_scale(p2, 3 * u * tt));     // 3ut^2 * p2
    result = vec3_add(result, vec3_scale(p3, ttt));            // t^3 * p3

    return result;
}
