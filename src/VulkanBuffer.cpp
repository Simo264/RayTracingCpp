#include "VulkanBuffer.hpp"
#include "VulkanDeviceManager.hpp"

#include <cassert>
#include <iostream>
#include <cstring>


VulkanBuffer::VulkanBuffer(std::shared_ptr<VulkanDeviceManager> dev_manager,
                           size_t size,
                           const void* data,
                           VkBufferUsageFlags usage,
                           VkMemoryPropertyFlags properties):
  __dev_manager{ dev_manager },
  __buffer_size{ size },
  __buffer{ VK_NULL_HANDLE }, 
  __buffer_memory{ VK_NULL_HANDLE }
{
  // 1. Creazione del buffer
  auto buffer_info = VkBufferCreateInfo{};
  buffer_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
  buffer_info.size = __buffer_size;
  buffer_info.usage = usage;
  buffer_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

  auto result = vkCreateBuffer(__dev_manager->getLogicalDevice(), &buffer_info, nullptr, &__buffer);
  assert(result == VK_SUCCESS && "failed to create buffer!");

  // 2. Requisiti di memoria
  auto mem_requirements = VkMemoryRequirements{};
  vkGetBufferMemoryRequirements(__dev_manager->getLogicalDevice(), __buffer, &mem_requirements);

  // 3. Allocazione della memoria
  auto alloc_info = VkMemoryAllocateInfo{};
  alloc_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
  alloc_info.allocationSize = mem_requirements.size;
  alloc_info.memoryTypeIndex = __findMemoryType(__dev_manager->getPhysicalDevice(),
                                                mem_requirements.memoryTypeBits,
                                                properties);

  result = vkAllocateMemory(__dev_manager->getLogicalDevice(), &alloc_info, nullptr, &__buffer_memory);
  assert(result == VK_SUCCESS && "failed to allocate buffer memory!");

  // 4. Binding del buffer alla memoria
  vkBindBufferMemory(__dev_manager->getLogicalDevice(), __buffer, __buffer_memory, 0);

  if (data != nullptr)
    loadData(data);
}

void VulkanBuffer::loadData(const void* data)
{
  void* mapped_data;
  vkMapMemory(__dev_manager->getLogicalDevice(), __buffer_memory, 0, __buffer_size, 0, &mapped_data);
  memcpy(mapped_data, data, __buffer_size);

  // Per memoria HOST_VISIBLE e HOST_COHERENT, il flush non � strettamente necessario,
  // ma � buona pratica per chiarezza se si cambiano i tipi di memoria.
  // vkFlushMappedMemoryRanges(...)

  vkUnmapMemory(__dev_manager->getLogicalDevice(), __buffer_memory);
}

void VulkanBuffer::destroy() const
{
  if (__buffer_memory != VK_NULL_HANDLE)
    vkFreeMemory(__dev_manager->getLogicalDevice(), __buffer_memory, nullptr);
  if (__buffer != VK_NULL_HANDLE)
    vkDestroyBuffer(__dev_manager->getLogicalDevice(), __buffer, nullptr);
}

void VulkanBuffer::readMappedData(std::vector<std::byte>& out) const
{
  // A pointer to the mapped memory. We'll use a byte pointer for generic memory access.
  void* mapped_data_ptr = nullptr;

  // Map the buffer's memory to a host-accessible address.
  // The size of the buffer is a member variable, so we don't need to pass it.
  auto result = vkMapMemory(__dev_manager->getLogicalDevice(),
                            __buffer_memory,
                            0,
                            __buffer_size, // Use the member variable for size
                            0,
                            &mapped_data_ptr);
  assert(result == VK_SUCCESS && "Failed to map buffer memory!");

  // Resize the output vector to match the buffer's size.
  out.resize(__buffer_size);

  // Copy the data from the mapped memory to the output vector.
  memcpy(out.data(), mapped_data_ptr, __buffer_size);

  // Unmap the memory after the copy is complete.
  vkUnmapMemory(__dev_manager->getLogicalDevice(), __buffer_memory);
}


uint32_t VulkanBuffer::__findMemoryType(VkPhysicalDevice physical_device,
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
