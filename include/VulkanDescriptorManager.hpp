#pragma once

#include <vulkan/vulkan.h>
#include <memory>
#include <vector>

class VulkanDeviceManager;

class VulkanDescriptorManager
{
public:
  VulkanDescriptorManager(std::shared_ptr<VulkanDeviceManager> dev_manager);
  ~VulkanDescriptorManager() = default;

  void cleanup() const;

  void bind(VkCommandBuffer command_buffer, VkPipelineLayout pipeline_layout) const;

  // Aggiunge un binding al layout del descrittore
  void addBinding(uint32_t binding, VkDescriptorType type, VkShaderStageFlags stageFlags);

  // Costruisce il layout, il pool e alloca il set
  void build();

  // Aggiorna il descrittore con un buffer
  void updateBuffer(uint32_t binding, VkDescriptorType type, VkBuffer buffer, VkDeviceSize range);

  // Aggiorna il descrittore con un'immagine
  void updateImage(uint32_t binding, VkDescriptorType type, VkImageView imageView, VkSampler sampler);

  // Getters
  auto getLayout() const { return __descriptor_set_layout; }
  auto getSet() const { return __descriptor_set; }
  auto getPool() const { return __descriptor_pool; }

private:
  std::shared_ptr<VulkanDeviceManager> __dev_manager;
  std::vector<VkDescriptorSetLayoutBinding> __bindings;
  std::vector<VkDescriptorPoolSize> __pool_sizes;

  VkDescriptorSetLayout __descriptor_set_layout;
  VkDescriptorPool __descriptor_pool;
  VkDescriptorSet __descriptor_set;
};