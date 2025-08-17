#pragma once

#include <glm/glm.hpp>
#include "Image.hpp"
#include "Renderer/Renderer.hpp"

class Scene;
class Ray;

/**
 * @brief The camera class will be responsible for two important jobs:
 *	1. Construct and dispatch rays into the world
 *	2. Use the results of these rays to construct the rendered image.
 */
class Camera
{
public:
	Camera(glm::vec3 position, 
				 glm::uvec2 image_resolution,
				 float focal_length = 1.0f,							// default focal length
				 float viewport_h = 2.f									// default viewport height
	);
	~Camera() = default;

	void captureImage(const Scene& scene);
	void applyGammaCorrection(float gamma) const;
	
	glm::vec3 position;					// Camera position
	uint32_t samples_per_pixel;	// Count of random samples for each pixel
	Image image;

private:
	void __setupViewport(float viewport_h, float focal_length);
	Ray __getRay(uint32_t x, uint32_t y, glm::vec2 offset) const;

	Renderer __renderer;
	uint32_t __max_depth;					// Maximum number of ray bounces into scene
	glm::vec3 __pixel_delta_u;
	glm::vec3 __pixel_delta_v;
	glm::vec3 __pixel00_loc;
};
