#include <iostream>
#include <format>
#include <optional>
#include <limits>

#include <glm/glm.hpp>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#include "Ray.hpp"
#include "Sphere.hpp"
#include "Scene.hpp"

static constexpr float infinity = std::numeric_limits<float>::infinity();

static glm::vec3 ray_color(const Ray& ray, const Scene& scene)
{
  HitRecord record{};
  if(scene.hitAnything(ray, 0.f, infinity, record))
  {
    return 0.5f * (record.normal + glm::vec3(1.f));
  }

  glm::vec3 unit_direction = glm::normalize(ray.direction());
  float a = (unit_direction.y + 1.0) * 0.5f;
  return glm::mix(glm::vec3(1.f), glm::vec3(0.5f, 0.7f, 1.0f), a); // linear interpolation between blue and white
}


int main()
{
  // Image
  constexpr float ASPECT = 16.f / 9.f;
  constexpr int IMAGE_WIDTH = 400; // 400px
  constexpr int IMAGE_HEIGHT = glm::max(IMAGE_WIDTH/ASPECT, 1.f) ; // 225px and ensure that it's at least 1.
  constexpr glm::vec2 image_size(IMAGE_WIDTH, IMAGE_HEIGHT);

  // Camera
  constexpr float focal_length = 1.0f;
  constexpr float viewport_height = 2.f;
  constexpr float viewport_width = viewport_height * (float(image_size.x) / image_size.y);
  glm::vec3 camera_center(0.f);

  // Calculate the vectors across the horizontal and down the vertical viewport edges.
  glm::vec3 viewport_u(viewport_width, 0, 0);
  glm::vec3 viewport_v(0, -viewport_height, 0);
  // Calculate the horizontal and vertical delta vectors from pixel to pixel.
  glm::vec3 pixel_delta_u = viewport_u / image_size.x;
  glm::vec3 pixel_delta_v = viewport_v / image_size.y;
  // Calculate the location of the upper left pixel.
  glm::vec3 viewport_upper_left = camera_center - 
    glm::vec3(0, 0, focal_length) - 
    viewport_u / 2.f -
    viewport_v / 2.f;
  glm::vec3 pixel00_loc = viewport_upper_left + (pixel_delta_u + pixel_delta_v) * 0.5f;


  // World
  Scene scene;
  scene.addObject(std::make_shared<Sphere>(glm::vec3(0.f, 0.f, -1.f), 0.5f));
  scene.addObject(std::make_shared<Sphere>(glm::vec3(0.f, -100.5f, -1.f), 100.f));


  // Render
  auto image = std::make_unique<uint8_t[]>(image_size.x * image_size.y * 3);
  for (size_t y = 0; y < image_size.y; y++)
  {
    std::clog << "\rScanlines remaining: " << (image_size.y - y) << ' ' << std::flush;
    for (size_t x = 0; x < image_size.x; x++)
    {
      glm::vec3 pixel_center = pixel00_loc +
        (pixel_delta_u * static_cast<float>(x)) + 
        (pixel_delta_v * static_cast<float>(y));
      
      glm::vec3 ray_direction = pixel_center - camera_center;
      Ray ray(camera_center, ray_direction);

      glm::vec3 pixel_color = ray_color(ray, scene);
      pixel_color.r = static_cast<int>(glm::mix(0.f, 255.f, pixel_color.r)); // from [0-1] to [0-255]
      pixel_color.g = static_cast<int>(glm::mix(0.f, 255.f, pixel_color.g)); // from [0-1] to [0-255]
      pixel_color.b = static_cast<int>(glm::mix(0.f, 255.f, pixel_color.b)); // from [0-1] to [0-255]

      size_t index = (y * image_size.x + x) * 3;
      image[index + 0] = pixel_color.r; 
      image[index + 1] = pixel_color.g; 
      image[index + 2] = pixel_color.b; 
    }
  }

  std::clog << "\rDone.                 \n";
  stbi_write_png("image.png", image_size.x, image_size.y, 3, image.get(), image_size.x * 3);
  return 0;
}