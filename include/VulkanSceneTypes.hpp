#pragma once
#include <glm/glm.hpp>

struct VulkanMaterial
{
  glm::vec4 color_scale;
  glm::vec4 emission_scale;
  float roughness_scale;
  int material_id;
  float _pad[2];
};

struct VulkanSphere
{
  glm::vec3 center;
  float radius;
  VulkanMaterial material;
};

struct VulkanPlane
{
  glm::vec4 position;
  glm::vec4 normal;
  glm::vec2 size;
  float _pad[2];
  VulkanMaterial material;
};