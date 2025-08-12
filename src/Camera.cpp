#include "Camera.hpp"
#include "Scene.hpp"
#include "Ray.hpp"
#include "HittableObject.hpp"
#include "Interval.hpp"

#include <iostream>
#include <limits>

/** 
 * ============================================
 *		PUBLIC
 * ============================================
 */

Camera::Camera(glm::ivec2 image_resolution,
							 float focal_length,
							 float viewport_h,
							 glm::vec3 position)
{
	this->position = position;
	max_depth = 10;

	__image_size = image_resolution;
	__samples_per_pixel = 10;
	__image_output = std::make_unique<std::byte[]>(__image_size.x * __image_size.y * 3);

	// Calculate the vectors across the horizontal and down the vertical viewport edges.
	auto aspect = static_cast<float>(__image_size.x) / __image_size.y;
	auto viewport_w = viewport_h * (aspect);
	glm::vec3 viewport_u(viewport_w, 0, 0);
	glm::vec3 viewport_v(0, -viewport_h, 0);
	// Calculate the horizontal and vertical delta vectors from pixel to pixel.
	__pixel_delta_u = viewport_u / static_cast<float>(__image_size.x);
	__pixel_delta_v = viewport_v / static_cast<float>(__image_size.y);

	// Calculate the location of the upper left pixel.
	glm::vec3 viewportUpperLeft = position -
		glm::vec3(0, 0, focal_length) -
		viewport_u / 2.f -
		viewport_v / 2.f;
	__pixel00_loc = viewportUpperLeft + (__pixel_delta_u + __pixel_delta_v) * 0.5f;
}


void Camera::render(const Scene& scene) const
{
	for (size_t y = 0; y < __image_size.y; y++)
	{
		std::clog << "\rScanlines remaining: " << (__image_size.y - y) << ' ' << std::flush;
		for (size_t x = 0; x < __image_size.x; x++)
		{
			glm::vec3 pixel_color(0.f);
			for (int sample = 0; sample < __samples_per_pixel; sample++)
			{
				auto offset = __sample_square();
				auto ray = __getRay(x, y, offset);
				pixel_color += __computeRayColor(ray, scene, max_depth); // in range [0-1]
			}

			pixel_color /= static_cast<float>(__samples_per_pixel); // average
			pixel_color = glm::sqrt(pixel_color); // Apply gamma correction(usually sqrt() for gamma = 2.0)
			auto to_byte = [](float c) -> std::byte {
				return static_cast<std::byte>(glm::clamp(c * 255.999f, 0.0f, 255.0f)); // from [0-1] to [0-255]
			};
			
			std::byte r = to_byte(pixel_color.r); // from [0-1] to [0-255]
			std::byte g = to_byte(pixel_color.g); // from [0-1] to [0-255]
			std::byte b = to_byte(pixel_color.b); // from [0-1] to [0-255]
			
			size_t index = (y * __image_size.x + x) * 3;
			__image_output[index + 0] = r;
			__image_output[index + 1] = g;
			__image_output[index + 2] = b;
		}
	}
	std::cout << "\nDone\n";
}


/**
 * ============================================
 *		PRIVATE
 * ============================================
 */

#include <random>
#include <glm/gtx/norm.hpp> // For glm::length2

static glm::vec3 __random_unit_vector()
{
	static std::mt19937 gen(std::random_device{}());
	static std::uniform_real_distribution<float> dist(-1.0f, 1.0f);

	while (true)
	{
		glm::vec3 v(dist(gen), dist(gen), dist(gen));
		auto lensq = glm::length2(v);	// More efficient than glm::length(p) * glm::length(p)
		if (lensq <= 1.0f && lensq > 1e-6f)
			return v / std::sqrt(lensq); // = glm::normalize(v)
	}

	return glm::vec3(0.f);
}

static glm::vec3 __random_on_hemisphere(const glm::vec3& n)
{
	glm::vec3 v = __random_unit_vector();
	if (glm::dot(v, n) > 0.f) // In the same hemisphere as the normal
		return v;
	return -v;
}


glm::vec2 Camera::__sample_square() const
{
	// Returns a random point in the unit square [-0.5, -0.5] to [0.5, 0.5]
	static std::uniform_real_distribution<double> distribution(0.0, 1.0);
	static std::mt19937 generator;
	auto random = distribution(generator);
	
	auto x = static_cast<float>(random) - 0.5f;
	auto y = static_cast<float>(random) - 0.5f;
	return glm::vec2(x, y);
}

Ray Camera::__getRay(size_t x, size_t y, glm::vec2 offset) const
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

glm::vec3 Camera::__computeRayColor(const Ray& ray, 
																		const Scene& scene, 
																		int depth) const
{
	if (depth <= 0) // If we've exceeded the ray bounce limit, no more light is gathered.
		return glm::vec3(0.f);

	constexpr auto infinity = std::numeric_limits<float>::infinity();

	HitRecord record{};
	if (scene.hitAnything(ray, Interval(0.001f, infinity), record))
	{
		auto new_random_direction = __random_on_hemisphere(record.normal);
		auto new_ray = Ray(record.p, new_random_direction);
		return 0.5f * __computeRayColor(new_ray, scene, depth-1);
	}

	auto unit_direction = glm::normalize(ray.direction());
	auto a = (unit_direction.y + 1.0f) * 0.5f;
	return glm::mix(glm::vec3(1.f), glm::vec3(0.5f, 0.7f, 1.0f), a); // linear interpolation between blue and white
}
