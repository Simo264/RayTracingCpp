#pragma once

#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>
#include <vector>
#include <optional>
#include <set>
#include <string_view>

// Queue families
// Almost every operation in Vulkan, anything from drawing to uploading textures, 
// requires commands to be submitted to a queue.
// There are different types of queues that originate from different queue families and each family of queues allows only a 
// subset of commands. 
// For example, there could be a queue family that only allows processing of compute commands or 
// one that only allows memory transfer related commands.
struct QueueFamilyIndices
{
  std::optional<uint32_t> compute_family_index;
  std::optional<uint32_t> graphics_family_index;
  std::optional<uint32_t> present_family_index;

  bool isComplete() const
  {
    return graphics_family_index.has_value() &&
      compute_family_index.has_value() &&
      present_family_index.has_value();
  }
};
struct SwapChainSupportDetails
{
  VkSurfaceCapabilitiesKHR capabilities;
  std::vector<VkSurfaceFormatKHR> formats;
  std::vector<VkPresentModeKHR> present_modes;
};

class VulkanApp
{
public:
  VulkanApp(uint32_t window_w, uint32_t window_h, std::string_view window_title);
  ~VulkanApp();

  void run();

private:
  GLFWwindow* __window;
  VkInstance __vk_instance;
  VkSurfaceKHR __surface;
  VkPhysicalDevice __physical_device;
  VkDevice __logical_device;

  // Queues and queue family indices
  VkQueue __graphics_queue;
  VkQueue __compute_queue;
  VkQueue __present_queue;
  QueueFamilyIndices __queue_family_indices;

  // Swapchain related data
  VkSwapchainKHR __swapchain;
  std::vector<VkImage> __swapchain_images;
  VkFormat __swapchain_image_format;
  VkExtent2D __swapchain_extent;


  // Private helper functions (for internal use only)
  void __initWindow(uint32_t window_w, uint32_t window_h, std::string_view window_title);
  bool __checkValidationLayerSupport();
  void __createVulkanInstance();
  void __createSurface();
  
  void __selectPhysicalDevice();
  bool __isDeviceSuitable(VkPhysicalDevice device);
  bool __checkDeviceExtensionSupport(VkPhysicalDevice device, const std::vector<const char*>& required_extensions);
  QueueFamilyIndices __findQueueFamilies(VkPhysicalDevice device) const;
  SwapChainSupportDetails __querySwapChainSupport(VkPhysicalDevice device) const;

  void __createLogicalDevice();

  void __createSwapchain();
  VkSurfaceFormatKHR __chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& available_formats);
  VkPresentModeKHR __chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& available_present_modes);
  VkExtent2D __chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);
};
