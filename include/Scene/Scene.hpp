#pragma once

#include <vector>
#include <memory>

class IHittableObject;
class Ray;
class Interval;
struct HitRecord;

class Scene
{
public:
	Scene() = default;
	~Scene() = default;

	void add(std::shared_ptr<IHittableObject> object);
	void clear();
	bool hitAnything(const Ray& ray, 
									 const Interval& interval,
									 HitRecord& record) const;

private:
	std::vector<std::shared_ptr<IHittableObject>> __objects;
};
