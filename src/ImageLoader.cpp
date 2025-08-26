#include "ImageLoader.hpp"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

namespace ImageLoader
{
	bool writePNG(const path& file_path,
								 glm::uvec2 image_size,
								 const std::byte* data)
	{
		auto success = stbi_write_png(file_path.string().c_str(),
																	image_size.x,
																	image_size.y,
																	3,
																	data,
																	image_size.x * 3);
		return success;
	}
	
	std::byte* load(const path& file_path,
									int& width,
									int& height,
									int& nr_channels)
	{
		auto data = stbi_load(file_path.string().c_str(), &width, &height, &nr_channels, 3);
		return reinterpret_cast<std::byte*>(data);
	}

	void imageFree(std::byte* data)
	{
		stbi_image_free(data);
	}
}
