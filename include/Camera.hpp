#pragma once

#include <glm/glm.hpp>
#include <memory>

#include "Renderer.hpp"

class Scene;
class Ray;

class Camera
{
public:
	Camera(
		const glm::vec3& position,
		const glm::vec3& look_at,
		const glm::uvec2& image_resolution = glm::uvec2(640u, 480u),// default image resolution: 640x480 (with 4:3 aspect)
		float focal_length = 50.f,													// default focal length: 50mm
		const glm::vec2& sensor_size = { 36.f, 27.f }				// default sensor size: 36mm x 27mm
	);
	~Camera() = default;

	// Camera frame
	glm::vec3 position;						// lens center

	// Imaging surface
	glm::vec2 sensor_size;				// in mm
	glm::uvec2 image_resolution;	// in pixels
	uint32_t samples_per_pixel;
	float focal_length;						// in mm

	void captureImage(const Scene& scene) const;
	void applyGammaCorrection(float gamma) const;
	auto getImageData() const { return __image_data.get(); }

private:
	// Setup camera frame and imaging surface
	void __computeCameraFrame(const glm::vec3& target); // build an orthonormal basis
	void __computeImagingSurface();											// set up the imaging plane in world space
	Ray __generateRay(int x, int y, glm::vec2& offset) const;

	Renderer __renderer;
	std::shared_ptr<std::byte[]> __image_data; // final image

	// Camera frame
	glm::vec3 __forward;    // -Z axis
	glm::vec3 __right;      // +X axis
	glm::vec3 __up;         // +Y axis

	// Precomputed values for ray generation
	glm::vec3 __top_left_corner;
	glm::vec3 __sensor_width_vector;
	glm::vec3 __sensor_height_vector;
};
