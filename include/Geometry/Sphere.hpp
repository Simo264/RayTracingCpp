#pragma once

#include "IHittableObject.hpp"

class Sphere : public IHittableObject
{
public:
	Sphere(const glm::vec3& position, // the center of the sphere
				 const std::shared_ptr<IMaterial>& material,
				 float radius = 1.0f) :
		IHittableObject(position, material),
		__radius{ radius }
	{}
	~Sphere() = default;

	bool intersect(const Ray& ray,
								 float t_min,
								 float t_max,
								 HitRecord& hit) const override;
	
	/** @brief return the normal vector */
	glm::vec3 getNormal(const glm::vec3& p) const override;

	glm::vec2 getTextureCoordinates(const glm::vec3& p) const override;

	auto getRadius() const { return __radius; }

private:
	float __radius;
};