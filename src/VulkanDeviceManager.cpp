#include "VulkanDeviceManager.hpp"

#include <iostream>
#include <vector>
#include <cassert>
#include <set>
#include <string>

/**
 *	=========================
 *					PUBLIC
 *	=========================
 */


VulkanDeviceManager::VulkanDeviceManager(VkInstance instance) :
  __physical_device{ VK_NULL_HANDLE },
  __logical_device{ VK_NULL_HANDLE },
  __compute_queue{ VK_NULL_HANDLE },
  __queue_family_indices{}
{
  __selectPhysicalDevice(instance);
  __queue_family_indices = __findQueueFamilies(__physical_device);

  __createLogicalDevice();
  
  // The queues are automatically created along with the logical device.
  // Now you can retrieve the handles for queues.
  vkGetDeviceQueue(__logical_device, __queue_family_indices.compute_family_index.value(), 0, &__compute_queue);
}

void VulkanDeviceManager::cleanup() const
{
  if (__logical_device != VK_NULL_HANDLE)
    vkDestroyDevice(__logical_device, nullptr);
}

/**
 *	=========================
 *					PRIVATE
 *	=========================
 */

void VulkanDeviceManager::__selectPhysicalDevice(VkInstance instance)
{
  auto device_count = 0u;
  vkEnumeratePhysicalDevices(instance, &device_count, nullptr);
  assert(device_count > 0 && "Failed to find GPUs with Vulkan support!\n");

  auto device_list = std::vector<VkPhysicalDevice>(device_count);
  vkEnumeratePhysicalDevices(instance, &device_count, device_list.data());

  for (const auto& device : device_list)
  {
    if (__isDeviceSuitable(device))
    {
      __physical_device = device;
      break;
    }
  }
  assert(__physical_device != VK_NULL_HANDLE && "Failed to find a suitable GPU!");

  auto physical_device_properties = VkPhysicalDeviceProperties{};
  vkGetPhysicalDeviceProperties(__physical_device, &physical_device_properties);
  std::cout << "Selected physical device: " << physical_device_properties.deviceName << std::endl;
}
bool VulkanDeviceManager::__isDeviceSuitable(VkPhysicalDevice device)
{
  auto indices = __findQueueFamilies(device);
  if (!indices.isComplete())
  {
    std::cerr << "Error: The device does not have a compute queue family." << std::endl;
    return false;
  }

  // Opzionale: verifica che supporti SSBO e storage images
  auto supported_features = VkPhysicalDeviceFeatures{};
  vkGetPhysicalDeviceFeatures(device, &supported_features);
  if (!supported_features.shaderStorageImageWriteWithoutFormat)
  {
    std::cerr << "Error: !supported_features.shaderStorageImageWriteWithoutFormat" << std::endl;
    return false; // Richiesto per imageStore senza format qualifier
  }

  return true;
}

void VulkanDeviceManager::__createLogicalDevice()
{
  auto queue_create_infos = std::vector<VkDeviceQueueCreateInfo>();

  // Vulkan lets you assign priorities to queues to influence the scheduling of command buffer execution using floating point 
  // numbers between 0.0 and 1.0. This is required even if there is only a single queue.
  constexpr auto queue_priority = 1.0f;
  auto queue_create_info = VkDeviceQueueCreateInfo{};
  queue_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
  queue_create_info.queueFamilyIndex = __queue_family_indices.compute_family_index.value();
  queue_create_info.queueCount = 1;
  queue_create_info.pQueuePriorities = &queue_priority;
  queue_create_infos.push_back(queue_create_info);

  auto device_features = VkPhysicalDeviceFeatures{};
  // You can enable features here if needed, but for a basic ray tracer, none are required by default.
  auto create_info = VkDeviceCreateInfo{};
  create_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
  create_info.pQueueCreateInfos = queue_create_infos.data();
  create_info.queueCreateInfoCount = queue_create_infos.size();
  create_info.pEnabledFeatures = &device_features;
  create_info.enabledExtensionCount = 0;
  create_info.ppEnabledExtensionNames = nullptr;

  auto result = vkCreateDevice(__physical_device, &create_info, nullptr, &__logical_device);
  assert(result == VK_SUCCESS && "Failed to create logical device!");
  std::cout << "Logical device created successfully" << std::endl;
}

QueueFamilyIndices VulkanDeviceManager::__findQueueFamilies(VkPhysicalDevice device) const
{
  auto queue_family_count = 0u;
  vkGetPhysicalDeviceQueueFamilyProperties(device, &queue_family_count, nullptr);
  auto queue_families = std::vector<VkQueueFamilyProperties>(queue_family_count);
  vkGetPhysicalDeviceQueueFamilyProperties(device, &queue_family_count, queue_families.data());

  auto indices = QueueFamilyIndices{};
  for (auto i = 0u; i < queue_families.size(); i++)
  {
    if (queue_families.at(i).queueFlags & VK_QUEUE_COMPUTE_BIT)
    {
      indices.compute_family_index = i;
      break;
    }
  }
  return indices;
}

