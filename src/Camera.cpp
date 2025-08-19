#include "Camera.hpp"
#include "Ray.hpp"
#include "Random.hpp"
#include "Interval.hpp"
#include "Scene/Scene.hpp"

#include "Scene/IHittableObject.hpp"
#include "Material/IMaterial.hpp"

#include <iostream>

/** 
 * ============================================
 *		PUBLIC
 * ============================================
 */

Camera::Camera(const glm::vec3& position,
							 const glm::vec3& look_at,
							 const glm::uvec2& image_resolution,
							 const glm::vec2& sensor_size,
							 float focal_length,
							 float aperture,
							 float focus_distance
) :
	position{ position },
	image_resolution{ image_resolution },
	sensor_size{ sensor_size },
	focal_length{ focal_length },
	aperture{ aperture },
	focus_distance{ focus_distance },
	samples_per_pixel{ 8u },
	__forward{},
	__right{},
	__up{},
	__lower_left_corner{},
	__horizontal{},
	__vertical{}
{
	if (image_resolution.x == 0 || image_resolution.y == 0)
		std::cerr << "Invalid argument: image_resolution\n";

	image_data = std::make_shared<std::byte[]>(image_resolution.x * image_resolution.y * 3);
	__computeCameraFrame(look_at);
	__computeImagingSurface();
}

void Camera::captureImage(const Scene& scene) const
{
	for (int y = image_resolution.y - 1; y >= 0; --y)
	{
		for (int x = 0; x < image_resolution.x; x++)
		{
			std::clog << "\rScanlines remaining: " << (image_resolution.y - y) << ' ' << std::flush;

			auto pixel_color = glm::vec3(0.f);
			for (auto sample = 0u; sample < samples_per_pixel; sample++)
			{
				auto offset = Random::generateRandomVector2(Interval(-0.5f, 0.5f));
				auto ray = __generateRay(x, y, offset);
				pixel_color += __computeRayColor(ray, scene, 5);
			}
			pixel_color /= static_cast<float>(samples_per_pixel);

			static auto to_byte = [](float c) -> std::byte {
				return static_cast<std::byte>(glm::clamp(c * 255.999f, 0.0f, 255.0f)); // from [0-1] to [0-255]
			};
			auto r = to_byte(pixel_color.r); // from [0-1] to [0-255]
			auto g = to_byte(pixel_color.g); // from [0-1] to [0-255]
			auto b = to_byte(pixel_color.b); // from [0-1] to [0-255]
			auto flipped_y = image_resolution.y - 1 - y;
			auto index = (flipped_y * image_resolution.x + x) * 3;
			image_data[index + 0] = r;
			image_data[index + 1] = g;
			image_data[index + 2] = b;
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
	auto film_width = sensor_size.x;
	auto film_height = sensor_size.y;
	auto image_center = position + __forward * focal_length;

	__horizontal = __right * film_width;
	__vertical = __up * film_height;
	__lower_left_corner = image_center - (__horizontal * 0.5f) - (__vertical * 0.5f);
}

Ray Camera::__generateRay(int x, int y, glm::vec2& offset) const
{
	// Convert pixel coordinates + offset to normalized screen space [0,1]
	auto u = (static_cast<float>(x) + 0.5f + offset.x) / image_resolution.x;
	auto v = (static_cast<float>(y) + 0.5f + offset.y) / image_resolution.y;
	// Compute the point on the image plane
	auto image_point = __lower_left_corner + u * __horizontal + v * __vertical;
	// Direction from camera position to image point
	auto ray_dir = glm::normalize(image_point - position);
	return Ray(position, ray_dir);
}

glm::vec3 Camera::__computeRayColor(const Ray& ray,
																		const Scene& scene,
																		uint32_t depth) const
{
	if (depth == 0) // If we've exceeded the ray bounce limit, no more light is gathered.
		return glm::vec3(0.f);

	constexpr auto infinity = std::numeric_limits<float>::infinity();

	auto record = HitRecord{};
	if (scene.hitAnything(ray, Interval(0.001f, infinity), record))
	{
		if (record.material == nullptr)
			return glm::vec3(0.f);

		auto attenuation = glm::vec3();
		auto scattered_ray = Ray();
		auto is_scattered = record.material->scatter(ray, record, scattered_ray, attenuation);
		if (!is_scattered)
			return attenuation;

		return attenuation * __computeRayColor(scattered_ray, scene, depth - 1);
	}

	auto unit_direction = glm::normalize(ray.direction());
	auto a = (unit_direction.y + 1.0f) * 0.5f;
	return glm::mix(glm::vec3(1.f), glm::vec3(0.5f, 0.7f, 1.0f), a); // linear interpolation between blue and white
}
