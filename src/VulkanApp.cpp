#include "VulkanApp.hpp"

#include <array>
#include <algorithm>
#include <iostream>
#include <cassert>

// Validation layers for debugging (optional)
static const auto g_validation_layers = std::vector<const char*>{ "VK_LAYER_KHRONOS_validation" };


/**
 *	=========================
 *					PUBLIC
 *	=========================
 */


VulkanApp::VulkanApp(uint32_t window_w, uint32_t window_h, std::string_view window_title)
{
	// 1. Initialize GLFW and create a window
  __initWindow(window_w, window_h, window_title);

  // 2. Create the Vulkan instance
  __createVulkanInstance();

  // 3. Create the window surface
  __createSurface();
  
  // 4. Selecting a physical device
  __selectPhysicalDevice();

  // 5. Set up logical device
  __createLogicalDevice();
  // The queues are automatically created along with the logical device.
  // Now you can retrieve the handles for queues.
  vkGetDeviceQueue(__logical_device, __queue_family_indices.graphics_family_index.value(), 0, &__graphics_queue);
  vkGetDeviceQueue(__logical_device, __queue_family_indices.compute_family_index.value(), 0, &__compute_queue);
  vkGetDeviceQueue(__logical_device, __queue_family_indices.present_family_index.value(), 0, &__present_queue);

  // 6. Creating the swapchain
  __createSwapchain();
}

VulkanApp::~VulkanApp()
{
  // Cleanup
  vkDestroySwapchainKHR(__logical_device, __swapchain, nullptr);
  vkDestroyDevice(__logical_device, nullptr);
  vkDestroySurfaceKHR(__vk_instance, __surface, nullptr);
  vkDestroyInstance(__vk_instance, nullptr);
  glfwDestroyWindow(__window);
  glfwTerminate();
}

void VulkanApp::run()
{  
  // Main loop
  while (!glfwWindowShouldClose(__window))
  {
    glfwPollEvents();
  }
}

/**
 *	=========================
 *					PRIVATE
 *	=========================
 */

void VulkanApp::__initWindow(uint32_t window_w, uint32_t window_h, std::string_view window_title)
{
  auto result = glfwInit();
  assert(result == GLFW_TRUE && "Failed to initialize GLFW");
  
  glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
  __window = glfwCreateWindow(window_w, window_h, window_title.data(), nullptr, nullptr);
  assert(__window != nullptr && "Failed to create GLFW window");

  std::cout << "GLFW Window created successfully" << std::endl;
}

bool VulkanApp::__checkValidationLayerSupport()
{
  auto layer_count = 0u;
  vkEnumerateInstanceLayerProperties(&layer_count, nullptr);
  auto available_layers = std::vector<VkLayerProperties>(layer_count);
  vkEnumerateInstanceLayerProperties(&layer_count, available_layers.data());

  for (std::string_view layer_name : g_validation_layers)
  {
    auto result = std::find_if(available_layers.begin(), available_layers.end(), [&](const VkLayerProperties& layerProperties) {
      return layer_name == layerProperties.layerName;
    });

    if (result == available_layers.end())
      return false;
  }
  return true;
}
void VulkanApp::__createVulkanInstance()
{
  // The Vulkan API is designed around the idea of minimal driver overhead and one of the manifestations of that goal is that 
  // there is very limited error checking in the API by default. 
  // Vulkan introduces an elegant system for this known as validation layers.
  // Validation layers are optional components that hook into Vulkan function calls to apply additional operations.
  // Using the validation layers is the best way to avoid your application breaking on different drivers by accidentally 
  // relying on undefined behavior.
  // Validation layers can only be used if they have been installed onto the system.
  if (!__checkValidationLayerSupport())
  {
    std::cerr << "Validation layers requested, but not available!" << std::endl;
    exit(1);
  }

  // Now, to create an instance we'll first have to fill in a struct with some information about our application. 
  // This data is technically optional, but it may provide some useful information to the driver in order to optimize 
  // our specific application
  auto app_info = VkApplicationInfo{};
  app_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
  app_info.pApplicationName = "RayTracingVulkan_Compute";
  app_info.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
  app_info.pEngineName = "NoEngine";
  app_info.engineVersion = VK_MAKE_VERSION(1, 0, 0);
  app_info.apiVersion = VK_API_VERSION_1_3;

  // A lot of information in Vulkan is passed through structs instead of function parameters and we'll have to fill 
  // in one more struct to provide sufficient information for creating an instance.
  // This next struct is not optional and tells the Vulkan driver which global extensions and validation layers 
  // we want to use. 
  // Global here means that they apply to the entire program and not a specific device
  auto glfw_extension_count = 0u;
  auto glfw_extensions = glfwGetRequiredInstanceExtensions(&glfw_extension_count);
  auto extensions = std::vector<const char*>(glfw_extensions, glfw_extensions + glfw_extension_count);

  auto create_info = VkInstanceCreateInfo{};
  create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
  create_info.pApplicationInfo = &app_info;
  create_info.enabledExtensionCount = extensions.size();
  create_info.ppEnabledExtensionNames = extensions.data();
  create_info.enabledLayerCount = g_validation_layers.size();
  create_info.ppEnabledLayerNames = g_validation_layers.data();
  auto result = vkCreateInstance(&create_info, nullptr, &__vk_instance);
  assert(result == VK_SUCCESS && "Failed to create instance!");
  std::cout << "VkInstance created successfully" << std::endl;
}
void VulkanApp::__createSurface()
{
  __surface = VkSurfaceKHR{};

  auto result = glfwCreateWindowSurface(__vk_instance, __window, nullptr, &__surface);
  assert(result == VK_SUCCESS && "Failed to create window surface!\n");
  std::cout << "Window surface created successfully" << std::endl;
}

void VulkanApp::__selectPhysicalDevice()
{
  __physical_device = VK_NULL_HANDLE;

  auto device_count = 0u;
  vkEnumeratePhysicalDevices(__vk_instance, &device_count, nullptr);
  assert(device_count > 0 && "Failed to find GPUs with Vulkan support!\n");

  auto device_list = std::vector<VkPhysicalDevice>(device_count);
  vkEnumeratePhysicalDevices(__vk_instance, &device_count, device_list.data());

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

  __queue_family_indices = __findQueueFamilies(__physical_device);
}
bool VulkanApp::__isDeviceSuitable(VkPhysicalDevice device)
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

  auto indices = __findQueueFamilies(device);
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
  auto swap_chain_support = __querySwapChainSupport(device);
  auto swap_chain_adequate = !swap_chain_support.formats.empty() && !swap_chain_support.present_modes.empty();
  if (!swap_chain_adequate)
  {
    std::cerr << "Error: The device has an inadequate swapchain. It may not support suitable formats or presentation modes." << std::endl;
    return false;
  }
  return true;
}
bool VulkanApp::__checkDeviceExtensionSupport(VkPhysicalDevice device, const std::vector<const char*>& required_extensions)
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
QueueFamilyIndices VulkanApp::__findQueueFamilies(VkPhysicalDevice device) const
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
    vkGetPhysicalDeviceSurfaceSupportKHR(device, i, __surface, &present_support);
    if (present_support)
      indices.present_family_index = i;

    // if we've found both, we can stop searching
    if (indices.isComplete())
      break;
  }
  return indices;
}
SwapChainSupportDetails VulkanApp::__querySwapChainSupport(VkPhysicalDevice device) const
{
  auto details = SwapChainSupportDetails{};
  // Get surface capabilities
  vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, __surface, &details.capabilities);

  // Get surface formats
  auto format_count = 0u;
  vkGetPhysicalDeviceSurfaceFormatsKHR(device, __surface, &format_count, nullptr);
  if (format_count != 0)
  {
    details.formats.resize(format_count);
    vkGetPhysicalDeviceSurfaceFormatsKHR(device, __surface, &format_count, details.formats.data());
  }

  // Get presentation modes
  auto present_mode_count = 0u;
  vkGetPhysicalDeviceSurfacePresentModesKHR(device, __surface, &present_mode_count, nullptr);
  if (present_mode_count != 0)
  {
    details.present_modes.resize(present_mode_count);
    vkGetPhysicalDeviceSurfacePresentModesKHR(device, __surface, &present_mode_count, details.present_modes.data());
  }

  return details;
}

void VulkanApp::__createLogicalDevice()
{
  __logical_device = VK_NULL_HANDLE;
  
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


void VulkanApp::__createSwapchain()
{
  __swapchain = VkSwapchainKHR{};

  // First, query the device for its swapchain capabilities
  auto swap_chain_support = __querySwapChainSupport(__physical_device);
  // Then, choose the optimal settings based on the queried capabilities
  auto surface_format = __chooseSwapSurfaceFormat(swap_chain_support.formats);
  auto present_mode = __chooseSwapPresentMode(swap_chain_support.present_modes);
  auto extent = __chooseSwapExtent(swap_chain_support.capabilities); // Pass your window size here

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
  auto queue_family_indices = std::array<uint32_t, 2>{ 
    __queue_family_indices.graphics_family_index.value(),
    __queue_family_indices.present_family_index.value()
  };
  if (__queue_family_indices.graphics_family_index != __queue_family_indices.present_family_index)
  {
    create_info.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
    create_info.queueFamilyIndexCount = 2;
    create_info.pQueueFamilyIndices = queue_family_indices.data();
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
  // auto __swapchain_image_format = surfaceFormat.format;
  // auto __swapchain_extent = extent;
  
  std::cout << "Swap chain created successfully!" << std::endl;
}
VkSurfaceFormatKHR VulkanApp::__chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& available_formats)
{
  for (const auto& available_format : available_formats)
  {
    if (available_format.format == VK_FORMAT_B8G8R8A8_SRGB && available_format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
      return available_format;
  }

  // If the ideal format isn't available, just take the first one.
  return available_formats.at(0);
}
VkPresentModeKHR VulkanApp::__chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& available_present_modes)
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
VkExtent2D VulkanApp::__chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities)
{
  if (capabilities.currentExtent.width != UINT32_MAX)
    return capabilities.currentExtent;

  auto width = 0, height = 0;
  glfwGetFramebufferSize(__window, &width, &height);

  auto actual_extent = VkExtent2D{
    static_cast<uint32_t>(width),
    static_cast<uint32_t>(height)
  };

  actual_extent.width = std::clamp(actual_extent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
  actual_extent.height = std::clamp(actual_extent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);
  return actual_extent;
}

