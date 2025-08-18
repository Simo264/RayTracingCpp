#pragma once

#include <glm/glm.hpp>

class Interval;

namespace Random
{
	float generateRandomFloat(Interval I);
	glm::vec2 generateRandomVector2(Interval I);
	glm::vec3 generateRandomVector3(Interval I);
	glm::vec3 generateRandomUnitVector3();

	glm::vec2 generateRandomUnitDiskPoint();
}
