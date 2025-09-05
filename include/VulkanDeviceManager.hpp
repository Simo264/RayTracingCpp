#pragma once

#include <vulkan/vulkan.h>

#include "QueueFamilyIndices.hpp"

class VulkanDeviceManager
{
public:
  VulkanDeviceManager(VkInstance instance);
	~VulkanDeviceManager() = default;

  auto getPhysicalDevice() const { return __physical_device; }
  auto getLogicalDevice() const { return __logical_device; }
  auto getQueueFamilyIndices() const { return __queue_family_indices; }

  auto getComputeQueue() const { return __compute_queue; }

  void cleanup() const;

private:
  void __selectPhysicalDevice(VkInstance instance);
  bool __isDeviceSuitable(VkPhysicalDevice device);
  void __createLogicalDevice();

  QueueFamilyIndices __findQueueFamilies(VkPhysicalDevice device) const;

	VkPhysicalDevice __physical_device;
	VkDevice __logical_device;
  QueueFamilyIndices __queue_family_indices;

  VkQueue __compute_queue;
};
