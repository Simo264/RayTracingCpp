#pragma once

#include <vulkan/vulkan.h>
#include <vector>

struct QueueFamilyIndices;

struct SwapChainSupportDetails
{
  VkSurfaceCapabilitiesKHR capabilities;
  std::vector<VkSurfaceFormatKHR> formats;
  std::vector<VkPresentModeKHR> present_modes;
};

class SwapChainManager
{
public:
	SwapChainManager(VkDevice logical_device, 
                   VkPhysicalDevice physical_device, 
                   VkSurfaceKHR surface,
                   const QueueFamilyIndices& queue_family_indices,
                   uint32_t window_width, 
                   uint32_t window_height);
	~SwapChainManager() = default;

  VkSwapchainKHR getSwapchain() const {return __swapchain; }

  const auto& getImages() const { return __swapchain_images; }
  const auto& getImageViews() const { return __swapchain_image_views; }
  auto getFormat() const { return __swapchain_image_format; }
  auto getExtent() const { return __swapchain_extent; }

  void cleanup() const;

private:
  void __createSwapchain(const QueueFamilyIndices& queue_family_indices,
                         uint32_t window_width,
                         uint32_t window_height);
  VkSurfaceFormatKHR __chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& available_formats) const;
  VkPresentModeKHR __chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& available_present_modes) const;
  VkExtent2D __chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities,
                                uint32_t window_width,
                                uint32_t window_height) const;
  SwapChainSupportDetails __querySwapChainSupport(VkPhysicalDevice device, VkSurfaceKHR surface) const;

  void __createImageViews();

  VkDevice __logical_device;
  VkPhysicalDevice __physical_device;
  VkSurfaceKHR __surface;

  // Swapchain related data
  VkSwapchainKHR __swapchain;
  std::vector<VkImage> __swapchain_images;
  std::vector<VkImageView> __swapchain_image_views;
  VkFormat __swapchain_image_format;
  VkExtent2D __swapchain_extent;
};


