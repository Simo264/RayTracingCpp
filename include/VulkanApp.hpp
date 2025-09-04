#pragma once

#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>

#include <vector>
#include <string_view>
#include <memory>

class DeviceManager;
class SwapChainManager;
class RenderPass;
class GraphicsPipeline;
class Shader;

/**
 * @brief Basic Concepts
 * 
 * Queue Families:
 * Vulkan organizes different GPU capabilities into queue families. 
 * Each queue family specializes in a specific type of operation, such as graphics, compute, or data transfer. 
 * When initializing the Vulkan application, we must select a physical device and query its queue families to find one that 
 * supports the operations we need (e.g., graphics and compute). 
 * We then retrieve a handle for a queue from that family to submit commands. 
 * A single queue can often support multiple types of operations.  
 * For our application, we need three distinct types of capabilities, which correspond to the three indices in your struct:
 *  1.  Graphics Queue Family: Handles drawing commands and other graphics-related operations. 
 *      Your application's graphics pipeline will use a queue from this family to display the final rendered image.
 *  2.  Compute Queue Family: Manages compute shader workloads. 
 *      This is crucial for your ray tracer, which will run entirely within a compute shader.
 *  3.  Present Queue Family: Handles presenting images to the screen. 
 *      This queue works in tandem with the swap chain to display the final frame.
 * 
 * ====================================================================================================================
 * 
 * Window Surface:
 * The window surface (VkSurfaceKHR) is a crucial abstraction in Vulkan that allows for interaction with a window system. 
 * It represents the abstract surface of a window to which rendered images can be presented. 
 * It's a cross-platform concept that connects the Vulkan instance to a native window (in this case, a GLFW window). 
 * Without a surface, it would be impossible to display any rendering output from Vulkan on a monitor. 
 * 
 * ====================================================================================================================
 * 
 * Swap Chain and Image Views:
 * The swap chain (VkSwapchainKHR) is an essential component for displaying rendered images to a window surface without artifacts 
 * like screen tearing. 
 * It is a queue of renderable images that the application can draw to. 
 * The GPU renders to a "back buffer" while the "front buffer" is displayed on the screen. 
 * Once rendering is complete, the buffers are "swapped," making the new frame visible instantly. 
 * The __swapchain member holds the swap chain handle, and the __swapchain_images vector holds the handles 
 * to the actual image buffers.
 * 
 * An image view (VkImageView) is an object that provides a "view" into a VkImage. 
 * An image is just a block of memory, and the image view provides the necessary metadata, like format and subresource range, 
 * for the GPU to interpret that memory correctly. 
 * Every VkImage within the swap chain must have a corresponding VkImageView to be used for rendering. 
 * The __swapchain_image_views vector stores these views, which are essential for creating the render pass and framebuffers later on.
 * 
 * ====================================================================================================================
 * 
 * Render Pass:
 * Before we can finish creating the pipeline, we need to tell Vulkan about the framebuffer attachments that will be used 
 * while rendering. 
 * We need to specify how many color and depth buffers there will be, how many samples to use for each of them and how 
 * their contents should be handled throughout the rendering operations. 
 * All of this information is wrapped in a render pass object, 
 * 
 * ====================================================================================================================
 * 
 * Graphics Pipeline:
 * The Graphics Pipeline is the core component that converts vertex data into rendered pixels. 
 * It's a highly configurable object that bundles together all rendering states, from shaders to fixed-function operations 
 * like rasterization and color blending.
 * 
 * In the context of your ray tracing application, the graphics pipeline has a very specific purpose: 
 * to draw the final image. 
 * Your ray tracer will compute pixel colors on the GPU using a separate compute pipeline. 
 * The graphics pipeline then takes over to display this computed image. It's configured to draw a simple quad that covers 
 * the entire window, with the fragment shader sampling the ray traced texture to color the pixels. 
 * This minimal setup ensures you can efficiently present the results of your ray tracing algorithm.
 * 
 * ====================================================================================================================
 * 
 */


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

  std::shared_ptr<Shader> __vertex_shader;
  std::shared_ptr<Shader> __fragment_shader;

  std::shared_ptr<DeviceManager> __dev_manager;
  std::shared_ptr<SwapChainManager> __swapchain_manager;
  std::shared_ptr<RenderPass> __render_pass;
  std::shared_ptr<GraphicsPipeline> __graphics_pipeline;
  

  // Private helper functions (for internal use only)
  void __initWindow(uint32_t window_w, uint32_t window_h, std::string_view window_title);
  bool __checkValidationLayerSupport();
  void __createVulkanInstance();
  void __createSurface();
};
