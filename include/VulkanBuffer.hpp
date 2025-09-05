#pragma once

#include <vulkan/vulkan.h>
#include <memory>
#include <vector>

class VulkanDeviceManager; 

class VulkanBuffer
{
public:
  VulkanBuffer(std::shared_ptr<VulkanDeviceManager> dev_manager,
               size_t size,
               const void* data,
               VkBufferUsageFlags usage,
               VkMemoryPropertyFlags properties);

  ~VulkanBuffer() = default;

  void loadData(const void* data);

  void readMappedData(std::vector<std::byte>& out) const;

  void destroy() const;

  auto getBuffer() const { return __buffer; }
  auto getMemory() const { return __buffer_memory; }
  auto getBufferSize() const { return __buffer_size; }

private:
  uint32_t __findMemoryType(VkPhysicalDevice physical_device, 
                            uint32_t type_filter, 
                            VkMemoryPropertyFlags properties);
  
  std::shared_ptr<VulkanDeviceManager> __dev_manager;

  VkBuffer __buffer;
  VkDeviceMemory __buffer_memory;
  size_t __buffer_size;
};
