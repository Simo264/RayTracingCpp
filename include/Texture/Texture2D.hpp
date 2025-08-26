#pragma once

#include "ITexture.hpp"

#include <filesystem>
#include <memory>

class Texture2D : public ITexture
{
public:
	using path = std::filesystem::path;
	Texture2D(const glm::vec3& color);
	Texture2D(const path& file_path);
	~Texture2D() = default;

	/** @brief Evaluate the texture at given texture coordinates (u, v) */
	glm::vec3 sample(float u, float v) const;
	
	/** @brief Get pixel at position (x, y) */
	glm::vec3 getPixel(glm::uvec2 position) const;
	
	/** @brief Used to convert color values from sRGB space to linear space. */
	glm::vec3 toLinear(const glm::vec3& color) const;

private:
	std::shared_ptr<std::byte[]> __pixels;
	glm::uvec2 __texture_size;
};

template<typename... Args>
inline std::shared_ptr<Texture2D> createTexture2D(Args&&... args)
{
	return std::make_shared<Texture2D>(std::forward<Args>(args)...);
}