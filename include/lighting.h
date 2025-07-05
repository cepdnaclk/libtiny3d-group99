#ifndef LIGHTING_H
#define LIGHTING_H

#include "math3d.h"

typedef struct {
    vec3_t direction;
    float intensity;
} light_t;

float lambert_intensity(vec3_t edge_dir, vec3_t light_dir);

#endif // LIGHTING_H