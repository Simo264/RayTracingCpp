#pragma once

#include <memory>
#include <glm/glm.hpp>
#include "Material/IMaterial.hpp"

class Ray;

struct HitRecord
{
	HitRecord() : 
		point{},
		normal{},
		t{},
		tc_u{},
		tc_v{},
		is_ray_outside{ true },
		material{ nullptr }
	{}

	glm::vec3 point;												// Intersection point
	glm::vec3 normal;												// Surface normal at the hit point
	float t;																// Distance along the ray
	float tc_u;															// Texture coordinate u
	float tc_v;															// Texture coordinate v
	bool is_ray_outside;
	std::shared_ptr<IMaterial> material;
};


class IHittableObject
{
public:
	IHittableObject() = default;
	~IHittableObject() = default;

	virtual bool intersect(const Ray& ray,
												 float t_min, 
												 float t_max,
												 HitRecord& hit) const = 0;
};

template<typename ObjectType, typename... Args>
inline std::shared_ptr<IHittableObject> createObject(Args&&... args)
{
	static_assert(std::is_base_of<IHittableObject, ObjectType>::value, "ObjectType must inherit from IHittableObject");
	return std::make_shared<ObjectType>(std::forward<Args>(args)...);
}

