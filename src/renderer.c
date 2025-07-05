#include "renderer.h"
#include <stdlib.h>
#include <math.h>

vec3_t project_vertex(mat4_t view_proj, vec3_t vertex, int screen_width, int screen_height) {
    // Transform the vertex through the view-projection matrix
    vec3_t projected = mat4_transform_vec3(view_proj, vertex);
    
    // Convert from NDC (-1 to 1) to screen coordinates
    projected.x = (projected.x + 1.0f) * 0.5f * screen_width;
    projected.y = (1.0f - (projected.y + 1.0f) * 0.5f) * screen_height;
    
    return projected;
}

bool clip_to_circular_viewport(canvas_t* canvas, float x, float y, float radius) {
    if (!canvas) return false;
    
    float center_x = canvas->width / 2.0f;
    float center_y = canvas->height / 2.0f;
    float dx = x - center_x;
    float dy = y - center_y;
    
    return (dx*dx + dy*dy) <= (radius*radius);
}

void render_wireframe(canvas_t* canvas, mat4_t view_proj, mesh_t* mesh) {
    if (!canvas || !mesh) return;
    
    // Project all vertices first
    vec3_t* projected_vertices = (vec3_t*)malloc(mesh->vertex_count * sizeof(vec3_t));
    for (int i = 0; i < mesh->vertex_count; i++) {
        projected_vertices[i] = project_vertex(view_proj, mesh->vertices[i], canvas->width, canvas->height);
    }
    
    // Draw all edges
    for (int i = 0; i < mesh->edge_count; i++) {
        int v0_idx = mesh->edges[i*2];
        int v1_idx = mesh->edges[i*2 + 1];
        
        if (v0_idx >= 0 && v0_idx < mesh->vertex_count && 
            v1_idx >= 0 && v1_idx < mesh->vertex_count) {
            vec3_t v0 = projected_vertices[v0_idx];
            vec3_t v1 = projected_vertices[v1_idx];
            
            // Simple clipping - skip lines where both points are outside the viewport
            bool v0_inside = clip_to_circular_viewport(canvas, v0.x, v0.y, canvas->width/2.0f);
            bool v1_inside = clip_to_circular_viewport(canvas, v1.x, v1.y, canvas->width/2.0f);
            
            if (v0_inside || v1_inside) {
                draw_line_f(canvas, v0.x, v0.y, v1.x, v1.y, 1.0f);
            }
        }
    }
    
    free(projected_vertices);
}

mesh_t generate_cube() {
    mesh_t mesh;
    mesh.vertex_count = 8;
    mesh.edge_count = 12;
    
    // Allocate memory
    mesh.vertices = (vec3_t*)malloc(mesh.vertex_count * sizeof(vec3_t));
    mesh.edges = (int*)malloc(mesh.edge_count * 2 * sizeof(int));
    
    // Define cube vertices (-1 to 1 in all dimensions)
    mesh.vertices[0] = vec3_create(-1, -1, -1);
    mesh.vertices[1] = vec3_create(1, -1, -1);
    mesh.vertices[2] = vec3_create(1, 1, -1);
    mesh.vertices[3] = vec3_create(-1, 1, -1);
    mesh.vertices[4] = vec3_create(-1, -1, 1);
    mesh.vertices[5] = vec3_create(1, -1, 1);
    mesh.vertices[6] = vec3_create(1, 1, 1);
    mesh.vertices[7] = vec3_create(-1, 1, 1);
    
    // Define edges (vertex indices)
    int cube_edges[] = {
        0,1, 1,2, 2,3, 3,0, // Bottom face
        4,5, 5,6, 6,7, 7,4, // Top face
        0,4, 1,5, 2,6, 3,7  // Connecting edges
    };
    
    for (int i = 0; i < mesh.edge_count * 2; i++) {
        mesh.edges[i] = cube_edges[i];
    }
    
    return mesh;
}

mesh_t generate_soccer_ball() {
    mesh_t mesh;
    
    // A truncated icosahedron has 32 faces (12 pentagons and 20 hexagons) and 60 vertices
    // For wireframe, we'll just define the edges
    
    // Golden ratio
    const float phi = (1.0f + sqrtf(5.0f)) / 2.0f;
    
    mesh.vertex_count = 60;
    mesh.edge_count = 90; // Each of 32 faces has edges, but we'll avoid duplicates
    
    mesh.vertices = (vec3_t*)malloc(mesh.vertex_count * sizeof(vec3_t));
    mesh.edges = (int*)malloc(mesh.edge_count * 2 * sizeof(int));
    
    // Generate vertices (simplified version)
    int v = 0;
    // 12 vertices from an icosahedron
    for (int i = 0; i < 12; i++) {
        float theta = (i < 5) ? 2.0f * M_PI * i / 5.0f : 2.0f * M_PI * (i-5) / 5.0f + M_PI/5.0f;
        float phi_angle = (i < 5) ? atan(0.5f) : M_PI - atan(0.5f);
        
        mesh.vertices[v++] = vec3_from_spherical(1.0f, theta, phi_angle);
    }
    
    // Additional vertices to create the truncated icosahedron pattern
    // (This is a simplified version - a complete implementation would be more complex)
    for (int i = 12; i < mesh.vertex_count; i++) {
        float t = (float)i / mesh.vertex_count;
        mesh.vertices[i] = vec3_create(
            cosf(2 * M_PI * t) * sinf(M_PI * t),
            sinf(2 * M_PI * t) * sinf(M_PI * t),
            cosf(M_PI * t)
        );
        mesh.vertices[i] = vec3_normalize_fast(mesh.vertices[i]);
    }
    
    // Define edges (simplified pattern)
    int e = 0;
    // Connect vertices to create pentagons and hexagons
    // This is a placeholder - a complete implementation would require more precise edge definitions
    for (int i = 0; i < 12; i++) {
        for (int j = i+1; j < 12; j++) {
            if (e < mesh.edge_count * 2) {
                mesh.edges[e++] = i;
                mesh.edges[e++] = j;
            }
        }
    }
    // Connect remaining vertices
    for (int i = 12; i < mesh.vertex_count; i++) {
        if (e < mesh.edge_count * 2) {
            mesh.edges[e++] = i;
            mesh.edges[e++] = (i + 1) % mesh.vertex_count;
        }
    }
    
    return mesh;
}