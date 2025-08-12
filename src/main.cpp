#include <glm/glm.hpp>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#include "Scene.hpp"
#include "Camera.hpp"
#include "Sphere.hpp"
#include "Plane.hpp"


int main()
{
  // Camera
  constexpr auto image_resolution = glm::ivec2(1280, 720);
  Camera camera(image_resolution);

  // World
  Scene scene;
  scene.addObject(std::make_shared<Sphere>(glm::vec3(0.f, 0.f, -1.f), 0.5f));
  scene.addObject(std::make_shared<Plane>(glm::vec3(0.f, -0.5f, 0.f)));

  // Render
  camera.render(scene);
  auto image_output = camera.getImageOutput();
  auto image_size = camera.getImageSize();
  
  stbi_write_png("image.png", image_size.x, image_size.y, 3, image_output.get(), image_size.x * 3);
  return 0;
}