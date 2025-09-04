#include "SwapChainManager.hpp"
#include "QueueFamilyIndices.hpp"

#include <algorithm>
#include <array>
#include <cassert>
#include <iostream>

/**
 *	=========================
 *					PUBLIC
 *	=========================
 */


SwapChainManager::SwapChainManager(VkDevice logical_device,
                                   VkPhysicalDevice physical_device,
                                   VkSurfaceKHR surface,
                                   const QueueFamilyIndices& queue_family_indices,
                                   uint32_t window_width,
                                   uint32_t window_height) :
	__logical_device{ logical_device },
	__physical_device{ physical_device },
	__surface{ surface },
	__swapchain{},
  __swapchain_images{},
  __swapchain_image_views{},
  __swapchain_image_format{},
  __swapchain_extent{}
{
	__createSwapchain(queue_family_indices, window_width, window_height);
  __createImageViews();
}

void SwapChainManager::cleanup() const
{
	for (auto image_view : __swapchain_image_views)
		vkDestroyImageView(__logical_device, image_view, nullptr);
	vkDestroySwapchainKHR(__logical_device, __swapchain, nullptr);
}

/**
 *	=========================
 *					PRIVATE
 *	=========================
 */


void SwapChainManager::__createSwapchain(const QueueFamilyIndices& queue_family_indices,
                                         uint32_t window_width,
                                         uint32_t window_height)
{
  // First, query the device for its swapchain capabilities
  auto swap_chain_support = __querySwapChainSupport(__physical_device, __surface);
  // Then, choose the optimal settings based on the queried capabilities
  auto surface_format = __chooseSwapSurfaceFormat(swap_chain_support.formats);
  auto present_mode = __chooseSwapPresentMode(swap_chain_support.present_modes);
  auto extent = __chooseSwapExtent(swap_chain_support.capabilities, window_width, window_height);

  // Determine the number of images in the swapchain
  auto image_count = swap_chain_support.capabilities.minImageCount + 1;
  if (swap_chain_support.capabilities.maxImageCount > 0u && image_count > swap_chain_support.capabilities.maxImageCount)
    image_count = swap_chain_support.capabilities.maxImageCount;

  // Fill in the create info struct for the swapchain
  auto create_info = VkSwapchainCreateInfoKHR{};
  create_info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
  create_info.surface = __surface;
  create_info.minImageCount = image_count;
  create_info.imageFormat = surface_format.format;
  create_info.imageColorSpace = surface_format.colorSpace;
  create_info.imageExtent = extent;
  create_info.imageArrayLayers = 1; // Always 1 for non-stereoscopic 3D applications
  create_info.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT; // The images will be used as a render target

  // Handle queue sharing if the graphics and present queues are from different families
  auto indices = std::array<uint32_t, 2>{
    queue_family_indices.graphics_family_index.value(),
    queue_family_indices.present_family_index.value()
  };
  if (queue_family_indices.graphics_family_index != queue_family_indices.present_family_index)
  {
    create_info.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
    create_info.queueFamilyIndexCount = 2;
    create_info.pQueueFamilyIndices = indices.data();
  }
  else
  {
    create_info.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    create_info.queueFamilyIndexCount = 0; // Optional
    create_info.pQueueFamilyIndices = nullptr; // Optional
  }
  create_info.preTransform = swap_chain_support.capabilities.currentTransform;
  create_info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR; // Do not blend with other windows
  create_info.presentMode = present_mode;
  create_info.clipped = VK_TRUE; // Ignore pixels that are obscured by other windows
  create_info.oldSwapchain = VK_NULL_HANDLE; // Used for recreating the swapchain

  auto result = vkCreateSwapchainKHR(__logical_device, &create_info, nullptr, &__swapchain);
  assert(result == VK_SUCCESS && "Failed to create swap chain!");

  // Now, retrieve the handles for the images created by the swapchain
  auto swapchain_image_count = 0u;
  vkGetSwapchainImagesKHR(__logical_device, __swapchain, &swapchain_image_count, nullptr);
  __swapchain_images = std::vector<VkImage>(swapchain_image_count);
  vkGetSwapchainImagesKHR(__logical_device, __swapchain, &swapchain_image_count, __swapchain_images.data());

  // You can also store the format and extent for later use
  __swapchain_image_format = surface_format.format;
  __swapchain_extent = extent;

  std::cout << "Swap chain created successfully!" << std::endl;
}
VkSurfaceFormatKHR SwapChainManager::__chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& available_formats) const
{
  for (const auto& available_format : available_formats)
  {
    if (available_format.format == VK_FORMAT_B8G8R8A8_SRGB && available_format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
      return available_format;
  }

  // If the ideal format isn't available, just take the first one.
  return available_formats.at(0);
}
VkPresentModeKHR SwapChainManager::__chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& available_present_modes) const
{
  for (const auto& present_mode : available_present_modes)
  {
    if (present_mode == VK_PRESENT_MODE_FIFO_KHR)
      return present_mode;
  }

  // VK_PRESENT_MODE_FIFO_KHR is guaranteed to be available by the Vulkan specification.
  // However, a fallback is included for defensive programming.
  return VK_PRESENT_MODE_FIFO_KHR;
}
VkExtent2D SwapChainManager::__chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities,
                                                uint32_t window_width,
                                                uint32_t window_height) const
{
  if (capabilities.currentExtent.width != UINT32_MAX)
    return capabilities.currentExtent;

  auto actual_extent = VkExtent2D{ window_width, window_height };
  actual_extent.width = std::clamp(actual_extent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
  actual_extent.height = std::clamp(actual_extent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);
  return actual_extent;
}
SwapChainSupportDetails SwapChainManager::__querySwapChainSupport(VkPhysicalDevice device, VkSurfaceKHR surface) const
{
  auto details = SwapChainSupportDetails{};
  // Get surface capabilities
  vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface, &details.capabilities);

  // Get surface formats
  auto format_count = 0u;
  vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &format_count, nullptr);
  if (format_count != 0)
  {
    details.formats.resize(format_count);
    vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &format_count, details.formats.data());
  }

  // Get presentation modes
  auto present_mode_count = 0u;
  vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &present_mode_count, nullptr);
  if (present_mode_count != 0)
  {
    details.present_modes.resize(present_mode_count);
    vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &present_mode_count, details.present_modes.data());
  }

  return details;
}


void SwapChainManager::__createImageViews()
{
  __swapchain_image_views.resize(__swapchain_images.size());
  for (auto i = 0; i < __swapchain_images.size(); i++)
  {
    auto create_info = VkImageViewCreateInfo{};
    create_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    create_info.image = __swapchain_images[i];

    // Specifica come l'immagine deve essere interpretata
    create_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
    create_info.format = __swapchain_image_format;

    // Mappa i canali di colore (RGB A)
    create_info.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
    create_info.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
    create_info.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
    create_info.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;

    // Specifica l'intervallo di subrisorse dell'immagine
    create_info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    create_info.subresourceRange.baseMipLevel = 0;
    create_info.subresourceRange.levelCount = 1;
    create_info.subresourceRange.baseArrayLayer = 0;
    create_info.subresourceRange.layerCount = 1;

    auto result = vkCreateImageView(__logical_device, &create_info, nullptr, &__swapchain_image_views[i]);
    assert(result == VK_SUCCESS && "failed to create image views!");
  }
}
