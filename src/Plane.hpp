#pragma once

#include "HittableObject.hpp"

class Plane : public HittableObject
{
public:
	Plane(glm::vec3 center = glm::vec3(0.f),						// default center position
				glm::vec3 normal = glm::vec3(0.f, 1.f, 0.f))	// default normal vector
		: __center{ center }, __normal{ normal } {}
	~Plane() = default;

	bool hit(const Ray& ray,
					 const Interval& interval,
					 HitRecord& rec) const;

	auto center() const	{ return __center; }
	auto normal() const { return __normal; }

private:
	glm::vec3 __center;
	glm::vec3 __normal;
};
