#include "Shader.hpp"

#include <cassert>
#include <iostream>
#include <fstream>

Shader::Shader(const path& spirv_file, VkDevice device) : 
	__device{ device },
	__shader_module{ VK_NULL_HANDLE }
{
	if (!std::filesystem::exists(spirv_file))
	{
		std::cerr << "Error: spirv file does not exist: " << spirv_file.string() << std::endl;
		exit(1);
	}

	auto bytecode = std::vector<std::byte>();
	loadBytecode(spirv_file, bytecode);

	auto create_info = VkShaderModuleCreateInfo{};
	create_info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	create_info.codeSize = bytecode.size();
	create_info.pCode = reinterpret_cast<const uint32_t*>(bytecode.data());
	auto result = vkCreateShaderModule(device, &create_info, nullptr, &__shader_module);
	assert(result == VK_SUCCESS && "failed to create shader module!");
}

void Shader::loadBytecode(const path& spirv_file, std::vector<std::byte>& out)
{
	auto file = std::ifstream(spirv_file, std::ios::ate | std::ios::binary);
	if (!file)
	{
		std::cerr << "Error on opening file " << spirv_file.string() << std::endl;
		exit(1);
	}
	
	auto file_size = static_cast<size_t>(file.tellg());
	out = std::vector<std::byte>(file_size);

	file.seekg(0);
	file.read(reinterpret_cast<char*>(out.data()), file_size);
	file.close();
}

void Shader::destroy() const
{
	if (__shader_module != VK_NULL_HANDLE)
		vkDestroyShaderModule(__device, __shader_module, nullptr);
}
