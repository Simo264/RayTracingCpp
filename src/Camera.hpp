#pragma once

#include <memory>
#include <glm/glm.hpp>

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
	Camera(glm::ivec2 image_resolution,
				 float focal_length = 1.0f,							// default focal length
				 float viewport_h = 2.f,								// default viewport height
				 glm::vec3 position = glm::vec3( 0.f )	// default position
	);
	~Camera() = default;
	
	auto getImageOutput() const { return __image_output; }
	auto getImageSize() const { return __image_size; }
	
	void render(const Scene& scene) const;
	
	glm::vec3 position;					// Camera position
	uint32_t max_depth;					// Maximum number of ray bounces into scene
	
private:
	glm::vec2 __sample_square() const;
	Ray __getRay(size_t x, size_t y, glm::vec2 offset) const;
	glm::vec3 __computeRayColor(const Ray& ray, 
															const Scene& scene,
															int depth) const;
	
	uint32_t __samples_per_pixel;	// Count of random samples for each pixel

	glm::ivec2 __image_size;
	glm::vec3 __pixel_delta_u;
	glm::vec3 __pixel_delta_v;
	glm::vec3 __pixel00_loc;
	std::shared_ptr<std::byte[]> __image_output;
};
