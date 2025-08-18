#include "Random.hpp"
#include "Interval.hpp"

#include <random>
#include <glm/gtx/norm.hpp> // glm::length2

namespace Random
{
	static std::mt19937 __generator(std::random_device{}());

	float generateRandomFloat(Interval I)
	{
		std::uniform_real_distribution<float> dist(I.min, I.max);
		return dist(__generator);
	}

	glm::vec2 generateRandomVector2(Interval I)
	{
		std::uniform_real_distribution<float> dist(I.min, I.max);
		auto v = glm::vec2(dist(__generator), dist(__generator));
		return v;
	}
	
	glm::vec3 generateRandomVector3(Interval I)
	{
		std::uniform_real_distribution<float> dist(I.min, I.max);
		auto v = glm::vec3(dist(__generator), dist(__generator), dist(__generator));
		return v;
	}

	glm::vec3 generateRandomUnitVector3()
	{
		while (true)
		{
			auto v = generateRandomVector3(Interval(-1.0f, 1.0f));
			auto lensq = glm::length2(v);	// More efficient than glm::length(p) * glm::length(p)
			if (lensq <= 1.0f && lensq > 1e-6f)
				return v / std::sqrt(lensq); // = glm::normalize(v)
		}
	}

	glm::vec2 generateRandomUnitDiskPoint()
	{
		while (true)
		{
			auto p = glm::vec2(generateRandomFloat(Interval(-1.f, 1.f)), generateRandomFloat(Interval(-1.f, 1.f)));
			if (glm::length2(p) < 1)
				return p;
		}
	}
}
