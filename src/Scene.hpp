#pragma once

#include <vector>
#include <memory>

class HittableObject;
class Ray;
struct HitRecord;

class Scene
{
public:
	Scene() = default;
	~Scene() = default;

	void addObject(std::shared_ptr<HittableObject> object);
	void clear();
	bool hitAnything(const Ray& ray, float rayTmin, float rayTmax, HitRecord& record) const;

private:
	std::vector<std::shared_ptr<HittableObject>> _objects;
};
