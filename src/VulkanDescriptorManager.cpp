#include "VulkanDescriptorManager.hpp"
#include "VulkanDeviceManager.hpp"

#include <cassert>


VulkanDescriptorManager::VulkanDescriptorManager(std::shared_ptr<VulkanDeviceManager> dev_manager) :
  __dev_manager{ dev_manager },
  __descriptor_set_layout{ VK_NULL_HANDLE },
  __descriptor_pool{ VK_NULL_HANDLE },
  __descriptor_set{ VK_NULL_HANDLE }
{}

void VulkanDescriptorManager::cleanup() const
{
  if (__descriptor_pool != VK_NULL_HANDLE)
    vkDestroyDescriptorPool(__dev_manager->getLogicalDevice(), __descriptor_pool, nullptr);
  if (__descriptor_set_layout != VK_NULL_HANDLE)
    vkDestroyDescriptorSetLayout(__dev_manager->getLogicalDevice(), __descriptor_set_layout, nullptr);
}

void VulkanDescriptorManager::addBinding(uint32_t binding, VkDescriptorType type, VkShaderStageFlags stageFlags)
{
  // 1. Aggiunge il binding al vettore
  auto new_binding = VkDescriptorSetLayoutBinding{};
  new_binding.binding = binding;
  new_binding.descriptorType = type;
  new_binding.descriptorCount = 1;
  new_binding.stageFlags = stageFlags;
  __bindings.push_back(new_binding);

  // 2. Aggiorna il conteggio delle dimensioni del pool
  auto found = false;
  for (auto& pool_size : __pool_sizes)
  {
    if (pool_size.type == type)
    {
      pool_size.descriptorCount++;
      found = true;
      break;
    }
  }

  if (!found)
    __pool_sizes.push_back({ type, 1 });
}

void VulkanDescriptorManager::build()
{
  // 1. Crea il layout del descrittore
  auto layout_info = VkDescriptorSetLayoutCreateInfo{};
  layout_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
  layout_info.bindingCount = static_cast<uint32_t>(__bindings.size());
  layout_info.pBindings = __bindings.data();

  auto result = vkCreateDescriptorSetLayout(__dev_manager->getLogicalDevice(), &layout_info, nullptr, &__descriptor_set_layout);
  assert(result == VK_SUCCESS && "failed to create descriptor set layout!");

  // 2. Crea il pool di descrittori
  auto pool_info = VkDescriptorPoolCreateInfo{};
  pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
  pool_info.poolSizeCount = static_cast<uint32_t>(__pool_sizes.size());
  pool_info.pPoolSizes = __pool_sizes.data();
  pool_info.maxSets = 1; // Gestiamo un solo set per ora

  result = vkCreateDescriptorPool(__dev_manager->getLogicalDevice(), &pool_info, nullptr, &__descriptor_pool);
  assert(result == VK_SUCCESS && "failed to create descriptor pool!");

  // 3. Alloca il set di descrittori
  auto alloc_info = VkDescriptorSetAllocateInfo{};
  alloc_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
  alloc_info.descriptorPool = __descriptor_pool;
  alloc_info.descriptorSetCount = 1;
  alloc_info.pSetLayouts = &__descriptor_set_layout;

  result = vkAllocateDescriptorSets(__dev_manager->getLogicalDevice(), &alloc_info, &__descriptor_set);
  assert(result == VK_SUCCESS && "failed to allocate descriptor set!");
}

void VulkanDescriptorManager::updateBuffer(uint32_t binding, VkDescriptorType type, VkBuffer buffer, VkDeviceSize range)
{
  auto buffer_info = VkDescriptorBufferInfo{};
  buffer_info.buffer = buffer;
  buffer_info.offset = 0;
  buffer_info.range = range;

  auto descriptor_write = VkWriteDescriptorSet{};
  descriptor_write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
  descriptor_write.dstSet = __descriptor_set;
  descriptor_write.dstBinding = binding;
  descriptor_write.dstArrayElement = 0;
  descriptor_write.descriptorType = type;
  descriptor_write.descriptorCount = 1;
  descriptor_write.pBufferInfo = &buffer_info;

  vkUpdateDescriptorSets(__dev_manager->getLogicalDevice(), 1, &descriptor_write, 0, nullptr);
}

void VulkanDescriptorManager::updateImage(uint32_t binding, VkDescriptorType type, VkImageView imageView, VkSampler sampler)
{
  auto image_info = VkDescriptorImageInfo{};
  image_info.imageLayout = VK_IMAGE_LAYOUT_GENERAL; // Usato per storage image
  image_info.imageView = imageView;
  image_info.sampler = sampler;

  auto descriptor_write = VkWriteDescriptorSet{};
  descriptor_write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
  descriptor_write.dstSet = __descriptor_set;
  descriptor_write.dstBinding = binding;
  descriptor_write.dstArrayElement = 0;
  descriptor_write.descriptorType = type;
  descriptor_write.descriptorCount = 1;
  descriptor_write.pImageInfo = &image_info;

  vkUpdateDescriptorSets(__dev_manager->getLogicalDevice(), 1, &descriptor_write, 0, nullptr);
}

void VulkanDescriptorManager::bind(VkCommandBuffer command_buffer, 
                                   VkPipelineLayout pipeline_layout) const
{
  vkCmdBindDescriptorSets(command_buffer,
                          VK_PIPELINE_BIND_POINT_COMPUTE,
                          pipeline_layout,
                          0,
                          1,
                          &__descriptor_set,
                          0,
                          nullptr);
}
