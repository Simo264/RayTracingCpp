#include "Scene/Scene.hpp"
#include "Scene/IHittableObject.hpp"
#include "Ray.hpp"
#include "Interval.hpp"

void Scene::add(std::shared_ptr<IHittableObject> object)
{
	__objects.push_back(object);
}

void Scene::clear()
{
	__objects.clear();
}

bool Scene::hitAnything(const Ray& ray, const Interval& interval, HitRecord& record) const
{
	HitRecord rec{};
	bool hit_anything = false;
	float closest_tmax = interval.max;

	for (const auto& object : __objects)
	{
		if (object->hit(ray, Interval(interval.min, closest_tmax), rec))
		{
			closest_tmax = rec.t;
			hit_anything = true;
			record = rec;
		}
	}
	return hit_anything;
}
