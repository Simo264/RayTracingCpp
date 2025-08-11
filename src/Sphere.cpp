#include "Sphere.hpp"
#include "Ray.hpp"
#include "Interval.hpp"

bool Sphere::hit(const Ray& ray, 
                 const Interval& interval, 
                 HitRecord& rec) const
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

  auto p0 = _center;
  auto d = ray.direction(); // Assume already normalized
  auto r0 = ray.origin();
  glm::vec3 r0p0 = r0 - p0;

  // Coefficients for the quadratic equation: at^2 + bt + c = 0
  auto a = glm::dot(d, d);
  auto b = 2.f * glm::dot(d, r0p0);
  auto c = glm::dot(r0p0, r0p0) - glm::pow(_radius, 2);
  auto delta = glm::pow(b, 2) - 4.f * a * c;
  if (delta < 1e-6f) // No real roots -> no intersection
    return false;

  auto sqroot = std::sqrt(delta);
  auto den = (2.0f * a);
  auto t = (-b - sqroot) / den; // Try the nearest root first
  if (!interval.surrounds(t))
  {
    t = (-b + sqroot) / den; // Try the farther root
    if (!interval.surrounds(t))
      return false;
  }

  glm::vec3 p = ray.at(t);
  glm::vec3 N = (p - p0) / _radius; // Already normalized
  bool isRayOutside = true;
  if (glm::dot(d, N) > 0.f) // Ray is inside the sphere
  {
    isRayOutside = false;
    N = -N;
  }

  rec.t = static_cast<float>(t);
  rec.p = p;
  rec.normal = N;
  rec.isRayOutside = isRayOutside;
	return true;
}
