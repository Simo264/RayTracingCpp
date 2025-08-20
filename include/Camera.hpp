#pragma once

#include <glm/glm.hpp>

#include "Image.hpp"
#include "Renderer/Renderer.hpp"

class Scene;
class Ray;

class Camera
{
public:
	Camera(
		const glm::vec3& position,
		const glm::vec3& look_at,
		const glm::uvec2& image_resolution = { 640, 480 },	// default image resolution: 640x480 (with 4:3 aspect)
		const glm::vec2& sensor_size = { 36.f, 24.f },			// default sensor size: 36mm x 24mm
		float focal_length = 18.f,													// default focal length: 18mm
		float aperture = 1.4,																// default lens aperture:1.4mm
		float focus_distance = 1000.f												// default focus distance at 1 meter: 1000mm
	);
	~Camera() = default;

	// Camera frame
	glm::vec3 position;						// lens center

	// Imaging surface
	glm::vec2 sensor_size;				// in mm
	glm::uvec2 image_resolution;	// in pixels
	uint32_t samples_per_pixel;

	// Lens parameters
	float focal_length;   // in mm
	float aperture;       // in mm
	float focus_distance; // in mm

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
