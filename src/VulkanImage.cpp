#include "VulkanImage.hpp"
#include "VulkanDeviceManager.hpp"
#include "VulkanCommandManager.hpp"
#include "VulkanBuffer.hpp"

#include <cassert>
#include <iostream>

VulkanImage::VulkanImage(std::shared_ptr<VulkanDeviceManager> dev_manager,
                         uint32_t width,
                         uint32_t height,
                         VkFormat format,
                         VkImageUsageFlags usage,
                         VkMemoryPropertyFlags properties) : 
  __dev_manager{ dev_manager },
  __image{ VK_NULL_HANDLE },
  __image_view{ VK_NULL_HANDLE },
  __image_memory{ VK_NULL_HANDLE }
{

  // 1. Creazione dell'immagine (VkImage)
  auto image_info = VkImageCreateInfo{};
  image_info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
  image_info.imageType = VK_IMAGE_TYPE_2D;
  image_info.extent.width = width;
  image_info.extent.height = height;
  image_info.extent.depth = 1;
  image_info.mipLevels = 1;
  image_info.arrayLayers = 1;
  image_info.format = format;
  image_info.tiling = VK_IMAGE_TILING_OPTIMAL;
  image_info.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
  image_info.usage = usage;
  image_info.samples = VK_SAMPLE_COUNT_1_BIT;
  image_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

  auto result = vkCreateImage(__dev_manager->getLogicalDevice(), &image_info, nullptr, &__image);
  assert(result == VK_SUCCESS && "Failed to create image!");

  // 2. Allocazione della memoria (VkDeviceMemory)
  auto mem_requirements = VkMemoryRequirements{};
  vkGetImageMemoryRequirements(__dev_manager->getLogicalDevice(), __image, &mem_requirements);

  auto alloc_info = VkMemoryAllocateInfo{};
  alloc_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
  alloc_info.allocationSize = mem_requirements.size;
  alloc_info.memoryTypeIndex = __findMemoryType(__dev_manager->getPhysicalDevice(),
                                                mem_requirements.memoryTypeBits,
                                                properties);

  result = vkAllocateMemory(__dev_manager->getLogicalDevice(), &alloc_info, nullptr, &__image_memory);
  assert(result == VK_SUCCESS && "Failed to allocate image memory!");

  // 3. Binding dell'immagine alla memoria
  vkBindImageMemory(__dev_manager->getLogicalDevice(), __image, __image_memory, 0);

  // 4. Creazione della vista dell'immagine (VkImageView)
  auto view_info = VkImageViewCreateInfo{};
  view_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
  view_info.image = __image;
  view_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
  view_info.format = format;
  view_info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
  view_info.subresourceRange.baseMipLevel = 0;
  view_info.subresourceRange.levelCount = 1;
  view_info.subresourceRange.baseArrayLayer = 0;
  view_info.subresourceRange.layerCount = 1;

  result = vkCreateImageView(__dev_manager->getLogicalDevice(), &view_info, nullptr, &__image_view);
  assert(result == VK_SUCCESS && "Failed to create image view!");
}


void VulkanImage::destroy() const
{
  if (__image_view != VK_NULL_HANDLE)
    vkDestroyImageView(__dev_manager->getLogicalDevice(), __image_view, nullptr);
  if (__image != VK_NULL_HANDLE)
    vkDestroyImage(__dev_manager->getLogicalDevice(), __image, nullptr);
  if (__image_memory != VK_NULL_HANDLE)
    vkFreeMemory(__dev_manager->getLogicalDevice(), __image_memory, nullptr);
}

void VulkanImage::copyToBuffer(VkCommandBuffer command_buffer, 
                               std::shared_ptr<VulkanBuffer> dst_buffer, 
                               uint32_t width, 
                               uint32_t height) const
{
  // Regione di copia
  auto region = VkBufferImageCopy{};
  region.bufferOffset = 0;
  region.bufferRowLength = 0;
  region.bufferImageHeight = 0;
  region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
  region.imageSubresource.mipLevel = 0;
  region.imageSubresource.baseArrayLayer = 0;
  region.imageSubresource.layerCount = 1;
  region.imageOffset = { 0, 0, 0 };
  region.imageExtent = { width, height, 1 };

  // Copia l'immagine nel buffer di destinazione
  vkCmdCopyImageToBuffer(command_buffer,
                         __image,
                         VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
                         dst_buffer->getBuffer(),
                         1,
                         &region);
}


VkImageMemoryBarrier VulkanImage::createImageLayoutBarrier(VkImageLayout oldLayout, VkImageLayout newLayout) const
{
  auto barrier = VkImageMemoryBarrier{};
  barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
  barrier.oldLayout = oldLayout;
  barrier.newLayout = newLayout;
  barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
  barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
  barrier.image = __image;
  barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
  barrier.subresourceRange.baseMipLevel = 0;
  barrier.subresourceRange.levelCount = 1;
  barrier.subresourceRange.baseArrayLayer = 0;
  barrier.subresourceRange.layerCount = 1;

  if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_GENERAL)
  {
    barrier.srcAccessMask = 0;
    barrier.dstAccessMask = VK_ACCESS_SHADER_WRITE_BIT;
  }
  else if (oldLayout == VK_IMAGE_LAYOUT_GENERAL && newLayout == VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL)
  {
    barrier.srcAccessMask = VK_ACCESS_SHADER_WRITE_BIT;
    barrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
  }

  return barrier;
}



uint32_t VulkanImage::__findMemoryType(VkPhysicalDevice physical_device,
                                       uint32_t type_filter,
                                       VkMemoryPropertyFlags properties)
{
  auto mem_properties = VkPhysicalDeviceMemoryProperties{};
  vkGetPhysicalDeviceMemoryProperties(physical_device, &mem_properties);

  for (auto i = 0u; i < mem_properties.memoryTypeCount; i++)
  {
    if ((type_filter & (1 << i)) && (mem_properties.memoryTypes[i].propertyFlags & properties) == properties)
    {
      return i;
    }
  }

  std::cerr << "failed to find suitable memory type!" << std::endl;
  exit(1);
}