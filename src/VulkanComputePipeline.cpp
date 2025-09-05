#include "VulkanComputePipeline.hpp"
#include "VulkanDeviceManager.hpp"
#include "VulkanDescriptorManager.hpp"
#include "VulkanShader.hpp"

#include <cassert>
#include <iostream>

VulkanComputePipeline::VulkanComputePipeline(std::shared_ptr<VulkanDeviceManager> dev_manager,
                                             std::shared_ptr<VulkanShader> compute_shader,
                                             std::shared_ptr<VulkanDescriptorManager> descriptor_manager) : 
  __dev_manager{ dev_manager },
  __compute_shader{ compute_shader },
  __descriptor_manager{ descriptor_manager },
  __pipeline_layout{ VK_NULL_HANDLE },
  __compute_pipeline{ VK_NULL_HANDLE }
{

  // 1. Definisci lo stage dello shader di calcolo
  auto compute_stage_info = VkPipelineShaderStageCreateInfo{};
  compute_stage_info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
  compute_stage_info.stage = VK_SHADER_STAGE_COMPUTE_BIT;
  compute_stage_info.module = __compute_shader->getShaderModule();
  compute_stage_info.pName = "main"; // Entry point del tuo shader

  // 2. Crea il pipeline layout
  auto pipeline_layout_info = VkPipelineLayoutCreateInfo{};
  pipeline_layout_info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
  pipeline_layout_info.setLayoutCount = 1;
  // Ottieni il layout del descrittore dal DescriptorManager
  auto descriptor_set_layout = __descriptor_manager->getLayout();
  pipeline_layout_info.pSetLayouts = &descriptor_set_layout;

  auto result = vkCreatePipelineLayout(__dev_manager->getLogicalDevice(), &pipeline_layout_info, nullptr, &__pipeline_layout);
  assert(result == VK_SUCCESS && "failed to create compute pipeline layout!");

  // 3. Crea la pipeline di calcolo
  auto pipeline_info = VkComputePipelineCreateInfo{};
  pipeline_info.sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;
  pipeline_info.layout = __pipeline_layout;
  pipeline_info.stage = compute_stage_info;

  result = vkCreateComputePipelines(__dev_manager->getLogicalDevice(), VK_NULL_HANDLE, 1, &pipeline_info, nullptr, &__compute_pipeline);
  assert(result == VK_SUCCESS && "failed to create compute pipeline!");
}

void VulkanComputePipeline::destroy() const
{
  if (__compute_pipeline != VK_NULL_HANDLE)
    vkDestroyPipeline(__dev_manager->getLogicalDevice(), __compute_pipeline, nullptr);
  if (__pipeline_layout != VK_NULL_HANDLE)
    vkDestroyPipelineLayout(__dev_manager->getLogicalDevice(), __pipeline_layout, nullptr);
}

void VulkanComputePipeline::bind(VkCommandBuffer command_buffer) const
{
  vkCmdBindPipeline(command_buffer, VK_PIPELINE_BIND_POINT_COMPUTE, __compute_pipeline);
}
