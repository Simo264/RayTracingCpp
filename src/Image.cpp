#include "Image.hpp"

Image::Image(uint32_t width, uint32_t height, uint32_t nr_channels)
	: resolution_w{ width }, resolution_h{ height }, nr_channels{ nr_channels }
{
	if (width != 0 && height != 0)
		data = std::make_shared<std::byte[]>(static_cast<uint64_t>(width * height * nr_channels));
}

void Image::getPixel(uint32_t x,
										 uint32_t y,
										 std::byte& r,
										 std::byte& g,
										 std::byte& b) const
{
	auto index = static_cast<uint64_t>(y * resolution_w + x) * nr_channels;
	 r = data[index + 0];
	 g = data[index + 1];
	 b = data[index + 2];
}

void Image::setPixel(uint32_t x,
										 uint32_t y,
										 std::byte r,
										 std::byte g,
										 std::byte b) const
{
	auto index = static_cast<uint64_t>(y * resolution_w + x) * nr_channels;
	data[index + 0] = r;
	data[index + 1] = g;
	data[index + 2] = b;
}
