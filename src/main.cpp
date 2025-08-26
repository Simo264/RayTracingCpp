#include <glm/glm.hpp>
#include <iostream>

#include "ImageLoader.hpp"
#include "Camera.hpp"
#include "Scene.hpp"
#include "Renderer.hpp"
#include "Geometry/Sphere.hpp"
#include "Geometry/Plane.hpp"
#include "Material/Matte.hpp"
#include "Material/Metal.hpp"

namespace fs = std::filesystem;

static auto getResourcesPath()
{
  static auto current_path = fs::current_path();
  static auto res_path = (current_path.parent_path() / "resources").lexically_normal();
  return res_path;
}


int main()
{
  auto resources_path = getResourcesPath();

  // Camera
  constexpr auto camera_position = glm::vec3(0.f, 0.8f, -4.5);
  constexpr auto camera_target = glm::vec3(0.f, 0.5f, 0.f);
  constexpr auto image_resolution = glm::uvec2(320u, 240u);
  constexpr float focal_length = 40.f;
  Camera camera(camera_position, camera_target, image_resolution, focal_length);
  camera.samples_per_pixel = 64u;

  // Materials
  auto texture_solid_color_brown = createTexture2D(glm::vec3(1.f, 0.87f, 0.67f));
  auto material_matte_brown = createMaterial<Matte>(texture_solid_color_brown);
  
  auto texture_plastic_orange = createTexture2D(resources_path / "Plastic_orange/Plastic014A_1K-PNG_Color.png");
  auto material_plastic = createMaterial<Matte>(texture_plastic_orange);

  auto texture_metal_white_color = createTexture2D(resources_path / "Metal_white/Metal049A_1K-PNG_Color.png");
  auto texture_metal_white_roughness = createTexture2D(resources_path / "Metal_white/Metal049A_1K-PNG_Roughness.png");
  auto material_metal_white = createMaterial<Metal>(texture_metal_white_color, 1.f, texture_metal_white_roughness);

  auto texture_metal_golden_color = createTexture2D(resources_path / "Metal_golden/Metal048A_1K-PNG_Color.png");
  auto texture_metal_golden_roughness = createTexture2D(resources_path / "Metal_golden/Metal048A_1K-PNG_Roughness.png");
  auto material_metal_golden = createMaterial<Metal>(texture_metal_golden_color, 1.f, texture_metal_golden_roughness);


  // World
  auto ground_object = createObject<Plane>(glm::vec3(0.f, -0.5f, 0.f),  // position
                                           glm::vec3(0.f, 1.f, 0.f),    // normal
                                           material_matte_brown);
  auto sphere_left = createObject<Sphere>(glm::vec3(-1.5f, 0.0f, -0.0f),// position
                                          0.5f,                         // radius
                                          material_metal_golden);
  auto sphere_center = createObject<Sphere>(glm::vec3(0.0f, 0.0f, 0.f),   // position
                                            0.5f,                         // radius
                                            material_plastic);
  auto sphere_right = createObject<Sphere>(glm::vec3(1.5f, 0.0f, -0.0f), // position
                                           0.5f,                         // radius
                                           material_metal_white);

  Scene scene;
  scene.add(ground_object);
  scene.add(sphere_center);
  scene.add(sphere_left);
  scene.add(sphere_right);
  
  // Render
  camera.captureImage(scene);
  camera.applyGammaCorrection(2.2f);
  auto data = camera.getImageData();
  ImageLoader::writePNG("image.png", image_resolution, data);

  return 0;
}