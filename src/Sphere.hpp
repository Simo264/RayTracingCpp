#pragma once

#include "HittableObject.hpp"

class Sphere : public HittableObject
{
public:
	Sphere(glm::vec3 center, float r) :
		_center{ center }, _radius{ std::fmax(0.f, r) }
	{}

	bool hit(const Ray& ray,
					 const Interval& interval,
					 HitRecord& rec) const;

	auto center() const { return _center; }
	auto radius() const { return _radius; }

private:
	glm::vec3 _center;
	float _radius;
};

