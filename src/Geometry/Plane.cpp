#include "Geometry/Plane.hpp"
#include "Ray.hpp"

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

	auto p0 = center;
	auto n = normal;
	auto r0 = ray.origin;
	auto d = ray.direction;
	auto denom = glm::dot(d, n);
	if (glm::abs(denom) < 1e-6f) // Ray is parallel to the plane
		return false; 

	auto t = glm::dot(p0 - r0, n) / denom;
	if (t < t_min || t > t_max)
		return false;

	auto p = ray.at(t);

	// calculate texture coordinate
	auto tangent = glm::normalize(glm::cross(n, glm::vec3(1.f, 0.f, 0.f)));
	auto bitangent = glm::normalize(glm::cross(n, tangent));
	auto local_hit = p - p0;
	float u = glm::dot(local_hit, tangent);
	float v = glm::dot(local_hit, bitangent);
	// If you want the texture to repeat every unit
	u = fmod(u, 1.0f);
	v = fmod(v, 1.0f);
	if (u < 0.0f) u += 1.0f;
	if (v < 0.0f) v += 1.0f;

	hit.tc_u = u;
	hit.tc_v = v;
	hit.t = t;
	hit.normal = n;
	hit.point = p;
	hit.material = material;
	hit.is_ray_outside = true;

	// Flip normal if ray hits from below
	if (denom > 0.f)
	{
		hit.is_ray_outside = false;
		hit.normal = -n;
	}
	return true;
}
