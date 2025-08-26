#pragma once

#include <limits>
#include <glm/glm.hpp>

/**
 * @brief 
 * The basic geometric operation required by all realistic renderers is to determine the first scene point along a ray. 
 * A ray, is the set of points p(t) along a half-open line that start at the ray origin r0 and goes in the ray direction d.
 * The parameter t is the distance along the ray from the origin, and is defined over the positive real numbers. 
 * For practical purposes, we limit the distance to a finite interval t:[t_min, t_max].
 * The minimum distance t_min is set to a small number to avoid numerical problems when considering too-close surfaces. 
 * The maximum distance t_max is set to the largest floating point value, to encompass the whole scene, 
 * but can be shorter to ignore some objects when necessary. 
 * 
 * We can write points on a ray as: p(t) = r0 + t*d.
 * 
 * The first scene point along a ray corresponds to the intersection of the ray and scene elements that is closest 
 * to the ray origin. Since we parameterize the ray by distance, we can find the closest point by finding 
 * the minimum distance along the ray for which an intersection occurs. 
 * This operation is called "ray casting", and we write it as a function that takes the ray and the scene, 
 * and returns the distance along the ray and the intersection point, if an intersection occurs.
 *  
 * we can write the ray casting function as: intersect_scene(r,S) -> set{}
 */
class Ray
{
public:
	Ray(glm::vec3 origin = glm::vec3(0.f),
			glm::vec3 direction = glm::vec3(0.0f, 0.0f, 1.0f)
	) : 
		origin{ origin }, 
		direction{ glm::normalize(direction) }
	{}
	~Ray() = default;
	
	auto at(float t) const { return origin + t * direction; }
	
	glm::vec3 origin;			// Ray origin (r0)
	glm::vec3 direction;	// Ray direction (d), normalized
};

