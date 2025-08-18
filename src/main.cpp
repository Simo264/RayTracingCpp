#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#include "Camera.hpp"
#include "Scene/Scene.hpp"
#include "Scene/Sphere.hpp"
#include "Scene/Plane.hpp"

#include "Material/Lambertian.hpp"
#include "Material/Metal.hpp"

#include "Renderer/Renderer.hpp"

int main()
{
  // Camera
  constexpr auto image_resolution = glm::ivec2(854, 480);
  constexpr auto camera_position = glm::vec3(-2.f, 2.f, 1.f);
  constexpr auto camera_look_at = glm::vec3(0.f, 0.f, -1.f);
  Camera camera(camera_position, image_resolution, camera_look_at, 20.f);

  // Materials
  auto material_ground = std::make_shared<Lambertian>(glm::vec3(0.8f, 0.8f, 0.0f));
  auto material_sphere_center = std::make_shared<Lambertian>(glm::vec3(0.1f, 0.2f, 0.5f));
  auto material_sphere_left = std::make_shared<Metal>(glm::vec3(0.8f, 0.8f, 0.8f), 0.3f);
  auto material_sphere_right = std::make_shared<Metal>(glm::vec3(0.8f, 0.6f, 0.2f), 1.f);

  // World
  auto ground = std::make_shared<Plane>(glm::vec3(0.f, -0.5f, 0.f), glm::vec3(0.f, 1.f, 0.f), material_ground);
  auto sphere_center = std::make_shared<Sphere>(glm::vec3(0.0f, 0.0f, -1.2f), 0.5f, material_sphere_center);
  auto sphere_left = std::make_shared<Sphere>(glm::vec3(-1.0f, 0.0f, -1.0f), 0.5f, material_sphere_left);
  auto sphere_right = std::make_shared<Sphere>(glm::vec3(1.0f, 0.0f, -1.0f), 0.5f, material_sphere_right);

  Scene scene;
  scene.add(ground);
  scene.add(sphere_center);
  scene.add(sphere_left);
  scene.add(sphere_right);
  
  // Render
  camera.captureImage(scene);
  camera.applyGammaCorrection(2.2f);
  auto& image = camera.image;
  
  stbi_write_png("image.png", 
                 image_resolution.x, image_resolution.y, 
                 3, 
                 image.data.get(),
                 image_resolution.x * 3);
  return 0;
}