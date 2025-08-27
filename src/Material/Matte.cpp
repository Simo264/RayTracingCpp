#include "Material/Matte.hpp"
#include "Geometry/IHittableObject.hpp"

#include "Ray.hpp"
#include "Scene.hpp"

#include <glm/gtx/norm.hpp>		// glm::length2
#include <glm/gtc/random.hpp> // glm::sphericalRand

extern Scene g_scene;

bool Matte::scatter(const Ray& incident,
										const HitRecord& hit,
										glm::vec3& surface_color,
										Ray& scattered_ray) const
{
  // We generate a new direction by adding the surface normal to a random unit vector.
  auto scatter_dir = hit.normal + glm::normalize(glm::sphericalRand(1.0f));

  // If the random vector is exactly opposite the normal, 
  // their sum could be near zero, resulting in a degenerate direction.
  if (glm::length2(scatter_dir) < 1e-8f)
    scatter_dir = hit.normal;

  scattered_ray = Ray(hit.point, glm::normalize(scatter_dir));

  // If a texture is provided, it samples the texture at the hit point's UV coordinates and 
  // multiplies it by the base color scale.
  surface_color = color_scale;
  if (color_texture != nullptr)
    surface_color = color_scale * color_texture->sample(hit.tc_u, hit.tc_v);

  return true;
}
