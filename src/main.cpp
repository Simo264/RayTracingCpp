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
  constexpr auto imageResolution = glm::ivec2(400, 225);
  Camera camera(imageResolution);

  // World
  Scene scene;
  scene.addObject(std::make_shared<Sphere>(glm::vec3(0.f, 0.f, -1.f), 0.5f));
  scene.addObject(std::make_shared<Plane>(glm::vec3(0.f, -0.5f, 0.f)));

  // Render
  camera.render(scene);
  auto imageOutput = camera.getImageOutput();
  auto imageSize = camera.getImageSize();
  
  stbi_write_png("image.png", imageSize.x, imageSize.y, 3, imageOutput.get(), imageSize.x * 3);
  return 0;
}