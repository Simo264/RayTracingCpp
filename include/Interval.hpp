#pragma once

#include <glm/glm.hpp>

class Interval
{
public:
	constexpr Interval(float min, float max) : min{ min }, max{ max } {}

	auto size() const { return glm::abs(max - min); }
	auto contains(float x) const { return x >= min && x <= max; }
	auto surrounds(double x) const { return x > min && x < max; }

	float min, max;
};
