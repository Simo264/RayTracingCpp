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

Camera::Camera(glm::ivec2 imageResolution,
							 float focalLength,
							 float viewportH,
							 glm::vec3 position) :
	_imageSize{ imageResolution },
	position{ position }
{
	constexpr int nrChannels = 3;// default 3 channels: rgb
	_imageOutput = std::make_unique<std::byte[]>(_imageSize.x * _imageSize.y * nrChannels);

	// Calculate the vectors across the horizontal and down the vertical viewport edges.
	float aspect = static_cast<float>(_imageSize.x) / _imageSize.y;
	float viewportW = viewportH * (aspect);
	glm::vec3 viewportU(viewportW, 0, 0);
	glm::vec3 viewportV(0, -viewportH, 0);
	// Calculate the horizontal and vertical delta vectors from pixel to pixel.
	_pixelDeltaU = viewportU / static_cast<float>(_imageSize.x);
	_pixelDeltaV = viewportV / static_cast<float>(_imageSize.y);

	// Calculate the location of the upper left pixel.
	glm::vec3 viewportUpperLeft = position -
		glm::vec3(0, 0, focalLength) -
		viewportU / 2.f -
		viewportV / 2.f;
	_pixel00Loc = viewportUpperLeft + (_pixelDeltaU + _pixelDeltaV) * 0.5f;
}


void Camera::render(const Scene& scene) const
{
	for (size_t y = 0; y < _imageSize.y; y++)
	{
		std::clog << "\rScanlines remaining: " << (_imageSize.y - y) << ' ' << std::flush;
		for (size_t x = 0; x < _imageSize.x; x++)
		{
			glm::vec3 pixelCenter = _pixel00Loc +
				(_pixelDeltaU * static_cast<float>(x)) +
				(_pixelDeltaV * static_cast<float>(y));
			glm::vec3 rayDir = glm::normalize(pixelCenter - position);
			
			Ray ray(position, rayDir);
			glm::vec3 pixelColorFP = __getRayColor(ray, scene); // in range [0-1]
			std::byte r = static_cast<std::byte>(glm::mix(0.f, 255.f, pixelColorFP.r)); // from [0-1] to [0-255]
			std::byte g = static_cast<std::byte>(glm::mix(0.f, 255.f, pixelColorFP.g)); // from [0-1] to [0-255]
			std::byte b = static_cast<std::byte>(glm::mix(0.f, 255.f, pixelColorFP.b)); // from [0-1] to [0-255]

			size_t index = (y * _imageSize.x + x) * 3;
			_imageOutput[index + 0] = r;
			_imageOutput[index + 1] = g;
			_imageOutput[index + 2] = b;
		}
	}
	std::cout << "\nDone\n";
}


/**
 * ============================================
 *		PRIVATE
 * ============================================
 */

glm::vec3 Camera::__getRayColor(const Ray& ray, const Scene& scene) const
{
	constexpr auto infinity = std::numeric_limits<float>::infinity();

	HitRecord record{};
	if (scene.hitAnything(ray, Interval(0.f, infinity), record))
	{
		return 0.5f * (record.normal + glm::vec3(1.f));
	}

	auto unit_direction = glm::normalize(ray.direction());
	auto a = (unit_direction.y + 1.0f) * 0.5f;
	return glm::mix(glm::vec3(1.f), glm::vec3(0.5f, 0.7f, 1.0f), a); // linear interpolation between blue and white
}
