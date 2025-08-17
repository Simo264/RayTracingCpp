#pragma once

#include <glm/glm.hpp>

class Interval;

namespace Random
{
	float generateFloat(Interval I);
	glm::vec2 generateVector2(Interval I);
	glm::vec3 generateVector3(Interval I);
	glm::vec3 generateUnitVector();
}
