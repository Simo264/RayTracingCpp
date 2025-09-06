#include "VulkanApp.hpp"
#include "VulkanDeviceManager.hpp"
#include "VulkanDescriptorManager.hpp"
#include "VulkanComputePipeline.hpp"
#include "VulkanCommandManager.hpp"
#include "VulkanBuffer.hpp"
#include "VulkanImage.hpp"
#include "VulkanShader.hpp"

#include "Paths.hpp"
#include "ImageLoader.hpp"

#include <array>
#include <algorithm>
#include <iostream>
#include <cassert>
#include <limits>
#include <vector>
#include <random>

// Validation layers for debugging (optional)
static const auto g_validation_layers = std::vector<const char*>{ "VK_LAYER_KHRONOS_validation" };
static const auto g_instance_extensions = std::vector<const char*>{ VK_EXT_DEBUG_UTILS_EXTENSION_NAME };
static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT severity,
                                                    VkDebugUtilsMessageTypeFlagsEXT type,
                                                    const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
                                                    void* pUserData)
{
  std::cerr << "Validation layer: " << pCallbackData->pMessage << std::endl;
  return VK_FALSE;
}

struct VulkanMaterial
{
  // Recommended: Use vec4 for better alignment and simplicity
  glm::vec4 color_scale;
  glm::vec4 emission_scale;
  float roughness_scale;
  int material_id; // 0:Matte 1:Metal 2:Emissive
  float _pad[2];
};
struct VulkanSphere
{
  glm::vec3 center;
  float radius;
  VulkanMaterial material;
};
struct VulkanPlane
{
  glm::vec4 position;
  glm::vec4 normal;
  glm::vec2 size;
  float _pad[2];
  VulkanMaterial material;
};

/**
 *	=========================
 *					PUBLIC
 *	=========================
 */

VulkanApp::VulkanApp(uint32_t image_width,
                     uint32_t image_height,
                     std::string_view image_output_name) :
  __image_width{ image_width },
  __image_height{ image_height },
  __image_output_name{ image_output_name }
{
  // Create the Vulkan instance
  __createVulkanInstance();

  // Selecting a physical device and set up logical device
  __dev_manager = std::make_shared<VulkanDeviceManager>(__vk_instance);

  const auto& shader_dir = getShadersDir();
  __compute_shader = std::make_shared<VulkanShader>(shader_dir / "ray_tracer.comp.spirv", __dev_manager->getLogicalDevice());

  // Create Sphere buffer
  {
    auto spheres = std::array<VulkanSphere, 3>{};
    spheres[0].center = glm::vec3(-1.2f, 0.0f, 0.0f);
    spheres[0].radius = 0.5f;
    spheres[0].material.color_scale = glm::vec4(1.f, 0.270f, 0.f, 0.f); // Rosso
    spheres[0].material.material_id = 0; // matte

    spheres[1].center = glm::vec3(0.0f, 0.0f, 0.0f);
    spheres[1].radius = 0.5f;
    spheres[1].material.color_scale = glm::vec4(1.0f, 1.0f, 1.0f, 0.f); // bianco
    spheres[1].material.roughness_scale = 0.0f;
    spheres[1].material.material_id = 1; // metal

    spheres[2].center = glm::vec3(1.2f, 0.0f, 0.0f);
    spheres[2].radius = 0.5f;
    spheres[2].material.color_scale = glm::vec4(0.254f, 0.411f, 0.882f, 0.f); // Blu
    spheres[2].material.material_id = 0; // matte
    __sphere_buffer = std::make_shared<VulkanBuffer>(__dev_manager,
                                                     sizeof(VulkanSphere) * 3,
                                                     reinterpret_cast<void*>(spheres.data()),
                                                     VK_BUFFER_USAGE_STORAGE_BUFFER_BIT,
                                                     VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                                                     VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
  }
  // Create Plane buffer
  {
    auto plane = VulkanPlane{};
    plane.position = glm::vec4(0.0f, -0.5f, 0.0f, 0.f);
    plane.normal = glm::vec4(0.0f, 1.0f, 0.0f, 0.f);
    plane.size = glm::vec2(10.f, 10.f);
    plane.material.color_scale = glm::vec4(0.980, 0.921, 0.843, 0.f); // Grigio chiaro
    plane.material.emission_scale = glm::vec4(0.0f);
    plane.material.roughness_scale = 0.0f;
    plane.material.material_id = 0; // matte
    __plane_buffer = std::make_shared<VulkanBuffer>(__dev_manager,
                                                    sizeof(VulkanPlane),
                                                    reinterpret_cast<void*>(&plane),
                                                    VK_BUFFER_USAGE_STORAGE_BUFFER_BIT,
                                                    VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                                                    VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
  }
  
  // Create light buffer
  {
    auto sphere_light = VulkanSphere{};
    sphere_light.center = glm::vec3(0.f, 1.f, 2.0f);
    sphere_light.radius = 0.25f;
    sphere_light.material.emission_scale = glm::vec4(1.0f);
    sphere_light.material.material_id = 2; // Emissive
    __light_buffer = std::make_shared<VulkanBuffer>(__dev_manager,
                                                     sizeof(VulkanSphere),
                                                     reinterpret_cast<void*>(&sphere_light),
                                                     VK_BUFFER_USAGE_STORAGE_BUFFER_BIT,
                                                     VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                                                     VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
  }

  // Create the image output
  __compute_output_image = std::make_shared<VulkanImage>(__dev_manager,
                                                         __image_width,
                                                         __image_height,
                                                         VK_FORMAT_R8G8B8A8_UNORM,
                                                         VK_IMAGE_USAGE_STORAGE_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT,
                                                         VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);


  __descriptor_manager = std::make_shared<VulkanDescriptorManager>(__dev_manager);
  __descriptor_manager->addBinding(0, VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, VK_SHADER_STAGE_COMPUTE_BIT);  // Output image
  //__descriptor_manager->addBinding(1, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, VK_SHADER_STAGE_COMPUTE_BIT); // Camera
  __descriptor_manager->addBinding(2, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, VK_SHADER_STAGE_COMPUTE_BIT); // Sphere buffer
  __descriptor_manager->addBinding(3, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, VK_SHADER_STAGE_COMPUTE_BIT); // Plane buffer
  __descriptor_manager->addBinding(4, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, VK_SHADER_STAGE_COMPUTE_BIT); // Light buffer

  // Costruisci il layout e il pool, e alloca il set
  __descriptor_manager->build();
  
  __descriptor_manager->updateImage(0, VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, __compute_output_image->getImageView(), VK_NULL_HANDLE);
  //__descriptor_manager->updateBuffer(1, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, __camera_buffer->getBuffer(), sizeof(GpuCamera));
  __descriptor_manager->updateBuffer(2, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, __sphere_buffer->getBuffer(), sizeof(VulkanSphere) * 3);
  __descriptor_manager->updateBuffer(3, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, __plane_buffer->getBuffer(), sizeof(VulkanPlane));
  __descriptor_manager->updateBuffer(4, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, __light_buffer->getBuffer(), sizeof(VulkanSphere));

  // Creazione della pipeline
  __compute_pipeline = std::make_shared<VulkanComputePipeline>(__dev_manager,__compute_shader,__descriptor_manager);
  // Creazione del gestore dei comandi
  __command_manager = std::make_shared<VulkanCommandManager>(__dev_manager);
}

VulkanApp::~VulkanApp()
{
  // 1. Command buffer and pool
  __command_manager->cleanup();

  // 2. Pipeline
  __compute_pipeline->destroy();

  // 3. Descriptors
  __descriptor_manager->cleanup();

  // 4. Shader
  __compute_shader->destroyModule();

  // 5. Scene objects
  __sphere_buffer->destroy();
  __plane_buffer->destroy();
  __light_buffer->destroy();

  // 6. Output image
  __compute_output_image->destroy();

  // 7. Logical device 
  __dev_manager->cleanup(); 

  // 8. Vulkan instance
  vkDestroyInstance(__vk_instance, nullptr);
}

void VulkanApp::run()
{  
  auto nr_pixels = __image_width * __image_height;
  auto buffer_size = static_cast<size_t>(nr_pixels * sizeof(uint32_t)); // RGBA8 = 4 byte

  // ========================================================================
  // 1. Inizia il command buffer
  // ========================================================================
  __command_manager->beginRecording();
  auto command_buffer = __command_manager->getCommandBuffer();

  // ========================================================================
  // 2. Transizione di layout: UNDEFINED -> GENERAL (per la scrittura dello shader)
  // ========================================================================
  auto initial_barrier = __compute_output_image->createImageLayoutBarrier(VK_IMAGE_LAYOUT_UNDEFINED, 
                                                                     VK_IMAGE_LAYOUT_GENERAL);
  __command_manager->pipelineBarrier(VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
                                     VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT,
                                     initial_barrier);




  // ========================================================================
  // 3. Lega pipeline e descriptor set
  // ========================================================================
  __compute_pipeline->bind(command_buffer);
  __descriptor_manager->bind(command_buffer, __compute_pipeline->getLayout());


  // ========================================================================
  // 4. Lancia il compute shader
  // ========================================================================
  auto group_size_x = 16u;
  auto group_size_y = 16u;
  __command_manager->dispatch((__image_width + group_size_x - 1) / group_size_x,
                              (__image_height + group_size_y - 1) / group_size_y,
                              1);

  // ========================================================================
  // 5. Transizione di layout: GENERAL -> TRANSFER_SRC_OPTIMAL (per la copia)
  // ========================================================================
  auto copy_barrier = __compute_output_image->createImageLayoutBarrier(VK_IMAGE_LAYOUT_GENERAL, 
                                                                       VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL);
  __command_manager->pipelineBarrier(VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT,
                                     VK_PIPELINE_STAGE_TRANSFER_BIT,
                                     copy_barrier);

  // ========================================================================
  // 6. Leggi l'immagine dalla GPU e salvala
  // ========================================================================

  // 6.1. Crea uno staging buffer (host visible, host coherent)
  auto staging_buffer = std::make_shared<VulkanBuffer>(__dev_manager,
                                                       buffer_size,
                                                       nullptr,
                                                       VK_BUFFER_USAGE_TRANSFER_DST_BIT,
                                                       VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

  // 6.2. Copia l'immagine nello staging buffer
  __compute_output_image->copyToBuffer(command_buffer, staging_buffer, __image_width, __image_height);

  // ========================================================================
  // 7. Fine del command buffer e submit
  // ========================================================================
  __command_manager->endRecording();
  __command_manager->submitQueue();

  // ========================================================================
  // 8. Mappa e leggi i pixel
  // ========================================================================
  
  // The buffer_data vector, populated by your readMappedData method contains the raw pixel data directly from the GPU.
  // This data is in the RGBA format, with each pixel occupying four bytes.
  auto buffer_data = std::vector<std::byte>();
  staging_buffer->readMappedData(buffer_data);
  staging_buffer->destroy();

  // We need to create a new pixels vector because the stbi_write_png function you're using requires the pixel data 
  // to be in the RGB format, which means each pixel should only occupy three bytes.
  auto pixels = std::vector<std::byte>();
  pixels.reserve(nr_pixels * 3);

  auto mapped_data = reinterpret_cast<const uint32_t*>(buffer_data.data());
  for (auto i = 0u; i < nr_pixels; ++i)
  {
    auto pixel = mapped_data[i];
    auto r = (pixel >> 0) & 0xFF;
    auto g = (pixel >> 8) & 0xFF;
    auto b = (pixel >> 16) & 0xFF;
    pixels.push_back(static_cast<std::byte>(r));
    pixels.push_back(static_cast<std::byte>(g));
    pixels.push_back(static_cast<std::byte>(b));
  }

  // ========================================================================
  // 9. Salva in PNG
  // ========================================================================
  auto dest_path = getRootDir() / __image_output_name;
  ImageLoader::writePNG(dest_path, glm::uvec2(__image_width, __image_height), pixels.data());
  std::cout << "Output image \"" << __image_output_name << "\"" << std::endl;
}

/**
 *	=========================
 *					PRIVATE
 *	=========================
 */

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

  auto create_info = VkInstanceCreateInfo{};
  create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
  create_info.pApplicationInfo = &app_info;
  create_info.enabledExtensionCount = g_instance_extensions.size();
  create_info.ppEnabledExtensionNames = g_instance_extensions.data();
  create_info.enabledLayerCount = g_validation_layers.size();
  create_info.ppEnabledLayerNames = g_validation_layers.data();
  auto result = vkCreateInstance(&create_info, nullptr, &__vk_instance);
  assert(result == VK_SUCCESS && "Failed to create instance!");
  std::cout << "VkInstance created successfully" << std::endl;
}


