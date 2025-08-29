#pragma once

#include "IHittableObject.hpp"

class Plane : public IHittableObject
{
public:
	Plane(const glm::vec3& position,		// the plane's center.
				const std::shared_ptr<IMaterial>& material,
				glm::vec3 orientation,				// the plane's normal.
				float width,
				float height
	)	: 
		IHittableObject(position, material),
		__orientation{ orientation },
		__width{ width },
		__height{ height }
	{}
	~Plane() = default;

	bool intersect(const Ray& ray,
								 float t_min,
								 float t_max,
								 HitRecord& hit) const override;

	/** @brief return the normal vector */
	glm::vec3 getNormal(const glm::vec3& p) const override { return __orientation; }

	/** @brief return the local, unnormalized (u, v) coordinates */
	glm::vec2 getTextureCoordinates(const glm::vec3& p) const override;

private:
	glm::vec3 __orientation;
	float __width;
	float __height;
};

