#pragma once

#include <vulkan/vulkan.h>
#include <filesystem>

class Shader
{
public:
	using path = std::filesystem::path;

	Shader(const path& spirv_file, VkDevice device);
	~Shader() = default;

	void loadBytecode(const path& spirv_file, std::vector<std::byte>& out);
	auto getShaderModule() const { return __shader_module; }

	void destroy() const;

private:
	VkDevice __device;
	VkShaderModule __shader_module;
};

