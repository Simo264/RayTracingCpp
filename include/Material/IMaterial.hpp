#pragma once

#include <glm/glm.hpp>

class Ray;
struct HitRecord;

class IMaterial
{
public:
	IMaterial() = default;
	~IMaterial() = default;

	virtual bool scatter(const Ray& incident,
											 const HitRecord& rec,
											 Ray& scattered_ray,
											 glm::vec3& attenuation) = 0;
protected:

};
