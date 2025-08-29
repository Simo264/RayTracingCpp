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

  auto p0 = this->__position;
  auto d = ray.direction; // Assume already normalized
  auto r0 = ray.origin;
  auto r0p0 = r0 - p0;

  // Coefficients for the quadratic equation: at^2 + bt + c = 0
  auto a = glm::length2(d);
  auto b = 2.f * glm::dot(d, r0p0);
  auto c = glm::length2(r0p0) - glm::pow(__radius, 2);
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

  auto hit_point = ray.at(t);
  auto n = this->getNormal(hit_point); // Already normalized
  auto tc = this->getTextureCoordinates(hit_point);
  
  auto is_ray_outside = true;
  if (glm::dot(d, n) > 0.f) // Ray is inside the sphere
  {
    is_ray_outside = false;
    n = -n;
  }

  hit.t = t;
  hit.tc_u = tc.x;
  hit.tc_v = tc.y;
  hit.point = hit_point;
  hit.normal = n;
  hit.is_ray_outside = is_ray_outside;
  hit.material = this->__material;
	return true;
}

glm::vec3 Sphere::getNormal(const glm::vec3& p) const 
{
  auto center = __position;
  return glm::normalize(p - center);
}

glm::vec2 Sphere::getTextureCoordinates(const glm::vec3& p) const
{
  // For a sphere, texture mapping is typically done using spherical projection.
  // This involves converting the 3D Cartesian coordinates of the hit point into spherical coordinates (theta, phi) 
  // and then mapping them to the(u, v) texture space.

  // The texture coordinates (u, v) are based on the hit point on the sphere's surface.
  // The point 'p' is a world-space coordinate.
  // First, we need to convert the hit point to a local coordinate system centered on the sphere.
  auto local_p = (p - __position) / __radius;

  // Now, we convert these local Cartesian coordinates to spherical coordinates (phi, theta).
  // Phi is the polar angle (latitude), ranging from 0 to pi.
  // Theta is the azimuthal angle (longitude), ranging from -pi to pi.
  auto theta = std::atan2(local_p.z, local_p.x);
  auto phi = glm::acos(-local_p.y);

  // Finally, we map these spherical coordinates to the [0, 1] UV texture space.
  // U corresponds to longitude and V to latitude.
  auto u = (theta + glm::pi<float>()) / (2.0f * glm::pi<float>());
  auto v = phi / glm::pi<float>();
  return glm::vec2(u, v);
}