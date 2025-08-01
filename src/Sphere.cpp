#include "Sphere.hpp"
#include "Ray.hpp"

bool Sphere::hit(const Ray& ray, float rayTmin, float rayTmax, HitRecord& rec) const
{
  //(P-C) dot (P-C) = r^2
  //(O+td - C) dot (O+td - C) = r^2
  //(d dot d)t^2 + 2d dot (O-C)t + (O-C) dot (O-C) - r^2 = 0
  auto& const d = ray.direction();
  auto& const o = ray.origin();
  glm::vec3 oc = o - _center;

  // at^2 + bt + c = 0
  // a = (d dot d)
  // b = 2d dot (O-C)
  // c = (O-C) dot (O-C) - r^2
  float a = glm::dot(d, d);
  float b = glm::dot(2.f * d, oc);
  float c = glm::dot(oc, oc) - glm::pow(_radius, 2);
  double delta = glm::pow(b, 2) - 4.f * a * c;
  if (delta < 0) // no intersections
    return false;

  double sqroot = std::sqrt(delta);
  double den = (2.0f * a);
  double t = (-b - sqroot) / den; // calculate the nearest point
  if (t > rayTmax || t < rayTmin)
  {
    t = (-b + sqroot) / den; // calculate the farest point
    if (t > rayTmax || t < rayTmin)
      return false;
  }

  glm::vec3 p = ray.at(t);
  glm::vec3 outerSurfaceNormal = (p - _center) / _radius;
  glm::vec3 N = outerSurfaceNormal;
  bool isRayOutside = true;
  if (glm::dot(d, N) > 0.f) // if ray is inside the sphere
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
