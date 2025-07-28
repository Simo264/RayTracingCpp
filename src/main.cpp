#include <iostream>
#include <format>

#include <glm/glm.hpp>
#include <glm/common.hpp>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

int main()
{
  constexpr size_t width = 256;
  constexpr size_t height = 256;
  std::unique_ptr<std::uint8_t[]> image = std::make_unique<std::uint8_t[]>(width * height * 3);

  for (int y = 0; y < height; y++)
  {
    std::clog << "\rScanlines remaining: " << (height - y) << ' ' << std::flush;
    for (int x = 0; x < width; x++)
    {
      float r = (static_cast<float>(x) / (width - 1));  // [0 - 1]
      float g = (static_cast<float>(y) / (height - 1)); // [0 - 1]
      float b = 0.f;

      glm::u8vec3 pixel{};
      pixel.r = static_cast<std::uint8_t>(glm::mix(0.f, 255.f, r)); // [0 - 255]
      pixel.g = static_cast<std::uint8_t>(glm::mix(0.f, 255.f, g)); // [0 - 255]
      pixel.b = static_cast<std::uint8_t>(b);

      size_t index = (y * width + x) * 3;
      image[index + 0] = pixel.r;
      image[index + 1] = pixel.g;
      image[index + 2] = pixel.b;
    }
  }

  std::clog << "\rDone.                 \n";

  stbi_write_png("image.png", width, height, 3, image.get(), width * 3);
  return 0;
}