#include "Geometry/Plane.hpp"
#include "Ray.hpp"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/norm.hpp> // glm::length2

bool Plane::intersect(const Ray& ray,
											float t_min,
											float t_max,
											HitRecord& hit) const
{
	// A plane is characterized by a point p (plane center) indicating its distance from the world's origin,
	// and a normal n, which defines the plane's orientation.
	// We can derive a vector on the plane from any point p on it by subtracting p0 from p.
	// Since this resultant vector lies within the plane, it is perpendicular to the plane's normal
	// Leveraging the property that the dot product of two perpendicular vectors equals 0, we have:
	// (p - p0) dot n = 0
	//
	// Likewise, a ray is described using the parametric form: 
	// p(t) = r0 + td
	//
	// If the ray intersects the plane, they share a point p at the intersection:
	// (p - p0) dot n = 0
	// (r0 + td - p0) dot n = 0
	// Our goal is to find a value for t that allows us to compute the intersection point's position 
	// using the ray's parametric equation:
	// t = - ((r0-p0) dot n)/(d dot n)
	//   = ((p0-r0) dot n)/(d dot n)
	//
	// It's worth noting that if the plane and ray are parallel we return false (indicating no intersection)
	// when the denominator is less than a very small threshold.

	auto p0 = this->__position;
	auto n = this->__orientation;
	auto r0 = ray.origin;
	auto d = ray.direction;
	auto denom = glm::dot(d, n);
	if (glm::abs(denom) < 1e-6f) // Ray is parallel to the plane
		return false; 

	auto t = glm::dot(p0 - r0, n) / denom;
	if (t < t_min || t > t_max)
		return false;

	auto hit_point = ray.at(t);
	auto local_coords = this->getTextureCoordinates(hit_point);
	// Check if the hit point is within the finite dimensions of the plane.
	if (glm::abs(local_coords.x) > (__width / 2.0f) || glm::abs(local_coords.y) > (__height / 2.0f))
		return false;

	// Normalize texture coordinates to be in the [0, 1] range and handle wrapping.
	local_coords.x = (local_coords.x / __width) + 0.5f;
	local_coords.y = (local_coords.y / __height) + 0.5f;

	hit.t = t;
	hit.tc_u = local_coords.x;
	hit.tc_v = local_coords.y;
	hit.point = hit_point;
	hit.material = this->__material;
	
	if (glm::dot(ray.direction, n) < 0)
	{
		hit.is_ray_outside = true;
		hit.normal = n;
	}
	else
	{
		hit.is_ray_outside = false;
		hit.normal = -n;
	}
	return true;
}

glm::vec2 Plane::getTextureCoordinates(const glm::vec3& p) const
{
	// For a plane, texture coordinates are typically a 2D projection of the hit point onto the plane's surface. 
	// We need to define a local coordinate system (tangent and bitangent vectors) on the plane to map 
	// the 3D point to a 2D (u, v) pair.

	// Get the plane's normal.
	auto n = this->__orientation;

	// Calculate a stable orthogonal basis (tangent and bitangent) for the plane.
	auto tangent = glm::vec3();
	if (glm::abs(n.x) > glm::abs(n.y))
		tangent = glm::normalize(glm::vec3(n.z, 0.0f, -n.x));
	else
		tangent = glm::normalize(glm::vec3(0.0f, -n.z, n.y));
	auto bitangent = glm::cross(n, tangent);

	// The hit point 'p' is in world space. We need to project it onto the plane's local coordinate system.
	auto local_hit = p - this->__position;
	auto u = glm::dot(local_hit, tangent);
	auto v = glm::dot(local_hit, bitangent);
	return glm::vec2(u, v);
}
