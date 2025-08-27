#pragma once

#include "IMaterial.hpp"

class Emissive : public IMaterial
{
public:
  Emissive(glm::vec3 emission_scale) : IMaterial()
  {
    this->emission_scale = emission_scale;
  }
  Emissive(std::shared_ptr<Texture2D> emission_texture) : IMaterial()
  {
    this->emission_scale = glm::vec3(1.f);
    this->emission_texture = emission_texture;
  }

	~Emissive() = default;

  bool scatter(const Ray& incident,
               const HitRecord& hit,
               glm::vec3& surface_color,
               Ray& scattered_ray) const
  {
    return false;
  }

  glm::vec3 emitted(float u, float v) const override;
};