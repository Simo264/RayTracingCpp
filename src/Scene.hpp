#pragma once

#include <vector>
#include <memory>

class HittableObject;
class Ray;
struct HitRecord;
struct Interval;

class Scene
{
public:
	Scene() = default;
	~Scene() = default;

	void addObject(std::shared_ptr<HittableObject> object);
	void clear();
	bool hitAnything(const Ray& ray, 
									 const Interval& interval,
									 HitRecord& record) const;

private:
	std::vector<std::shared_ptr<HittableObject>> __objects;
};
