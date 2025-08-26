#pragma once

#include "IHittableObject.hpp"

class Plane : public IHittableObject
{
public:
	Plane(glm::vec3 center = glm::vec3(0.f),						// default center position
				glm::vec3 normal = glm::vec3(0.f, 1.f, 0.f),  // default normal vector
				std::shared_ptr<IMaterial> material = nullptr	// default material
	)	: 
		center{ center }, 
		normal{ normal },
		material{ material }
	{}
	~Plane() = default;

	bool intersect(const Ray& ray,
								 float t_min,
								 float t_max,
								 HitRecord& hit) const;

	glm::vec3 center;
	glm::vec3 normal;
	std::shared_ptr<IMaterial> material;
};

