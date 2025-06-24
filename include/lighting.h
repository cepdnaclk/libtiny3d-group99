#ifndef LIGHTING_H
#define LIGHTING_H

#include "math3d.h"

// === Lambert Lighting ===

/**
 * Computes Lambertian lighting intensity for a given edge direction and light direction.
 * If the edge faces the light, it appears brighter.
 *
 * @param edge_dir Direction vector of the edge (should be normalized)
 * @param light_dir Direction vector from the surface to the light (should be normalized)
 * @return intensity in [0.0, 1.0] based on dot product
 */
float compute_lambert_intensity(vec3_t edge_dir, vec3_t light_dir);

/**
 * Computes the total intensity from multiple light sources using Lambert model.
 *
 * @param edge_dir Direction of the edge (normalized)
 * @param lights Array of light direction vectors (normalized)
 * @param light_count Number of lights in the array
 * @return total intensity in [0.0, 1.0] (sum of individual Lambert intensities, capped at 1)
 */
float compute_total_lighting(vec3_t edge_dir, vec3_t* lights, int light_count);


// === Bézier Animation ===

/**
 * Computes a cubic Bézier curve position at time t.
 * Returns a smoothly interpolated position from p0 to p3 via p1 and p2.
 *
 * @param p0 Start point
 * @param p1 First control point
 * @param p2 Second control point
 * @param p3 End point
 * @param t Time value in [0.0, 1.0]
 * @return Position on the curve at time t
 */
vec3_t bezier(vec3_t p0, vec3_t p1, vec3_t p2, vec3_t p3, float t);

#endif // LIGHTING_H
