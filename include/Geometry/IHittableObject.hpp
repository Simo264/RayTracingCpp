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
	float tc_u;															// Texture coordinate u 
	float tc_v;															// Texture coordinate v 
	float t;																// Distance along the ray
	bool is_ray_outside;
	std::shared_ptr<IMaterial> material;
};


class IHittableObject
{
public:
	IHittableObject(const glm::vec3& position,
									const std::shared_ptr<IMaterial>& material) : 
		__position{ position},
		__material{ material }
	{}
	virtual ~IHittableObject() = default;

	virtual bool intersect(const Ray& ray,
												 float t_min, 
												 float t_max,
												 HitRecord& hit) const = 0;
	virtual glm::vec3 getNormal(const glm::vec3& p) const = 0;
	virtual glm::vec2 getTextureCoordinates(const glm::vec3& p) const = 0;

	const auto& getMaterial() const { return __material; }
	const auto& getPosition() const { return __position; }


protected:
	glm::vec3 __position;
	std::shared_ptr<IMaterial> __material;
};

template<typename ObjectType, typename... Args>
inline std::shared_ptr<IHittableObject> createObject(Args&&... args)
{
	static_assert(std::is_base_of<IHittableObject, ObjectType>::value, "ObjectType must inherit from IHittableObject");
	return std::make_shared<ObjectType>(std::forward<Args>(args)...);
}

