#include "Paths.hpp"

std::filesystem::path getResourcesDir()
{
	static auto current_path = std::filesystem::current_path();
	static auto res_dir = (current_path.parent_path() / "resources").lexically_normal();
	return res_dir;
}

std::filesystem::path getShadersDir()
{
	static auto current_path = std::filesystem::current_path();
	static auto shader_dir = (current_path.parent_path() / "shaders").lexically_normal();
	return shader_dir;
}
