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
	Camera(glm::ivec2 imageResolution,
				 float focalLength = 1.0f,							// default focal length
				 float viewportH = 2.f,									// default viewport height
				 glm::vec3 position = glm::vec3( 0.f )	// default position
	);
	~Camera() = default;
	
	auto getImageOutput() const { return _imageOutput; }
	auto getImageSize() const { return _imageSize; }
	
	void render(const Scene& scene) const;
	
	glm::vec3 position;

private:
	glm::vec3 __getRayColor(const Ray& ray, 
													const Scene& scene) const;

	glm::ivec2 _imageSize;
	glm::vec3 _pixelDeltaU;
	glm::vec3 _pixelDeltaV;
	glm::vec3 _pixel00Loc;
	std::shared_ptr<std::byte[]> _imageOutput;
};
