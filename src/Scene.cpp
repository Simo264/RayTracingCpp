#include "Scene.hpp"
#include "Ray.hpp"
#include "HittableObject.hpp"

void Scene::addObject(std::shared_ptr<HittableObject> object)
{
	_objects.push_back(object);
}

void Scene::clear()
{
	_objects.clear();
}

bool Scene::hitAnything(const Ray& ray, float rayTmin, float rayTmax, HitRecord& record) const
{
	HitRecord tmpRec{};
	bool hitAnything = false;
	float closestTmax = rayTmax;

	for (const auto& object : _objects)
	{
		if (object->hit(ray, rayTmin, closestTmax, tmpRec))
		{
			closestTmax = tmpRec.t;
			hitAnything = true;
			record = tmpRec;
		}
	}
	return hitAnything;
}
