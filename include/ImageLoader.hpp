#pragma once

#include <filesystem>
#include <glm/glm.hpp>

namespace ImageLoader
{
	using path = std::filesystem::path;

	bool writePNG(const path& file_path,
								 glm::uvec2 image_size,
								 const std::byte* data);

	std::byte* load(const path& file_path,
									int& width,
									int& height,
									int& nr_channels);

	void imageFree(std::byte* data);
}
