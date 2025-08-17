#include "Scene/Plane.hpp"
#include "Ray.hpp"
#include "Interval.hpp"

bool Plane::hit(const Ray& ray,
								const Interval& interval,
								HitRecord& rec) const
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

	auto p0 = __center;
	auto n = __normal;
	auto r0 = ray.origin();
	auto d = ray.direction();

	auto denom = glm::dot(d, n);
	if (glm::abs(denom) < 1e-6f) // Ray is parallel to the plane
		return false; 

	auto t = glm::dot(p0 - r0, n) / denom;
	if (!interval.surrounds(t))
		return false;

	rec.t = t;
	rec.p = ray.at(t);

	// Flip normal if ray hits from below
	bool is_ray_outside = denom < 0.f;
	rec.normal = is_ray_outside ? n : -n;
	rec.is_ray_outside = is_ray_outside;
	rec.material = __material;
	return true;
}
