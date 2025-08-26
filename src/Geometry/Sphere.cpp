#include "Geometry/Sphere.hpp"
#include "Ray.hpp"

#include <cmath>
#include <glm/gtx/norm.hpp> // glm::length2

bool Sphere::intersect(const Ray& ray,
                       float t_min,
                       float t_max,
                       HitRecord& hit) const
{
  // A sphere is defined by a center point p0 and a radius r.
  // Any point p lies on the surface of the sphere if:
  // (p - p0) dot (p - p0) = r^2
  //
  // A ray is described using the parametric form:
  // p(t) = r0 + t*d
  //
  // To find if and where the ray intersects the sphere, we substitute p(t) into the sphere equation:
  // (r0 + t*d - p0) dot (r0 + t*d - p0) = r^2
  //
  // Expanding this expression yields a quadratic equation in t:
  // (d dot d)t^2 + 2*(d dot (r0 - p0))*t + ((r0 - p0) dot (r0 - p0) - r^2) = 0
  //
  // Solving this quadratic gives us potential intersection points.
  // If the discriminant (delta) is negative, there are no real roots -> no intersection.
  // Otherwise, we compute the roots and check whether they lie within the valid interval.

  auto p0 = center;
  auto d = ray.direction; // Assume already normalized
  auto r0 = ray.origin;
  auto r0p0 = r0 - p0;

  // Coefficients for the quadratic equation: at^2 + bt + c = 0
  auto a = glm::length2(d);
  auto b = 2.f * glm::dot(d, r0p0);
  auto c = glm::length2(r0p0) - glm::pow(radius, 2);
  auto delta = glm::pow(b, 2) - 4.f * a * c;
  if (delta < 1e-6f) // No real roots -> no intersection
    return false;

  auto sqroot = glm::sqrt(delta);
  auto den = (2.0f * a);
  auto t = (-b - sqroot) / den; // Try the nearest root first
  if (t < t_min || t > t_max)
  {
    t = (-b + sqroot) / den;    // Try the farther root
    if (t < t_min || t > t_max)
      return false;
  }

  auto p = ray.at(t);
  auto n = (p - p0) / radius; // Already normalized
  bool is_ray_outside = true;
  if (glm::dot(d, n) > 0.f) // Ray is inside the sphere
  {
    is_ray_outside = false;
    n = -n;
  }

  // Calculate texture coordinates
  auto p_local = glm::normalize(p - p0);          // point on unit sphere
  auto theta = glm::acos(p_local.y);              // angle from Y axis
  auto phi = std::atan2(p_local.z, p_local.x);    // angle around Y axis
  auto u = (phi + glm::pi<float>()) / (2.0f * glm::pi<float>());
  auto v = theta / glm::pi<float>();

  hit.t = t;
  hit.tc_u = u;
  hit.tc_v = v;
  hit.point = p;
  hit.normal = n;
  hit.is_ray_outside = is_ray_outside;
  hit.material = material;
	return true;
}
