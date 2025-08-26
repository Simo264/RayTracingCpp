#pragma once

#include "IHittableObject.hpp"

class Sphere : public IHittableObject
{
public:
	Sphere(glm::vec3 center = glm::vec3(0.f),							// default center position 
				 float radius = 1.0f,														// default radius
				 std::shared_ptr<IMaterial> material = nullptr		// no material
	) :
		IHittableObject(),
		center{ center },
		radius{ radius },
		material{ material }
	{}

	bool intersect(const Ray& ray,
								 float t_min,
								 float t_max,
								 HitRecord& hit) const;

	glm::vec3 center;
	float radius;
	std::shared_ptr<IMaterial> material;
};