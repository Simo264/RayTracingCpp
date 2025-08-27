#include "Camera.hpp"
#include "Ray.hpp"
#include "Scene.hpp"

#include "Geometry/IHittableObject.hpp"

#include <iostream>
#include <cassert> 
#include <vector>
#include <thread>
#include <chrono>

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
	const auto num_threads = std::thread::hardware_concurrency();
	std::cout << "Begin execution with " << num_threads << " threads\n";
	std::cout << "Image resolution: " << image_resolution.x << "x" << image_resolution.y << "\n";
	std::cout << "Total number of pixel to process: " << image_resolution.x * image_resolution.y << "\n";

	auto render_chunk = [&](uint32_t start_y, uint32_t end_y) -> void {
		for (auto y = start_y; y < end_y; ++y)
		{
			for (auto x = 0u; x < image_resolution.x; ++x)
			{
				auto pixel_color = glm::vec3(0.f);
				for (auto sample = 0u; sample < samples_per_pixel; sample++)
				{
					auto offset = glm::linearRand(glm::vec2(-0.5f), glm::vec2(0.5f));
					auto ray = __generateRay(x, y, offset);
					pixel_color += __renderer.computeRayColor(ray, scene, 10);
				}
				pixel_color /= static_cast<float>(samples_per_pixel);

				// Conversione e scrittura dei dati.
				static auto to_byte = [](float c) -> std::byte {
					return static_cast<std::byte>(glm::clamp(c * 255.999f, 0.0f, 255.0f));
				};
				auto r = to_byte(pixel_color.r);
				auto g = to_byte(pixel_color.g);
				auto b = to_byte(pixel_color.b);
				auto index = (y * image_resolution.x + x) * 3;
				__image_data[index + 0] = r;
				__image_data[index + 1] = g;
				__image_data[index + 2] = b;
			}
		}
	};

	auto rows_per_thread = image_resolution.y / num_threads;
	auto start_time = std::chrono::steady_clock::now();
	{
		std::vector<std::jthread> threads;
		threads.reserve(num_threads);
		for (auto i = 0u; i < num_threads; ++i)
		{
			auto start_y = i * rows_per_thread;
			auto end_y = (i == num_threads - 1) ? image_resolution.y : (i + 1) * rows_per_thread;
			threads.emplace_back(render_chunk, start_y, end_y);
		}
	}
	const auto end_time = std::chrono::steady_clock::now();
	const auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
	std::cout << "Render complete. Elapsed time: " << duration.count() << " ms" << std::endl;
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

