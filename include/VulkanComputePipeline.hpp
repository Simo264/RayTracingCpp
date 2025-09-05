#pragma once

#include <vulkan/vulkan.h>
#include <memory>
#include <vector>

class VulkanDeviceManager;
class VulkanDescriptorManager;
class VulkanShader;

class VulkanComputePipeline
{
public:
  VulkanComputePipeline(std::shared_ptr<VulkanDeviceManager> dev_manager,
                        std::shared_ptr<VulkanShader> compute_shader,
                        std::shared_ptr<VulkanDescriptorManager> descriptor_manager);

  ~VulkanComputePipeline() = default;

  void destroy() const;

  void bind(VkCommandBuffer command_buffer) const;

  auto getPipeline() const { return __compute_pipeline; }
  auto getLayout() const { return __pipeline_layout; }

private:
  std::shared_ptr<VulkanDeviceManager> __dev_manager;
  std::shared_ptr<VulkanShader> __compute_shader;
  std::shared_ptr<VulkanDescriptorManager> __descriptor_manager;

  VkPipelineLayout __pipeline_layout;
  VkPipeline __compute_pipeline;
};