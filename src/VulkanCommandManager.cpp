#include "VulkanCommandManager.hpp"
#include "VulkanDeviceManager.hpp"

#include <cassert>
#include <iostream>

VulkanCommandManager::VulkanCommandManager(std::shared_ptr<VulkanDeviceManager> dev_manager) : 
  __dev_manager{ dev_manager },
  __command_pool{ VK_NULL_HANDLE },
  __command_buffer{ VK_NULL_HANDLE }
{
  auto indices = __dev_manager->getQueueFamilyIndices();

  // Creazione del command pool
  auto pool_info = VkCommandPoolCreateInfo{};
  pool_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
  pool_info.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
  pool_info.queueFamilyIndex = indices.compute_family_index.value();

  auto result = vkCreateCommandPool(__dev_manager->getLogicalDevice(), &pool_info, nullptr, &__command_pool);
  assert(result == VK_SUCCESS && "failed to create command pool!");

  // Allocazione del command buffer
  auto alloc_info = VkCommandBufferAllocateInfo{};
  alloc_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
  alloc_info.commandPool = __command_pool;
  alloc_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
  alloc_info.commandBufferCount = 1;

  result = vkAllocateCommandBuffers(__dev_manager->getLogicalDevice(), &alloc_info, &__command_buffer);
  assert(result == VK_SUCCESS && "failed to allocate command buffer!");
}

void VulkanCommandManager::cleanup() const
{
  if (__command_pool != VK_NULL_HANDLE)
    vkDestroyCommandPool(__dev_manager->getLogicalDevice(), __command_pool, nullptr);
}

void VulkanCommandManager::beginRecording() const
{
  // Inizia il command buffer
  auto begin_info = VkCommandBufferBeginInfo{};
  begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
  begin_info.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
  
  auto result = vkBeginCommandBuffer(__command_buffer, &begin_info);
  assert(result == VK_SUCCESS && "failed to begin command buffer!");
}

void VulkanCommandManager::endRecording() const
{
  auto result = vkEndCommandBuffer(__command_buffer);
  assert(result == VK_SUCCESS && "failed to end command buffer!");
}

void VulkanCommandManager::submitQueue() const
{
  // Sottometti il command buffer
  auto submit_info = VkSubmitInfo{};
  submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
  submit_info.commandBufferCount = 1;
  submit_info.pCommandBuffers = &__command_buffer;

  // Sottometti i comandi alla coda e aspetta che siano completati
  auto result = vkQueueSubmit(__dev_manager->getComputeQueue(), 1, &submit_info, VK_NULL_HANDLE);
  assert(result == VK_SUCCESS && "failed vkQueueSubmit!");
  
  result = vkQueueWaitIdle(__dev_manager->getComputeQueue());
  assert(result == VK_SUCCESS && "failed vkQueueWaitIdle!");
}

void VulkanCommandManager::pipelineBarrier(VkPipelineStageFlags src_stage_mask,
                                           VkPipelineStageFlags dst_stage_mask,
                                           const VkImageMemoryBarrier& image_barrier) const
{
  vkCmdPipelineBarrier(__command_buffer,
                       src_stage_mask,
                       dst_stage_mask,
                       0,
                       0, 
                       nullptr,
                       0, 
                       nullptr,
                       1,
                       &image_barrier);
}


void VulkanCommandManager::executeSingleTimeCommand(std::function<void(VkCommandBuffer)> func) const
{
  // 1. Alloca un command buffer
  auto alloc_info = VkCommandBufferAllocateInfo{};
  alloc_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
  alloc_info.commandPool = __command_pool;
  alloc_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
  alloc_info.commandBufferCount = 1;
  auto command_buffer = VkCommandBuffer{};
  vkAllocateCommandBuffers(__dev_manager->getLogicalDevice(), &alloc_info, &command_buffer);

  // 2. Inizia la registrazione
  auto begin_info = VkCommandBufferBeginInfo{};
  begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
  begin_info.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
  vkBeginCommandBuffer(command_buffer, &begin_info);

  // 3. Esegui la funzione con i comandi
  func(command_buffer);

  // 4. Termina e invia
  vkEndCommandBuffer(command_buffer);
  auto submit_info = VkSubmitInfo{};
  submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
  submit_info.commandBufferCount = 1;
  submit_info.pCommandBuffers = &command_buffer;

  auto graphics_queue = __dev_manager->getComputeQueue();
  vkQueueSubmit(graphics_queue, 1, &submit_info, VK_NULL_HANDLE);
  vkQueueWaitIdle(graphics_queue);

  // 5. Libera il command buffer
  vkFreeCommandBuffers(__dev_manager->getLogicalDevice(), __command_pool, 1, &command_buffer);
}

void VulkanCommandManager::dispatch(uint32_t group_count_x,
                                    uint32_t group_count_y,
                                    uint32_t group_count_z) const
{
  vkCmdDispatch(__command_buffer, group_count_x, group_count_y, group_count_z);
}

