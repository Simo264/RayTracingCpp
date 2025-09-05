#include "Paths.hpp"

std::filesystem::path& getRootDir()
{
	auto current_path = std::filesystem::current_path();
	static auto root_dir = current_path.parent_path();
	return root_dir;
}

std::filesystem::path& getResourcesDir()
{
	const auto& root = getRootDir();
	static auto res_dir = (root / "resources").lexically_normal();
	return res_dir;
}

std::filesystem::path& getShadersDir()
{
	const auto& root = getRootDir();
	static auto shader_dir = (root / "shaders").lexically_normal();
	return shader_dir;
}
