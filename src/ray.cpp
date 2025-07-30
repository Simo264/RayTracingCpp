#include "ray.hpp"

glm::vec3 Ray::at(float t) const
{
	return _origin + t*_direction; // P(t) = O + t*d
}
