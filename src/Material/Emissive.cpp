#include "Material/Emissive.hpp"

glm::vec3 Emissive::emitted(float u, float v) const
{
  if (emission_texture != nullptr)
    return emission_scale * emission_texture->sample(u, v);

  return emission_scale;
}