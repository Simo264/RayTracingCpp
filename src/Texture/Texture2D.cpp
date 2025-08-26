#include "Texture/Texture2D.hpp"

#include "ImageLoader.hpp"

#include <iostream>
#include <cassert> 

Texture2D::Texture2D(const glm::vec3& color)
{
	__texture_size = glm::uvec2(1);
	auto pixel_count = __texture_size.x * __texture_size.y;
	__pixels = std::make_shared<std::byte[]>(pixel_count * 3);
	__pixels[0] = static_cast<std::byte>(glm::clamp(color.r * 255.0f, 0.0f, 255.0f));
	__pixels[1] = static_cast<std::byte>(glm::clamp(color.g * 255.0f, 0.0f, 255.0f));
	__pixels[2] = static_cast<std::byte>(glm::clamp(color.b * 255.0f, 0.0f, 255.0f));
}

Texture2D::Texture2D(const path& file_path)
{
	assert(std::filesystem::exists(file_path));

	auto width = 0, height = 0, nr_channels = 0;
	auto data = ImageLoader::load(file_path, width, height, nr_channels);
	assert(data);
	
	__texture_size = glm::uvec2(width, height);
	auto pixel_count = width * height;
	__pixels = std::make_shared<std::byte[]>(pixel_count * 3);
	for (auto i = 0; i < pixel_count * nr_channels; ++i)
		__pixels[i] = static_cast<std::byte>(data[i]);

	ImageLoader::imageFree(data);
}

glm::vec3 Texture2D::sample(float u, float v) const
{
	// Wrap coordinates to [0,1]
	u = u - std::floor(u);
	v = v - std::floor(v);

	// Flip vertically (optional, depending on image origin)
	auto x = static_cast<uint32_t>(u * __texture_size.x);
	auto y = static_cast<uint32_t>((1.0f - v) * __texture_size.y);
	x = glm::clamp(x, 0u, __texture_size.x - 1);
	y = glm::clamp(y, 0u, __texture_size.y - 1);

	// Sample and convert to linear space
	auto pixel = getPixel(glm::uvec2(x, y));
	return toLinear(pixel);
}

glm::vec3 Texture2D::getPixel(glm::uvec2 position) const
{
	auto index = (position.y * __texture_size.x + position.x) * 3;
	auto r = static_cast<float>(__pixels[index + 0]) / 255.0f;
	auto g = static_cast<float>(__pixels[index + 1]) / 255.0f;
	auto b = static_cast<float>(__pixels[index + 2]) / 255.0f;
	return glm::vec3(r, g, b);
}

glm::vec3 Texture2D::toLinear(const glm::vec3& color) const
{
	auto convert = [](float c) -> float {
		return (c <= 0.04045f) ? (c / 12.92f) : std::pow((c + 0.055f) / 1.055f, 2.4f);
	};
	return glm::vec3(convert(color.r), convert(color.g), convert(color.b));
}
