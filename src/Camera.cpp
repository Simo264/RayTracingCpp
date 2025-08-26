#include "Camera.hpp"
#include "Ray.hpp"
#include "Scene.hpp"

#include "Geometry/IHittableObject.hpp"

#include <iostream>
#include <cassert> 
#include <glm/gtc/random.hpp>


/** 
 * ============================================
 *		PUBLIC
 * ============================================
 */

Camera::Camera(const glm::vec3& position,
							 const glm::vec3& look_at,
							 const glm::uvec2& image_resolution,
							 float focal_length,
							 const glm::vec2& sensor_size
) :
	position{ position },
	image_resolution{ image_resolution },
	sensor_size{ sensor_size },
	focal_length{ focal_length },
	samples_per_pixel{ 128u },
	__renderer{},
	__forward{},
	__right{},
	__up{},
	__top_left_corner{},
	__sensor_width_vector{},
	__sensor_height_vector{}
{
	assert(image_resolution.x > 0 && image_resolution.y > 0);

	__image_data = std::make_shared<std::byte[]>(image_resolution.x * image_resolution.y * 3);
	__computeCameraFrame(look_at);
	__computeImagingSurface();
}

void Camera::captureImage(const Scene& scene) const
{
	for (auto y = 0u; y < image_resolution.y; y++)
	{
		for (auto x = 0u; x < image_resolution.x; x++)
		{
			std::clog << "\rScanlines remaining: " << (image_resolution.y - y) << ' ' << std::flush;

			auto pixel_color = glm::vec3(0.f);
			for (auto sample = 0u; sample < samples_per_pixel; sample++)
			{
				auto offset = glm::linearRand(glm::vec2(-0.5f), glm::vec2(0.5f));
				auto ray = __generateRay(x, y, offset);
				pixel_color += __renderer.computeRayColor(ray, scene, 10);
			}
			pixel_color /= static_cast<float>(samples_per_pixel);

			static auto to_byte = [](float c) -> std::byte {
				return static_cast<std::byte>(glm::clamp(c * 255.999f, 0.0f, 255.0f)); // from [0-1] to [0-255]
			};
			auto r = to_byte(pixel_color.r); // from [0-1] to [0-255]
			auto g = to_byte(pixel_color.g); // from [0-1] to [0-255]
			auto b = to_byte(pixel_color.b); // from [0-1] to [0-255]
			auto index = (y * image_resolution.x + x) * 3;
			__image_data[index + 0] = r;
			__image_data[index + 1] = g;
			__image_data[index + 2] = b;
		}
	}
}

void Camera::applyGammaCorrection(float gamma) const
{
	if (gamma == 0.f)
		return;

	for (auto y = 0u; y < image_resolution.y; y++)
	{
		for (auto x = 0u; x < image_resolution.x; x++)
		{
			auto index = (y * image_resolution.x + x) * 3;

			// Convert bytes to normalized [0,1] floats
			auto r = static_cast<float>(__image_data[index + 0]) / 255.0f;
			auto g = static_cast<float>(__image_data[index + 1]) / 255.0f;
			auto b = static_cast<float>(__image_data[index + 2]) / 255.0f;

			// Apply gamma correction
			r = glm::pow(r, 1.0f / gamma);
			g = glm::pow(g, 1.0f / gamma);
			b = glm::pow(b, 1.0f / gamma);

			// Convert back to bytes [0-255]
			__image_data[index + 0] = static_cast<std::byte>(glm::clamp(r * 255.999f, 0.0f, 255.0f));
			__image_data[index + 1] = static_cast<std::byte>(glm::clamp(g * 255.999f, 0.0f, 255.0f));
			__image_data[index + 2] = static_cast<std::byte>(glm::clamp(b * 255.999f, 0.0f, 255.0f));
		}
	}
}

/**
 * ============================================
 *		PRIVATE
 * ============================================
 */

void Camera::__computeCameraFrame(const glm::vec3& target)
{
	__forward = glm::normalize(target - position);
	__right = glm::normalize(glm::cross(__forward, glm::vec3(0.f, 1.f, 0.f)));
	__up = glm::cross(__right, __forward);
}

void Camera::__computeImagingSurface()
{
	auto image_center = position + __forward * focal_length;
	__sensor_width_vector = __right * sensor_size.x;
	__sensor_height_vector = __up * sensor_size.y;
	__top_left_corner = image_center - (__sensor_width_vector * 0.5f) + (__sensor_height_vector * 0.5f);
}

Ray Camera::__generateRay(int x, int y, glm::vec2& offset) const
{
	float u = (static_cast<float>(x) + 0.5f + offset.x) / image_resolution.x;
	float v = (static_cast<float>(y) + 0.5f + offset.y) / image_resolution.y;

	auto image_point = __top_left_corner
		+ u * __sensor_width_vector
		- v * __sensor_height_vector;

	auto ray_dir = glm::normalize(image_point - position);
	return Ray(position, ray_dir);
}

