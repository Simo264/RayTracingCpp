#pragma once

#include "IHittableObject.hpp"

class Sphere : public IHittableObject
{
public:
	Sphere(glm::vec3 center = glm::vec3(0.f),							// default center position 
				 float radius = 1.0f,														// default radius
				 std::shared_ptr<IMaterial> material = nullptr	// no material
	) :
		__center{ center }, __radius{ radius }, __material{ material } {}

	bool hit(const Ray& ray,
					 const Interval& interval,
					 HitRecord& rec) const;

	auto center() const { return __center; }
	auto radius() const { return __radius; }

private:
	glm::vec3 __center;
	float __radius;
	std::shared_ptr<IMaterial> __material;
};