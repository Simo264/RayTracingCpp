#pragma once

#include <vulkan/vulkan.h>
#include <filesystem>
#include <vector>

class VulkanShader
{
public:
	using path = std::filesystem::path;

	VulkanShader(const path& spirv_file, VkDevice device);
	~VulkanShader() = default;

	void loadBytecode(const path& spirv_file, std::vector<std::byte>& out);
	auto getShaderModule() const { return __shader_module; }

	void destroyModule() const;

private:
	VkDevice __device;
	VkShaderModule __shader_module;
};

