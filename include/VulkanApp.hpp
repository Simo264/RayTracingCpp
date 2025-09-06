#pragma once

#include <vulkan/vulkan.h>
#include <glm/glm.hpp>
#include <memory>
#include <string_view>

class VulkanDeviceManager;
class VulkanDescriptorManager;
class VulkanComputePipeline;
class VulkanCommandManager;
class VulkanBuffer;
class VulkanImage;
class VulkanShader;


/**
 * @brief Basic Concepts
 * 
 * Compute shader:
 * Doing computationally expensive calculations on the GPU has several advantages. 
 * The most obvious one is offloading work from the CPU. Another one is not requiring moving data between the 
 * CPU's main memory and the GPU's memory. 
 * All of the data can stay on the GPU without having to wait for slow transfers from main memory.
 * Aside from these, GPUs are heavily parallelized with some of them having tens of thousands of small compute units. 
 * This often makes them a better fit for highly parallel workflows than a CPU with a few large compute units.
 * 
 * It's important to know that compute is completely separated from the graphics part of the pipeline. 
 * With the compute shader stage being detached from the graphics pipeline we'll be able to use it anywhere we see fit. 
 * This is very different from e.g. the fragment shader which is always applied to the transformed output of the vertex shader.
 * 
 * ====================================================================================================================
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
 * Shader storage buffer objects (SSBO):
 * A shader storage buffer (SSBO) allows shaders to read from and write to a buffer. 
 * Using these is similar to using uniform buffer objects. 
 * The biggest differences are that you can alias other buffer types to SSBOs and that they can be arbitrarily large.
 * 
 * ====================================================================================================================
 * 
 * Command pools:
 * Commands in Vulkan, like drawing operations and memory transfers, are not executed directly using function calls. 
 * You have to record all of the operations you want to perform in command buffer objects. 
 * The advantage of this is that when we are ready to tell the Vulkan what we want to do, all of the commands are submitted 
 * together and Vulkan can more efficiently process the commands since all of them are available together. 
 * In addition, this allows command recording to happen in multiple threads if so desired.
 */


class VulkanApp
{
public:
  VulkanApp(uint32_t image_width, uint32_t image_height, std::string_view image_output_name);
  ~VulkanApp();

  void run();

private:
  uint32_t __image_width;
  uint32_t __image_height;
  std::string_view __image_output_name;
  VkInstance __vk_instance;

  // Compute shader
  std::shared_ptr<VulkanShader> __compute_shader;

  // Output image
  std::shared_ptr<VulkanImage> __compute_output_image;

  // Device manager
  std::shared_ptr<VulkanDeviceManager> __dev_manager;

  // Descriptor manager
  std::shared_ptr<VulkanDescriptorManager> __descriptor_manager;
  
  // Compute shader pipeline
  std::shared_ptr<VulkanComputePipeline> __compute_pipeline;

  // Command manager
  std::shared_ptr<VulkanCommandManager> __command_manager;

  // Scene objects
  std::shared_ptr<VulkanBuffer> __sphere_buffer;
  std::shared_ptr<VulkanBuffer> __plane_buffer;
  //std::shared_ptr<VulkanBuffer> __camera_buffer;
  

  // Private helper functions (for internal use only)
  bool __checkValidationLayerSupport();
  void __createVulkanInstance();
};
