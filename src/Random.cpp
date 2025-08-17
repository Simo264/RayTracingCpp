#include "Random.hpp"
#include "Interval.hpp"

#include <random>
#include <glm/gtx/norm.hpp> // glm::length2

namespace Random
{
	static std::mt19937 __generator(std::random_device{}());

	float generateFloat(Interval I)
	{
		std::uniform_real_distribution<float> dist(I.min, I.max);
		return dist(__generator);
	}

	glm::vec2 generateVector2(Interval I)
	{
		std::uniform_real_distribution<float> dist(I.min, I.max);
		auto v = glm::vec2(dist(__generator), dist(__generator));
		return v;
	}
	
	glm::vec3 generateVector3(Interval I)
	{
		std::uniform_real_distribution<float> dist(I.min, I.max);
		auto v = glm::vec3(dist(__generator), dist(__generator), dist(__generator));
		return v;
	}

	glm::vec3 generateUnitVector()
	{
		constexpr Interval I(-1.0f, 1.0f);
		while (true)
		{
			auto v = generateVector3(I);
			auto lensq = glm::length2(v);	// More efficient than glm::length(p) * glm::length(p)
			if (lensq <= 1.0f && lensq > 1e-6f)
				return v / std::sqrt(lensq); // = glm::normalize(v)
		}
	}
}
