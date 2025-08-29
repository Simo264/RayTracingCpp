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

	const auto& getObjects() const { return __objects; }
	
	/** @brief Retrieve specific types of objects */
	template<typename T>
	std::vector<std::shared_ptr<T>> getObjects() const
	{
		std::vector<std::shared_ptr<T>> found_objects;
		for (const auto& object : __objects)
		{
			auto specific_object = std::dynamic_pointer_cast<T>(object);
			if (specific_object)
				found_objects.push_back(specific_object);
		}
		return found_objects;
	}

	/** @brief Get all objects that have an Emissive material */
	std::vector<std::shared_ptr<IHittableObject>> getEmissiveObjects() const;

private:
	std::vector<std::shared_ptr<IHittableObject>> __objects;
};
