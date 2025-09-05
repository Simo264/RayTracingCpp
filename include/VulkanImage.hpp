#pragma once

#include <vulkan/vulkan.h>
#include <memory>

class VulkanDeviceManager;
class VulkanCommandManager;
class VulkanBuffer;

class VulkanImage
{
public:
  VulkanImage(std::shared_ptr<VulkanDeviceManager> dev_manager,
              uint32_t width,
              uint32_t height,
              VkFormat format,
              VkImageUsageFlags usage,
              VkMemoryPropertyFlags properties);
  ~VulkanImage() = default;

  // Getters
  auto getImage() const { return __image; }
  auto getImageView() const { return __image_view; }
  auto getMemory() const { return __image_memory; }

  void copyToBuffer(VkCommandBuffer command_buffer, 
                    std::shared_ptr<VulkanBuffer> dst_buffer, 
                    uint32_t width, 
                    uint32_t height) const;

  VkImageMemoryBarrier createImageLayoutBarrier(VkImageLayout old_layout,
                                                VkImageLayout new_layout) const;

  void destroy() const;

private:
  uint32_t __findMemoryType(VkPhysicalDevice physical_device, uint32_t type_filter, VkMemoryPropertyFlags properties);
  
  std::shared_ptr<VulkanDeviceManager> __dev_manager;
  VkImage __image;
  VkImageView __image_view;
  VkDeviceMemory __image_memory;
};
