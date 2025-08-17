#include "Camera.hpp"
#include "Ray.hpp"
#include "Random.hpp"
#include "Interval.hpp"
#include "Scene/Scene.hpp"

#include <iostream>


/** 
 * ============================================
 *		PUBLIC
 * ============================================
 */

Camera::Camera(glm::vec3 position,
							 glm::uvec2 image_resolution,
							 float focal_length,
							 float viewport_h) :
	position{ position },
	samples_per_pixel{ 8 },
	image{ Image(image_resolution.x, image_resolution.y) },
	__max_depth{ 10 }
{
	__setupViewport(viewport_h, focal_length);
}

void Camera::captureImage(const Scene& scene)
{
	auto image_resolution = glm::uvec2(image.resolution_w, image.resolution_h);
	std::cout << "Number of total pixels: " << (image_resolution.x * image_resolution.y) << "\n";

	for (auto y = 0u; y < image_resolution.y; y++)
	{
		for (auto x = 0u; x < image_resolution.x; x++)
		{
			std::clog << "\rScanlines remaining: " << (image_resolution.y - y) << ' ' << std::flush;
			
			auto pixel_color = glm::vec3(0.f);
			for (auto sample = 0u; sample < samples_per_pixel; sample++)
			{
				auto offset = Random::generateVector2(Interval(-0.5f, 0.5f));
				auto ray = __getRay(x, y, offset);
				pixel_color += __renderer.computeRayColor(ray, scene, __max_depth); // in range [0-1]
			}
			
			pixel_color /= static_cast<float>(samples_per_pixel); // average
			
			auto to_byte = [](float c) -> std::byte {
				return static_cast<std::byte>(glm::clamp(c * 255.999f, 0.0f, 255.0f)); // from [0-1] to [0-255]
			};

			auto r = to_byte(pixel_color.r); // from [0-1] to [0-255]
			auto g = to_byte(pixel_color.g); // from [0-1] to [0-255]
			auto b = to_byte(pixel_color.b); // from [0-1] to [0-255]
			image.setPixel(x, y, r, g, b);
		}
	}
}

void Camera::applyGammaCorrection(float gamma) const
{
	if (gamma == 0.f)
		return;

	if (image.resolution_w == 0u || image.resolution_h == 0u)
		return;

	for (auto y = 0u; y < image.resolution_h; y++)
	{
		for (auto x = 0u; x < image.resolution_w; x++)
		{
			auto index = static_cast<uint64_t>(y * image.resolution_w + x) * image.nr_channels;
			for (auto c = 0u; c < image.nr_channels; c++) // Apply to R, G, B channels 
			{
				auto linear = static_cast<float>(image.data[index + c]) / 255.0f;
				auto corrected = std::pow(linear, 1.0f / gamma);
				image.data[index + c] = static_cast<std::byte>(glm::clamp(corrected * 255.0f, 0.0f, 255.0f));
			}
		}
	}
}

/**
 * ============================================
 *		PRIVATE
 * ============================================
 */

void Camera::__setupViewport(float viewport_h, float focal_length)
{
	auto image_resolution = glm::uvec2(image.resolution_w, image.resolution_h);

	// Calculate the vectors across the horizontal and down the vertical viewport edges.
	auto aspect = static_cast<float>(image_resolution.x) / image_resolution.y;
	auto viewport_w = viewport_h * (aspect);
	auto viewport_u = glm::vec3(viewport_w, 0, 0);
	auto viewport_v = glm::vec3(0, -viewport_h, 0);
	// Calculate the horizontal and vertical delta vectors from pixel to pixel.
	__pixel_delta_u = viewport_u / static_cast<float>(image_resolution.x);
	__pixel_delta_v = viewport_v / static_cast<float>(image_resolution.y);

	// Calculate the location of the upper left pixel.
	auto viewportUpperLeft = position -
		glm::vec3(0, 0, focal_length) -
		viewport_u / 2.f -
		viewport_v / 2.f;
	__pixel00_loc = viewportUpperLeft + (__pixel_delta_u + __pixel_delta_v) * 0.5f;
}

Ray Camera::__getRay(uint32_t x, uint32_t y, glm::vec2 offset) const
{
	// Construct a camera ray originating from the origin and directed at randomly sampled
	// point around the pixel location i, j.
	auto pixel_sample = __pixel00_loc
		+ ((x + offset.x) * __pixel_delta_u)
		+ ((y + offset.y) * __pixel_delta_v);

	auto ray_origin = position;
	auto ray_direction = glm::normalize(pixel_sample - ray_origin);
	return Ray(position, ray_direction);
}


