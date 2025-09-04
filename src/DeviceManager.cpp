#include "DeviceManager.hpp"

#include <iostream>
#include <cassert>
#include <set>
#include <string>

/**
 *	=========================
 *					PUBLIC
 *	=========================
 */


DeviceManager::DeviceManager(VkInstance instance, VkSurfaceKHR surface) :
  __physical_device{ VK_NULL_HANDLE },
  __logical_device{ VK_NULL_HANDLE },
  __graphics_queue{ VK_NULL_HANDLE },
  __compute_queue{ VK_NULL_HANDLE },
  __present_queue{ VK_NULL_HANDLE },
  __queue_family_indices{}
{
  __selectPhysicalDevice(instance, surface);
  __createLogicalDevice();
  // The queues are automatically created along with the logical device.
  // Now you can retrieve the handles for queues.
  vkGetDeviceQueue(__logical_device, __queue_family_indices.graphics_family_index.value(), 0, &__graphics_queue);
  vkGetDeviceQueue(__logical_device, __queue_family_indices.compute_family_index.value(), 0, &__compute_queue);
  vkGetDeviceQueue(__logical_device, __queue_family_indices.present_family_index.value(), 0, &__present_queue);
}

void DeviceManager::cleanup() const
{
  if (__logical_device != VK_NULL_HANDLE)
    vkDestroyDevice(__logical_device, nullptr);
}

/**
 *	=========================
 *					PRIVATE
 *	=========================
 */

void DeviceManager::__selectPhysicalDevice(VkInstance instance, VkSurfaceKHR surface)
{
  auto device_count = 0u;
  vkEnumeratePhysicalDevices(instance, &device_count, nullptr);
  assert(device_count > 0 && "Failed to find GPUs with Vulkan support!\n");

  auto device_list = std::vector<VkPhysicalDevice>(device_count);
  vkEnumeratePhysicalDevices(instance, &device_count, device_list.data());

  for (const auto& device : device_list)
  {
    if (__isDeviceSuitable(device, surface))
    {
      __physical_device = device;
      break;
    }
  }
  assert(__physical_device != VK_NULL_HANDLE && "Failed to find a suitable GPU!");

  auto physical_device_properties = VkPhysicalDeviceProperties{};
  vkGetPhysicalDeviceProperties(__physical_device, &physical_device_properties);
  std::cout << "Selected physical device: " << physical_device_properties.deviceName << std::endl;

  __queue_family_indices = __findQueueFamilies(__physical_device, surface);
}
bool DeviceManager::__isDeviceSuitable(VkPhysicalDevice device, VkSurfaceKHR surface)
{
  // Vulkan does not have the concept of a "default framebuffer", hence it requires an infrastructure that will own 
  // the buffers we will render to before we visualize them on the screen. 
  // This infrastructure is known as the swap chain and must be created explicitly in Vulkan.
  // The swap chain is essentially a queue of images that are waiting to be presented to the screen.
  // Our application will acquire such an image to draw to it, and then return it to the queue.
  // Checking for swap chain support: Not all graphics cards are capable of presenting images directly to a screen.
  auto required_extensions = std::vector<const char*>{ VK_KHR_SWAPCHAIN_EXTENSION_NAME };
  auto extensions_supported = __checkDeviceExtensionSupport(device, required_extensions);
  if (!extensions_supported)
  {
    std::cerr << "Error: The selected physical device does not support the required extensions." << std::endl;
    return false;
  }

  auto indices = __findQueueFamilies(device, surface);
  if (!indices.isComplete())
  {
    std::cerr << "Error: The device does not have a complete set of required queue families (graphics, compute, and/or present)." << std::endl;
    return false;
  }

  // Querying details of swap chain support: just checking if a swap chain is available is not sufficient.
  // Creating a swap chain also involves a lot more settings than instance and device creation, 
  // so we need to query for some more details before we're able to proceed.
  // There are basically three kinds of properties we need to check:
  // 1. Basic surface capabilities (min/max number of images in swap chain, min/max width and height of images)
  // 2. Surface formats (pixel format, color space)
  // 3. Available presentation modes
  auto swap_chain_adequate = __isSwapChainAdequate(device, surface);
  if (!swap_chain_adequate)
  {
    std::cerr << "Error: The device has an inadequate swapchain. It may not support suitable formats or presentation modes." << std::endl;
    return false;
  }
  return true;
}
bool DeviceManager::__checkDeviceExtensionSupport(VkPhysicalDevice device, const std::vector<const char*>& required_extensions)
{
  auto extension_count = 0u;
  vkEnumerateDeviceExtensionProperties(device, nullptr, &extension_count, nullptr);
  auto available_extensions = std::vector<VkExtensionProperties>(extension_count);
  vkEnumerateDeviceExtensionProperties(device, nullptr, &extension_count, available_extensions.data());

  auto required = std::set<std::string>(required_extensions.begin(), required_extensions.end());
  for (const auto& extension : available_extensions)
    required.erase(extension.extensionName);
  return required.empty();
}
bool DeviceManager::__isSwapChainAdequate(VkPhysicalDevice device, VkSurfaceKHR surface) const
{
  auto format_count = 0u;
  vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &format_count, nullptr);
  if (format_count == 0) 
    return false;

  auto present_mode_count = 0u;
  vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &present_mode_count, nullptr);
  if (present_mode_count == 0) 
    return false;

  return true;
}



void DeviceManager::__createLogicalDevice()
{
  // The creation of a logical device involves specifying a bunch of details in structs again, 
  // of which the first one will be VkDeviceQueueCreateInfo.
  auto unique_queue_families = std::set<uint32_t>{
      __queue_family_indices.graphics_family_index.value(),
      __queue_family_indices.compute_family_index.value(),
      __queue_family_indices.present_family_index.value()
  };

  auto queue_create_infos = std::vector<VkDeviceQueueCreateInfo>();

  // Vulkan lets you assign priorities to queues to influence the scheduling of command buffer execution using floating point 
  // numbers between 0.0 and 1.0. This is required even if there is only a single queue.
  constexpr auto queue_priority = 1.0f;
  for (auto queue_family : unique_queue_families)
  {
    auto queue_create_info = VkDeviceQueueCreateInfo{};
    queue_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    queue_create_info.queueFamilyIndex = queue_family;
    queue_create_info.queueCount = 1;
    queue_create_info.pQueuePriorities = &queue_priority;
    queue_create_infos.push_back(queue_create_info);
  }

  // Extensions and features for the device
  const auto device_extensions = std::vector<const char*>{ VK_KHR_SWAPCHAIN_EXTENSION_NAME };

  auto device_features = VkPhysicalDeviceFeatures{};
  // You can enable features here if needed, but for a basic ray tracer, none are required by default.

  auto create_info = VkDeviceCreateInfo{};
  create_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
  create_info.pQueueCreateInfos = queue_create_infos.data();
  create_info.queueCreateInfoCount = queue_create_infos.size();
  create_info.pEnabledFeatures = &device_features;
  create_info.enabledExtensionCount = device_extensions.size();
  create_info.ppEnabledExtensionNames = device_extensions.data();

  auto result = vkCreateDevice(__physical_device, &create_info, nullptr, &__logical_device);
  assert(result == VK_SUCCESS && "Failed to create logical device!");
  std::cout << "Logical device created successfully" << std::endl;
}

QueueFamilyIndices DeviceManager::__findQueueFamilies(VkPhysicalDevice device, VkSurfaceKHR surface) const
{
  auto queue_family_count = 0u;
  vkGetPhysicalDeviceQueueFamilyProperties(device, &queue_family_count, nullptr);
  auto queue_families = std::vector<VkQueueFamilyProperties>(queue_family_count);
  vkGetPhysicalDeviceQueueFamilyProperties(device, &queue_family_count, queue_families.data());

  auto indices = QueueFamilyIndices{};
  for (auto i = 0u; i < queue_families.size(); i++)
  {
    const auto& family_prop = queue_families.at(i);

    // Check for graphics capability
    if (family_prop.queueFlags & VK_QUEUE_GRAPHICS_BIT)
      indices.graphics_family_index = i;
    // Check for compute capability
    if (family_prop.queueFlags & VK_QUEUE_COMPUTE_BIT)
      indices.compute_family_index = i;

    // Check for presentation support
    auto present_support = VkBool32{ false };
    vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, &present_support);
    if (present_support)
      indices.present_family_index = i;

    // if we've found both, we can stop searching
    if (indices.isComplete())
      break;
  }
  return indices;
}

