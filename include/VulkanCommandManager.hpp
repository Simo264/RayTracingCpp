#pragma once

#include <vulkan/vulkan.h>
#include <memory>
#include <vector>
#include <functional>

class VulkanDeviceManager;

class VulkanCommandManager
{
public:
  VulkanCommandManager(std::shared_ptr<VulkanDeviceManager> dev_manager);
  ~VulkanCommandManager() = default;

  void cleanup() const;

  void dispatch(uint32_t group_count_x, 
                uint32_t group_count_y, 
                uint32_t group_count_z) const;
  
  // Funzione di utilità per un singolo comando (opzionale)
  void executeSingleTimeCommand(std::function<void(VkCommandBuffer)> func) const;

  // Inizia la registrazione dei comandi
  void beginRecording() const;

  // Termina la registrazione 
  void endRecording() const;
  
  void submitQueue() const;

  void pipelineBarrier(VkPipelineStageFlags src_stage_mask,
                       VkPipelineStageFlags dst_stage_mask,
                       const VkImageMemoryBarrier& image_barrier) const;

  // Ottiene il command buffer per registrare i comandi
  VkCommandBuffer getCommandBuffer() const { return __command_buffer; }

private:
  std::shared_ptr<VulkanDeviceManager> __dev_manager;
  VkCommandPool __command_pool;
  VkCommandBuffer __command_buffer;
};
