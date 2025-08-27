#pragma once

#include <vector>
#include <memory>
#include "Geometry/IHittableObject.hpp"

class Ray;

class Scene
{
public:
	Scene() = default;
	~Scene() = default;

	void add(std::shared_ptr<IHittableObject> object);
	void clear();
	bool rayCasting(const Ray& ray,
									float t_min,
									float t_max,
									HitRecord& record) const;

	auto& getObjects() const { return __objects; }

private:
	std::vector<std::shared_ptr<IHittableObject>> __objects;
};
