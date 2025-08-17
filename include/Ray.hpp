#pragma once

#include <limits>
#include <glm/glm.hpp>

/**
 * @brief The one thing that all ray tracers have is a ray class and a computation of what color is seen along a ray. 
 * Let's think of a ray as a function p(t) = r0 + t*d.
 *  
 * Here P is a 3D position along a line in 3D. A is the ray origin and b is the ray direction. 
 * The ray parameter t is a real number (double in the code). 
 * Plug in a different t and P(t) moves the point along the ray. 
 * Add in negative t values and you can go anywhere on the 3D line. 
 * For positive t, you get only the parts in front of A, and this is what is often called a half-line or a ray.
 * 
 * We can represent the idea of a ray as a class, and represent the function P(t) 
 * as a function that we'll call ray::at(t)
 */
class Ray
{
public:
	Ray(glm::vec3 origin = glm::vec3(0.f),		// default ray origin
			glm::vec3 direction = glm::vec3(0.f)	// default ray direction
	) 
		: __origin{ origin }, __direction{ direction } {}

	~Ray() = default;
	
	/** @brief Computes the position along the ray at parameter t: p(t) = r0 + t*d */
	constexpr auto at(float t) const { return __origin + t * __direction; }
	auto origin() const { return __origin; }
	auto direction() const { return __direction; }

private:
	glm::vec3 __origin;
	glm::vec3 __direction;
};

