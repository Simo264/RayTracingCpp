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
  constexpr auto camera_position = glm::vec3(0.f, 0.f, -3.f);
  constexpr auto camera_target = glm::vec3(0.f);
  constexpr auto image_resolution = glm::vec2{ 640, 480 };
  Camera camera(camera_position, camera_target, image_resolution);

  // Materials
  auto material_ground = std::make_shared<Lambertian>(glm::vec3(0.8f, 0.8f, 0.0f));
  auto material_sphere_center = std::make_shared<Lambertian>(glm::vec3(0.1f, 0.2f, 0.5f));
  
  //auto material_sphere_left = std::make_shared<Lambertian>(glm::vec3(0.1f, 0.2f, 0.5f));
  //auto material_sphere_right = std::make_shared<Lambertian>(glm::vec3(0.1f, 0.2f, 0.5f));
  auto material_sphere_left = std::make_shared<Metal>(glm::vec3(0.8, 0.8, 0.8), 0.3f);
  auto material_sphere_right = std::make_shared<Metal>(glm::vec3(0.8, 0.6, 0.2), 0.8f);

  // World
  auto ground_object = std::make_shared<Plane>(
    glm::vec3(0.f, -0.5f, 0.f),  // position
    glm::vec3(0.f, 1.f, 0.f),    // normal
    material_ground);
  
  auto sphere_center_object = std::make_shared<Sphere>(
    glm::vec3(0.0f, 0.0f, -1.2f), // position
    0.5f,                         // radius
    material_sphere_center);
  
  auto sphere_left_object = std::make_shared<Sphere>(
    glm::vec3(-1.0f, 0.0f, -1.0f),  // position
    0.5f,                           // radius
    material_sphere_left);
  
  auto sphere_right_object = std::make_shared<Sphere>(
    glm::vec3(1.0f, 0.0f, -1.0f),   // position
    0.5f,                           // radius
    material_sphere_right);
  
  Scene scene;
  scene.add(ground_object);
  scene.add(sphere_center_object);
  scene.add(sphere_left_object);
  scene.add(sphere_right_object);
  
  // Render
  camera.captureImage(scene);
  
  auto data = camera.getImageData();
  stbi_write_png("image_no_gamma.png", 
                 image_resolution.x, image_resolution.y,
                 3, 
                 data,
                 image_resolution.x * 3);

  camera.applyGammaCorrection(2.2f);
  stbi_write_png("image_gamma.png",
                 image_resolution.x, image_resolution.y,
                 3,
                 data,
                 image_resolution.x * 3);

  return 0;
}