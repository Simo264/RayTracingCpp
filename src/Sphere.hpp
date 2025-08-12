#pragma once

#include "HittableObject.hpp"

class Sphere : public HittableObject
{
public:
	Sphere(glm::vec3 center, float r) :
		__center{ center }, __radius{ std::fmax(0.f, r) }
	{}

	bool hit(const Ray& ray,
					 const Interval& interval,
					 HitRecord& rec) const;

	auto center() const { return __center; }
	auto radius() const { return __radius; }

private:
	glm::vec3 __center;
	float __radius;
};

