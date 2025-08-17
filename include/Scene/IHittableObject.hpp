#pragma once

#include <memory>
#include <glm/glm.hpp>

class Ray;
class Interval;
class IMaterial;

struct HitRecord
{
public:
	HitRecord() : 
		p{ glm::vec3(0.f) }, 
		normal{ glm::vec3(0.f) }, 
		t{ 0.0f }, 
		is_ray_outside{true}, 
		material{nullptr}
	{}

	glm::vec3 p;
	glm::vec3 normal;
	float t;
	bool is_ray_outside;
	std::shared_ptr<IMaterial> material;
};


/**
 * @brief This abstract class will have a hit function that takes in a ray.
 * Most ray tracers have found it convenient to add a valid interval for hits tmin to tmax,
 * so the hit only "counts" if tmin < t < tmax.
 * For the initial rays this is positive t, but as we will see, it can simplify our code to have an interval tmin to tmax.
 * One design question is whether to do things like compute the normal if we hit something.
 * We might end up hitting something closer as we do our search, and we will only need the normal of the closest thing.
 * I will go with the simple solution and compute a bundle of stuff I will store in some structure.
 */
class IHittableObject
{
public:
	IHittableObject() = default;
	~IHittableObject() = default;

	virtual bool hit(const Ray& ray, 
									 const Interval& interval,
									 HitRecord& record) const = 0;
};

