#include <glm/glm.hpp>
#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>

#include <iostream>

#include "ImageLoader.hpp"
#include "Camera.hpp"
#include "Scene.hpp"
#include "Renderer.hpp"
#include "Geometry/Sphere.hpp"
#include "Geometry/Plane.hpp"
#include "Material/Matte.hpp"
#include "Material/Metal.hpp"
#include "Material/Emissive.hpp"

static VkInstance createVulkanInstance()
{
  // Info sull'applicazione
  VkApplicationInfo appInfo{};
  appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
  appInfo.pApplicationName = "RayTracingVulkan";
  appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
  appInfo.pEngineName = "NoEngine";
  appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
  appInfo.apiVersion = VK_API_VERSION_1_3;

  // Estensioni richieste da GLFW
  uint32_t glfwExtensionCount = 0;
  const char** glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

  VkInstanceCreateInfo createInfo{};
  createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
  createInfo.pApplicationInfo = &appInfo;
  createInfo.enabledExtensionCount = glfwExtensionCount;
  createInfo.ppEnabledExtensionNames = glfwExtensions;

#ifndef NDEBUG
  const char* validationLayers[] = { "VK_LAYER_KHRONOS_validation" };
  createInfo.enabledLayerCount = 1;
  createInfo.ppEnabledLayerNames = validationLayers;
#else
  createInfo.enabledLayerCount = 0;
#endif

  VkInstance instance;
  VkResult result = vkCreateInstance(&createInfo, nullptr, &instance);
  if (result != VK_SUCCESS)
  {
    throw std::runtime_error("Errore nella creazione del VkInstance");
  }

  return instance;
}

int main()
{
  if (!glfwInit())
  {
    std::cerr << "Errore nell'inizializzazione di GLFW\n";
    return -1;
  }

  glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
  
  GLFWwindow* window = glfwCreateWindow(800, 600, "Ray Tracing Vulkan", nullptr, nullptr);
  if (!window)
  {
    std::cerr << "Errore nella creazione della finestra\n";
    glfwTerminate();
    return -1;
  }

  // Crea il VkInstance
  VkInstance instance;
  try
  {
    instance = createVulkanInstance();
    std::cout << "VkInstance creato con successo\n";
  }
  catch (const std::exception& e)
  {
    std::cerr << e.what() << "\n";
    glfwDestroyWindow(window);
    glfwTerminate();
    return -1;
  }
  
  while (!glfwWindowShouldClose(window))
  {
    glfwPollEvents();
  }

  // Cleanup Vulkan
  vkDestroyInstance(instance, nullptr);

  glfwDestroyWindow(window);
  glfwTerminate();
  return 0;
}