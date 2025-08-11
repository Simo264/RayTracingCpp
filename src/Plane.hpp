#pragma once

#include "HittableObject.hpp"

class Plane : public HittableObject
{
public:
	Plane(glm::vec3 center = glm::vec3(0.f),						// default center position
				glm::vec3 normal = glm::vec3(0.f, 1.f, 0.f))	// default normal vector
		: _center{ center }, _normal{ normal } {}
	~Plane() = default;

	bool hit(const Ray& ray,
					 const Interval& interval,
					 HitRecord& rec) const;

	auto center() const	{ return _center; }
	auto normal() const { return _normal; }

private:
	glm::vec3 _center;
	glm::vec3 _normal;
};
