#include "VulkanApp.hpp"
#include "DeviceManager.hpp"
#include "SwapChainManager.hpp"
#include "RenderPass.hpp"
#include "GraphicsPipeline.hpp"
#include "Paths.hpp"
#include "Shader.hpp"

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
  
  // 4-5. Selecting a physical device and set up logical device
  __dev_manager = std::make_shared<DeviceManager>(__vk_instance, __surface);

  // 6-7. Creating the Swap Chain and Image Views
  __swapchain_manager = std::make_shared<SwapChainManager>(__dev_manager->getLogicalDevice(),
                                                           __dev_manager->getPhysicalDevice(),
                                                           __surface,
                                                           __dev_manager->getQueueFamilyIndices(),
                                                           window_w,
                                                           window_h);

  // 8. Create Render Pass
  __render_pass = std::make_shared<RenderPass>(__dev_manager->getLogicalDevice(), 
                                               __swapchain_manager->getFormat());

  // 9. Create graphics pipeline
  auto shaders_dir = getShadersDir();
  __vertex_shader = std::make_shared<Shader>(shaders_dir / "shader.vert.spirv", __dev_manager->getLogicalDevice());
  __fragment_shader = std::make_shared<Shader>(shaders_dir / "shader.frag.spirv", __dev_manager->getLogicalDevice());
  __graphics_pipeline = std::make_shared<GraphicsPipeline>(__vertex_shader,
                                                           __fragment_shader,
                                                           __dev_manager->getLogicalDevice(),
                                                           __render_pass->getVkRenderPass(),
                                                           __swapchain_manager->getExtent());
}

VulkanApp::~VulkanApp()
{
  // 1. Destroy all device-level objects
  __graphics_pipeline->destroy();
  __render_pass->destroy();
  __swapchain_manager->cleanup();

  // 2. Destroy the shader objects (before the device)
  __vertex_shader->destroy();
  __fragment_shader->destroy();

  // 3. Destroy the logical device
  __dev_manager->cleanup();

  // 4. Destroy instance-level objects
  vkDestroySurfaceKHR(__vk_instance, __surface, nullptr);
  vkDestroyInstance(__vk_instance, nullptr);

  // 5. Destroy the window
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


