#pragma once

#include <glm/glm.hpp>

/**
 * 4.7. Texture Mapping
 * In the real world, material properties change on objects' surfaces.
 * A more practical manner to model material variations is to use images and access image locations from the surface while 
 * rendering. In graphics these images are called textures. 
 * In practice, using textures decouples the resolution of the material variations from the geometric resolution, 
 * just like using vertex normals gives a smoother look even when using a small number of polygons.
 * To apply an image onto a surface we need to determine the image coordinates , called texture coordinates, 
 * that corresponds to each surface point.
 * Texture coordinates are defined in the nominal range S:[0,1].
 * We can think of this assignment as defining a function s(p) that maps surface points p to image coordinates.
 * We can write the mapping function as s(p): vec3 -> vec2
 *
 * There are several ways to define texture coordinates. For example, a sphere can be parametrized with spherical coordinates, 
 * or with by treating it as a cube mesh and using the cube parametrization.
 * For discretized surfaces, we can store texture coordinates at the vertices of the mesh, and interpolate them inside 
 * the triangles using barycentric interpolation.
 * For a whole mesh, texture coordinates are generally authored manually in a 3D modeling software, 
 * and computed automatically either during editing or when scanning real-world objects.
 * 
 * Textures can be used to vary any material parameter.
 * Textures can also be used to alter the surface geometry used to compute an image. 
 * This gives the appearance of a more detailed, rougher, geometry.
 * 
 * 
 * 5.14. Textures
 * As discussed in the previous chapter, textures are images that represent variations of the material parameters. 
 * Textures are applied to surfaces by assigning texture coordinates s_i to each triangle vertex P_i.
 * We compute texture coordinates s for points P inside the triangle by interpolation with barycentric coordinates.
 * 
 * Differently than images, we often want to repeat the same texture to cover large surfaces, 
 * which can be obtained allowing texture coordinates to be outside of the [0,1] range, and using the modulus operation to 
 * bring them back to the canonical domain.
 * Also, texture values should always be linear since they either represent HDR illumination or linear material parameters.
 * For this reasons, LDR textures need to be converted to linear space before being used. 
 * 
 * Textures are also used in environment maps. In this case, texture coordinates depend on the ray direction d.
 */

class ITexture
{
public:
	ITexture() = default;
	~ITexture() = default;

	/** @brief Evaluate the texture at given texture coordinates (u, v) */ 
	virtual glm::vec3 sample(float u, float v) const = 0;
};
