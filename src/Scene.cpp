#include "Scene.hpp"
#include "Ray.hpp"
#include "HittableObject.hpp"
#include "Interval.hpp"

void Scene::addObject(std::shared_ptr<HittableObject> object)
{
	_objects.push_back(object);
}

void Scene::clear()
{
	_objects.clear();
}

bool Scene::hitAnything(const Ray& ray, 
												const Interval& interval, 
												HitRecord& record) const
{
	HitRecord tmpRec{};
	bool hitAnything = false;
	float closestTmax = interval.max;

	for (const auto& object : _objects)
	{
		if (object->hit(ray, Interval(interval.min, closestTmax), tmpRec))
		{
			closestTmax = tmpRec.t;
			hitAnything = true;
			record = tmpRec;
		}
	}
	return hitAnything;
}
