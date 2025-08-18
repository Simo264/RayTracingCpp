#pragma once

#include <glm/glm.hpp>
#include "Image.hpp"
#include "Renderer/Renderer.hpp"

class Scene;
class Ray;

/**
 * @brief Represents a virtual camera in 3D space for ray tracing.
 * The Camera class is responsible for generating rays through each pixel of an image,
 * based on a configurable position, orientation, and field of view. It supports antialiasing
 * via multiple samples per pixel and applies gamma correction to the final rendered image.
 * The camera uses a viewport defined by the image resolution and vertical field of view,
 * and constructs an orthonormal basis (u, v, w) to orient itself in the scene.
 *
 * @note The camera assumes a right-handed coordinate system and uses a fixed "up" direction.
 */
class Camera
{
public:
	Camera(glm::vec3 position, 
				 glm::uvec2 image_resolution,
				 glm::vec3 look_at,						// Point camera is looking at
				 float vfov = 90.f						// Default vertical field of view in degrees
	);
	~Camera() = default;

	/** @brief Renders the scene from the camera's perspective into the image buffer.	*/
	void captureImage(const Scene& scene);

	/** @brief Applies gamma correction (e.g., 2.2) to the rendered image. */
	void applyGammaCorrection(float gamma) const;
	
	glm::vec3 position;              // Camera position in world space.
	float vfov;                      // Vertical field of view in degrees.
	uint32_t samples_per_pixel;      // Number of samples per pixel for antialiasing.
	Image image;                     // Image buffer storing the rendered output.

private:
	// Initializes the camera's viewport and orientation based on the look-at point.
	void __setupViewport(glm::vec3 look_at);
	// Generates a ray through a specific pixel with subpixel offset.
	Ray __getRay(uint32_t x, uint32_t y, glm::vec2 offset) const;

	static constexpr inline auto __up = glm::vec3(0.f, 1.f, 0.f); // Fixed camera-relative "up" direction.

	glm::vec3 __u, __v, __w;          // Orthonormal basis vectors for camera orientation.
	glm::vec3 __pixel00_loc;          // World-space location of pixel (0,0).
	glm::vec3 __pixel_delta_u;        // Horizontal pixel spacing vector.
	glm::vec3 __pixel_delta_v;        // Vertical pixel spacing vector.
	Renderer __renderer;              // Internal renderer for ray-scene interaction.
	uint32_t __max_depth;             // Maximum recursion depth for ray bounces.
};
