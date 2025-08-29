#include "Material/Matte.hpp"
#include "Geometry/IHittableObject.hpp"

#include "Ray.hpp"

#include <glm/gtx/norm.hpp>		// glm::length2
#include <glm/gtc/random.hpp> // glm::sphericalRand

/**
 * We'll start with diffuse materials (also called matte).
 * Light that reflects off a diffuse surface has its direction randomized, 
 * so, if we send three rays into a crack between two diffuse surfaces they will each have different random behavior.
 * They might also be absorbed rather than reflected. The darker the surface, the more likely the ray is absorbed
 * (that’s why it's dark!). Really any algorithm that randomizes direction will produce surfaces that look matte.
 * For this material, a ray that hits the surface has an equal probability of bouncing in any direction 
 * away from the surface.
 * 
 * A more accurate representation of real diffuse objects is the Lambertian distribution.
 * This distribution scatters reflected rays in a manner that is proportional to cos(theta),
 * where 'theta' is the angle between the reflected ray and the surface normal. 
 * This means that a reflected ray is most likely to scatter in a direction near the surface normal, 
 * and less likely to scatter in directions away from the normal.
 * This non-uniform Lambertian distribution does a better job of modeling material reflection in the real world 
 * than our previous uniform scattering.
 * 
 * We can create this distribution by adding a random unit vector to the normal vector. At the point of intersection 
 * on a surface there is the hit point, p, and there is the normal of the surface, n.
 */

bool Matte::scatter(const Ray& incident,
										const HitRecord& hit,
										glm::vec3& surface_color,
										Ray& scattered_ray) const
{
  // We generate a new direction by adding the surface normal to a random unit vector.
  auto random_dir = glm::normalize(glm::sphericalRand(1.0f));
  auto scatter_dir = hit.normal + glm::normalize(random_dir);

  // If the random unit vector we generate is exactly opposite the normal vector, 
  // the two will sum to zero, which will result in a zero scatter direction vector. 
  // This leads to bad scenarios later on (infinities and NaNs)
  if (glm::length2(scatter_dir) < 1e-6f)
    scatter_dir = hit.normal;

  scattered_ray = Ray(hit.point, glm::normalize(scatter_dir));

  // If a texture is provided, it samples the texture at the hit point's UV coordinates and 
  // multiplies it by the base color scale.
  surface_color = color_scale;
  if (color_texture != nullptr)
    surface_color = color_scale * color_texture->sample(hit.tc_u, hit.tc_v);

  return true;
}
