#include "csg.hpp"

#include <algorithm>
#include <glm/gtc/matrix_access.hpp>
#include <glm/gtx/intersect.hpp>

// query_box.cpp -----------------------------------------------------------------

namespace csg {

static bool box_intersects_box(const box_t& box, const box_t& other_box) {
    return glm::all(glm::lessThanEqual(box.min, other_box.max)) &&
           glm::all(glm::greaterThanEqual(box.max, other_box.min));
}

std::vector<brush_t*> world_t::query_box(const box_t& box) {
    std::vector<brush_t*> result;
    brush_t *b = first();
    while (b) {
        if (box_intersects_box(b->box, box))
            result.push_back(b);
        b = next(b);
    }
    return result;
}

}

// query_frustum.cpp -----------------------------------------------------------------

namespace csg {

static float signed_distance(const glm::vec3& point, const plane_t& plane) {
    return glm::dot(plane.normal, point) + plane.offset;
}

static bool is_bit_set(int x, int bit) {
    return (x >> bit) & 1;
}

// a 3-bit encoding representing a box corner
using box_corner_t = int;

static constexpr box_corner_t left_bottom_near  = 0b000;
static constexpr box_corner_t right_bottom_near = 0b001;
static constexpr box_corner_t left_top_near     = 0b010;
static constexpr box_corner_t right_top_near    = 0b011;
static constexpr box_corner_t left_bottom_far   = 0b100;
static constexpr box_corner_t right_bottom_far  = 0b101;
static constexpr box_corner_t left_top_far      = 0b110;
static constexpr box_corner_t right_top_far     = 0b111;

static glm::vec3 box_corner(box_t box, box_corner_t corner) {
    return glm::vec3(
        is_bit_set(corner, 0)? box.max.x: box.min.x,
        is_bit_set(corner, 1)? box.max.y: box.min.y,
        is_bit_set(corner, 2)? box.max.z: box.min.z
    );
}

// the box corner with the smallest signed distance to the plane
// (always the same corner with every box)
static box_corner_t min_box_corner(const plane_t& plane) {
    box_t box{glm::vec3(0,0,0), glm::vec3(1,1,1)}; 
    std::pair<float, box_corner_t> distance_corner_pairs[8];
    for (box_corner_t i=0; i<8; ++i) 
        distance_corner_pairs[i] = {
            signed_distance(box_corner(box, i), plane),
            i
        };
    return std::min_element(
        begin(distance_corner_pairs),
        end(distance_corner_pairs)
    )->second;
}

struct frustum_t {
    // left right bottom top near far
    plane_t planes[6];
    box_corner_t min_box_corners[6];
};

static frustum_t make_frustum_from_matrix(
    const glm::mat4& view_projection)
{
    // https://gdbooks.gitbooks.io/legacyopengl/content/Chapter8/frustum.html
    // The plane values can be found by adding or subtracting one of the 
    // first three rows of the viewProjection matrix from the fourth row.
    // Left Plane Row 0 (addition)
    // Right Plane Row 0 Negated (subtraction)
    // Bottom Plane Row 1 (addition)
    // Top Plane Row 1 Negated (subtraction)
    // Near Plane Row 2 (addition)
    // Far Plane Row 2 Negated (subtraction)
    glm::vec4 l = glm::row(view_projection, 3) + glm::row(view_projection, 0); 
    glm::vec4 r = glm::row(view_projection, 3) - glm::row(view_projection, 0); 
    glm::vec4 b = glm::row(view_projection, 3) + glm::row(view_projection, 1); 
    glm::vec4 t = glm::row(view_projection, 3) - glm::row(view_projection, 1); 
    glm::vec4 n = glm::row(view_projection, 3) + glm::row(view_projection, 2); 
    glm::vec4 f = glm::row(view_projection, 3) - glm::row(view_projection, 2); 
    // we need to negate because the above source has normals pointing *inside*
    // the frustum
    plane_t left  {-glm::vec3(l), -l.w};
    plane_t right {-glm::vec3(r), -r.w};
    plane_t bottom{-glm::vec3(b), -b.w};
    plane_t top   {-glm::vec3(t), -t.w};
    plane_t near  {-glm::vec3(n), -n.w};
    plane_t far   {-glm::vec3(f), -f.w};
    return frustum_t{
        { left, right, bottom, top, near, far },
        {
            min_box_corner(left),
            min_box_corner(right),
            min_box_corner(bottom),
            min_box_corner(top),
            min_box_corner(near),
            min_box_corner(far)
        }
    };
}

// inexact-- returns false positives, but good for frustum culling
bool frustum_intersects_box(const frustum_t& frustum, const box_t& box) {
    for (int i=0; i<6; ++i) {
        glm::vec3 min_corner = box_corner(box, frustum.min_box_corners[i]);
        if (signed_distance(min_corner, frustum.planes[i]) > 0.0f)
            return false;
    }
    return true;
   /*
    glm::vec3 center = (box.max + box.min) / 2.0f;

    for (int i=0; i<6; ++i) {
        if (signed_distance(center, frustum.planes[i]) > 0.0f)
            return false;
    }
    return true;
    */
}

std::vector<brush_t*> world_t::query_frustum(
    const glm::mat4& view_projection
)
{
    frustum_t frustum = make_frustum_from_matrix(view_projection);
    std::vector<brush_t*> result;
    brush_t *b = first();
    while (b) {
        if (frustum_intersects_box(frustum, b->box))
            result.push_back(b);
        b = next(b);
    }
    return result;
}

}

// query_point.cpp -----------------------------------------------------------------

namespace csg {

static bool box_contains_point(const box_t& box, const glm::vec3& point) {
    return box_intersects_box(box, box_t{point, point});
}

std::vector<brush_t*> world_t::query_point(const glm::vec3& point) {
    std::vector<brush_t*> result;
    brush_t *b = first();
    while (b) {
        if (box_contains_point(b->box, point))
            result.push_back(b);
        b = next(b);
    }
    return result;
}

}

// query_ray.cpp -----------------------------------------------------------------

namespace csg {

static glm::vec3 projection_of_point_onto_plane(const glm::vec3& point,
                                                const plane_t& plane)
{
    return point - signed_distance(point, plane) * plane.normal;
}

static bool ray_intersects_box(const ray_t& ray,
                               const box_t& box,
                               const glm::vec3& one_over_ray_direction)
{
    // branchless slab method 
    // https://tavianator.com/2015/ray_box_nan.html

    float t1 = (box.min[0] - ray.origin[0])*one_over_ray_direction[0];
    float t2 = (box.max[0] - ray.origin[0])*one_over_ray_direction[0];

    float tmin = glm::min(t1, t2);
    float tmax = glm::max(t1, t2);

    for (int i = 1; i < 3; ++i) {
        t1 = (box.min[i] - ray.origin[i])*one_over_ray_direction[i];
        t2 = (box.max[i] - ray.origin[i])*one_over_ray_direction[i];

        tmin = glm::max(tmin, glm::min(t1, t2));
        tmax = glm::min(tmax, glm::max(t1, t2));
    }

    return tmax > glm::max(tmin, 0.0f);      
}

static bool ray_intersects_plane(const ray_t& ray,
                                 const plane_t& plane,
                                 float& t)
{
    return glm::intersectRayPlane(
        ray.origin,
        ray.direction,
        projection_of_point_onto_plane(glm::vec3(0,0,0), plane),
        plane.normal,
        t
    );
}

static bool point_inside_convex_polygon(const glm::vec3& point,
                                        const std::vector<vertex_t>& vertices)
{
    int n = vertices.size();
    if (n < 3) 
        return false;

    glm::vec3 v0 = vertices[0].position;
    glm::vec3 v1 = vertices[1].position;
    glm::vec3 v2 = vertices[2].position;
    glm::vec3 normal = glm::cross(v1-v0, v2-v0);

    for (int i=0; i<n; ++i) {
        int j = (i+1) % n;
        
        glm::vec3 vi = vertices[i].position;
        glm::vec3 vj = vertices[j].position;

        // if (glm::dot(normal, glm::cross(vj-vi, point-vi)) < 0.0f)
        
        static constexpr float epsilon = 0.001f;
        if (glm::dot(normal, glm::cross(vj-vi, point-vi)) < -epsilon)
            return false;
    }

    return true;
}

std::vector<ray_hit_t> world_t::query_ray(const ray_t& ray) {
    glm::vec3 one_over_ray_direction = 1.0f / ray.direction;

    std::vector<ray_hit_t> result;
    brush_t *b = first();
    while (b) {
        if (ray_intersects_box(ray, b->box, one_over_ray_direction)) {
            for (size_t iplane=0; iplane<b->planes.size(); ++iplane) {
                float t;
                if (ray_intersects_plane(ray, b->planes[iplane], t)) {
                    glm::vec3 intersection = ray.origin + t * ray.direction;
                    face_t& face = b->faces[iplane];
                    if (point_inside_convex_polygon(intersection,
                                                    face.vertices)) {
                        for (fragment_t& fragment: face.fragments) {
                            if (point_inside_convex_polygon(intersection,
                                                            face.vertices)) {
                                ray_hit_t ray_hit;
                                ray_hit.brush = b;
                                ray_hit.face = &face;
                                ray_hit.fragment = &fragment;
                                ray_hit.parameter = t;
                                ray_hit.position = intersection;
                                result.push_back(ray_hit);
                            }
                        }
                    }
                }
            }
        }
        b = next(b);
    }

    std::sort(result.begin(), result.end(),
        [](const ray_hit_t& hit0, const ray_hit_t& hit1) {
            return hit0.parameter < hit1.parameter;
        }
    );
    return result;
}

}
