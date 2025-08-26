#pragma once

#include <glm/glm.hpp>

// 5.7 Bounding Volumes
// We can observe though that the ray-scene intersection is fundamentally a search operation, 
// where we look for the closest element. 
// The methods discussed so far perform this search with a linear algorithm, testing all elements in the scene. 
// But just like other search problems in computer science, we get logarithmic complexity by sorting the data 
// in a data structure that allows us to quickly prune parts of the search space.
// A simple acceleration we can consider is to group multiple elements in a bounding volume that fully contains them. 
// We then test the ray against the bounding volume before testing the elements.
// If the ray hits the bounding volume, we test the elements contained in the bounding volume. 
// Otherwise, we can skip the elements since the ray cannot intersect them being outside the bounding volume.
// We represent an Axis-Aligned Bounding Box (AABB) B = [b1, b2] as the minimum b1 and the maximum b2 
// coordinate values along each axis.
//
// 5.8 Ray-Box Intersection
// To use bounding boxes, we need to compute the intersection between a ray and an axis-aligned box. 
// For this intersection routine, we only need to test whether an intersection has occurred, 
// rather than returning all intersection data. 
//
// To test for box intersection, we first intersect the ray against each slab.
// Since slabs are axis aligned, and consider only one coordinate, each intersection can be solved as a one-dimensional 
// problem and will give two results, one for each face in the slab. 
// The intersections occur in all cases except when the ray is parallel to the slab.
class AABB
{
public:
	AABB() = default;
	~AABB() = default;
};

