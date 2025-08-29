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
#include "Material/Emissive.hpp"

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
  constexpr auto camera_position = glm::vec3(0.f, 1.f, 5.f);
  constexpr auto camera_target = glm::vec3(0.f, 0.5f, 0.f);
  constexpr auto image_resolution = glm::uvec2(640u, 480u);
  constexpr float focal_length = 40.f;
  Camera camera(camera_position, camera_target, image_resolution, focal_length);
  camera.samples_per_pixel = 1024u;

  // Materials
  auto texture_color_brown = createTexture2D(glm::vec3(1.f, 0.87f, 0.67f));
  auto material_matte_brown = createMaterial<Matte>(texture_color_brown);
  auto texture_color_green = createTexture2D(resources_path / "Plastic_green/Plastic017A_1K-PNG_Color.png");
  auto material_matte_green = createMaterial<Matte>(texture_color_green);
  auto texture_color_orange = createTexture2D(resources_path / "Plastic_orange/Plastic014A_1K-PNG_Color.png");
  auto material_matte_orange = createMaterial<Matte>(texture_color_orange);
  auto texture_color_blue = createTexture2D(resources_path / "Plastic_blue/Plastic008_1K-PNG_Color.png");
  auto material_matte_blue = createMaterial<Matte>(texture_color_blue);
  auto texture_metal_color = createTexture2D(resources_path / "Metal_white/Metal049A_1K-PNG_Color.png");
  auto material_metal = createMaterial<Metal>(texture_metal_color, 0.0f, nullptr);

  auto material_emissive = createMaterial<Emissive>(glm::vec3(10.f));

  // World
  auto plane_object_bottom = createObject<Plane>(glm::vec3(0.f, -0.5f, 0.f),  // position
                                                 material_matte_brown,
                                                 glm::vec3(0.f, 1.f, 0.f),    // orientation
                                                 7.f, 7.f                     // width, height
  );
  auto sphere_object_1 = createObject<Sphere>(glm::vec3(0.0f, 0.0f, 0.f), // position
                                              material_metal,
                                              0.5f                        // radius
  );
  auto sphere_object_2 = createObject<Sphere>(glm::vec3(1.5f, 0.0f, 0.f), // position
                                              material_matte_orange,
                                              0.5f                        // radius
  );
  auto sphere_object_3 = createObject<Sphere>(glm::vec3(-1.5f, 0.0f, 0.f),  // position
                                              material_matte_blue,
                                              0.5f                          // radius
  );
  auto sphere_object_light_1 = createObject<Sphere>(glm::vec3(0.f, 1.f, 1.f),  // position
                                                    material_emissive,
                                                    0.25f                       // radius
  );

  Scene scene;
  scene.add(plane_object_bottom);
  scene.add(sphere_object_1);
  scene.add(sphere_object_2);
  scene.add(sphere_object_3);
  scene.add(sphere_object_light_1);
  
  // Render
  camera.captureImage(scene);
  camera.applyGammaCorrection(2.2f);
  auto data = camera.getImageData();
  ImageLoader::writePNG("image.png", image_resolution, data);

  return 0;
}