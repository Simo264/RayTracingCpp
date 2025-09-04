#pragma once

#include <vulkan/vulkan.h>
#include <array>
#include <memory>

class Shader;

class GraphicsPipeline
{
public:
	GraphicsPipeline(std::shared_ptr<Shader> vertex_shader,
									 std::shared_ptr<Shader> fragment_shader,
									 VkDevice device, 
									 VkRenderPass render_pass,
									 VkExtent2D swapchain_extent);
	~GraphicsPipeline() = default;

	auto getVkPipeline() const { return __graphics_pipeline; }
	auto getVkPipelineLayout() const { return __pipeline_layout; }
	
	void destroy() const;
	
	GraphicsPipeline(const GraphicsPipeline&) = delete;
	GraphicsPipeline& operator=(const GraphicsPipeline&) = delete;

private:
	VkDevice __device;
	VkPipeline __graphics_pipeline;
	VkPipelineLayout __pipeline_layout;
};

