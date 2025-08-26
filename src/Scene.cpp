#include "Scene.hpp"
#include "Ray.hpp"

void Scene::add(std::shared_ptr<IHittableObject> object)
{
	__objects.push_back(object);
}

void Scene::clear()
{
	__objects.clear();
}

bool Scene::rayCasting(const Ray& ray,
													 float t_min,
													 float t_max,
													 HitRecord& record) const
{
	auto rec = HitRecord{};
	auto hit = false;
	auto closest_tmax = t_max;
	for (const auto& object : __objects)
	{
		if (object->intersect(ray, t_min, closest_tmax, rec))
		{
			hit = true;
			closest_tmax = rec.t;
			record = rec;
		}
	}
	return hit;
}
