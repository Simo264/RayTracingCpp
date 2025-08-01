#pragma once

#include "HittableObject.hpp"

class Sphere : public HittableObject
{
public:
	Sphere(glm::vec3 center, float r) :
		_center{ center }, _radius{ std::fmax(0.f, r) }
	{}

	bool hit(const Ray& ray,
					 float rayTmin,
					 float rayTmax,
					 HitRecord& rec) const;

	auto& center() const { return _center; }
	float radius() const { return _radius; }

private:
	glm::vec3 _center;
	float _radius;
};

