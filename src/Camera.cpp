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
							 glm::vec3 look_at,
							 float vfov, 
							 float defocus_angle,
							 float focus_dist) :
	position{ position },
	image{ Image(image_resolution.x, image_resolution.y) },
	samples_per_pixel{ 8 },
	__max_depth{ 10 },
	__vfov{ vfov },
	__defocus_angle{ defocus_angle },
	__focus_dist{ focus_dist }
{
	__setupViewport(look_at);
}

void Camera::captureImage(const Scene& scene)
{
	auto image_resolution = glm::uvec2(image.resolution_w, image.resolution_h);
	for (auto y = 0u; y < image_resolution.y; y++)
	{
		for (auto x = 0u; x < image_resolution.x; x++)
		{
			std::clog << "\rScanlines remaining: " << (image_resolution.y - y) << ' ' << std::flush;
			
			// This loop performs supersampling antialiasing by shooting multiple rays per pixel with slight random offsets.
			// This technique smooths edges and improves image quality by sampling multiple points within each pixel.
			auto pixel_color = glm::vec3(0.f); // accumulates the color contributions from each sample.
			for (auto sample = 0u; sample < samples_per_pixel; sample++)
			{
				auto offset = Random::generateRandomVector2(Interval(-0.5f, 0.5f));
				auto ray = __getRay(x, y, offset);	// Computes a ray from the camera through the pixel
																						// with the given subpixel offset.
				pixel_color += __renderer.computeRayColor(ray, scene, __max_depth); // traces the ray through the scene and returns  
																																						// its color contribution in range [0-1]
			}
			pixel_color /= static_cast<float>(samples_per_pixel); // After all samples are collected, 
																														// the average color is computed by dividing samples_per_pixel
			
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

void Camera::__setupViewport(glm::vec3 look_at)
{
	auto image_resolution = glm::uvec2(image.resolution_w, image.resolution_h);
	auto aspect = static_cast<float>(image_resolution.x) / image_resolution.y;

	// Calculates the physical size of the viewport based on the vertical field of view 
	// and distance to the target.
	auto theta = glm::radians(__vfov);
	auto h = glm::tan(theta / 2);
	auto viewport_h = 2 * h * __focus_dist;
	auto viewport_w = viewport_h * (aspect);
	
	// Constructs an orthonormal basis for the camera's local coordinate system.
	__w = glm::normalize(position - look_at);
	__u = glm::normalize(glm::cross(__up, __w));
	__v = glm::cross(__w, __u);

	// Calculate the vectors across the horizontal and down the vertical viewport edges.
	auto viewport_u = viewport_w * __u;			// Vector across viewport horizontal edge
	auto viewport_v = viewport_h * -(__v);  // Vector down viewport vertical edge
	
	// Defines how much to move in world space to go from one pixel to the next horizontally or vertically.
	__pixel_delta_u = viewport_u / static_cast<float>(image_resolution.x);
	__pixel_delta_v = viewport_v / static_cast<float>(image_resolution.y);
	
	// Calculates the center of the top-left pixel to begin ray generation.
	auto viewport_upper_left = position - 
		(__focus_dist * __w) - 
		viewport_u / 2.f - 
		viewport_v / 2.f;
	__pixel00_loc = viewport_upper_left + (__pixel_delta_u + __pixel_delta_v) * 0.5f;


	// Calculate the camera defocus disk basis vectors.
	auto defocus_radius = __focus_dist * glm::tan(glm::radians(__defocus_angle/2));
	__defocus_disk_u = __u * defocus_radius;
	__defocus_disk_v = __v * defocus_radius;
}

Ray Camera::__getRay(uint32_t x, uint32_t y, glm::vec2 offset) const
{
	// Computes the exact world - space location of the sample point within the pixel.
	auto pixel_sample = __pixel00_loc
		+ ((x + offset.x) * __pixel_delta_u)
		+ ((y + offset.y) * __pixel_delta_v);

	
	auto ray_origin = position;
	if (__defocus_angle > 0)
		ray_origin = __defocus_disk_sample();

	auto ray_direction = glm::normalize(pixel_sample - ray_origin);
	return Ray(ray_origin, ray_direction);
}

glm::vec3 Camera::__defocus_disk_sample() const
{
	// Returns a random point in the camera defocus disk.
	auto p = Random::generateRandomUnitDiskPoint();
	return position + (p.x * __defocus_disk_u) + (p.y * __defocus_disk_v);
}
