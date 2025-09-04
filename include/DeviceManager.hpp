#pragma once

#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>

#include "QueueFamilyIndices.hpp"

#include <vector>

class DeviceManager
{
public:
  DeviceManager(VkInstance instance, VkSurfaceKHR surface);
	~DeviceManager() = default;

  auto getPhysicalDevice() const { return __physical_device; }
  auto getLogicalDevice() const { return __logical_device; }
  auto getQueueFamilyIndices() const { return __queue_family_indices; }

  auto getGraphicsQueue() const { return __graphics_queue; }
  auto getComputeQueue() const { return __compute_queue; }
  auto getPresentQueue() const { return __present_queue; }

  void cleanup() const;

private:
  void __selectPhysicalDevice(VkInstance instance, VkSurfaceKHR surface);
  bool __isDeviceSuitable(VkPhysicalDevice device, VkSurfaceKHR surface);
  bool __checkDeviceExtensionSupport(VkPhysicalDevice device, const std::vector<const char*>& required_extensions);
  bool __isSwapChainAdequate(VkPhysicalDevice device, VkSurfaceKHR surface) const;

  void __createLogicalDevice();

  QueueFamilyIndices __findQueueFamilies(VkPhysicalDevice device, VkSurfaceKHR surface) const;

	VkPhysicalDevice __physical_device;
	VkDevice __logical_device;
  QueueFamilyIndices __queue_family_indices;

  // Queues and queue family indices
  VkQueue __graphics_queue;
  VkQueue __compute_queue;
  VkQueue __present_queue;
};
