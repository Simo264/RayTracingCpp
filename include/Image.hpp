#pragma once

#include <memory>

class Image
{
public:
	Image(uint32_t width = 0, uint32_t height = 0, uint32_t nr_channels = 3);
	~Image() = default;

	void getPixel(uint32_t x, 
								uint32_t y, 
								std::byte& r, 
								std::byte& g, 
								std::byte& b) const;

	void setPixel(uint32_t x, 
								uint32_t y, 
								std::byte r, 
								std::byte g, 
								std::byte b) const;

	uint32_t resolution_w;
	uint32_t resolution_h;
	uint32_t nr_channels;
	std::shared_ptr<std::byte[]> data;
};
