cmake_minimum_required(VERSION 3.16)
cmake_policy(SET CMP0077 NEW)

project(RayTracingCpp)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(FETCHCONTENT_QUIET OFF)
set(GLFW_BUILD_EXAMPLES OFF)
set(GLFW_BUILD_TESTS OFF)
set(GLFW_BUILD_DOCS OFF)
set(GLFW_BUILD_INSTALL OFF)
set(GLFW_VULKAN_STATIC OFF)  # se usi loader Vulkan esterno

include(FetchContent)
FetchContent_Declare(
  glm
  GIT_REPOSITORY  https://github.com/g-truc/glm.git
  GIT_TAG         1.0.1
)
FetchContent_MakeAvailable(glm)

FetchContent_Declare(
  glfw
  GIT_REPOSITORY https://github.com/glfw/glfw.git
  GIT_TAG        3.4 
)
FetchContent_MakeAvailable(glfw)

find_package(Vulkan REQUIRED)

# Header files (optional to list, but good for IDEs)
set(HEADERS
  include/stb_image_write.h
  include/stb_image.h
  include/Camera.hpp
  include/Ray.hpp
  include/Renderer.hpp
  include/Scene.hpp
  include/ImageLoader.hpp
    
  include/Geometry/IHittableObject.hpp
  include/Geometry/Sphere.hpp
  include/Geometry/Plane.hpp

  include/Material/IMaterial.hpp
  include/Material/Matte.hpp
  include/Material/Metal.hpp
  include/Material/Emissive.hpp

  include/Texture/ITexture.hpp
  include/Texture/Texture2D.hpp
)

set(SOURCES
  src/main.cpp
  src/Camera.cpp
  src/Renderer.cpp
  src/Scene.cpp
  src/ImageLoader.cpp

  src/Geometry/Sphere.cpp
  src/Geometry/Plane.cpp
  
  src/Material/Matte.cpp
  src/Material/Metal.cpp
  src/Material/Emissive.cpp
  
  src/Texture/Texture2D.cpp
)



add_executable(RayTracingCpp ${SOURCES} ${HEADERS})

# Add include directories scoped to this target
target_include_directories(RayTracingCpp PRIVATE "include/")

target_link_libraries(RayTracingCpp PRIVATE glm::glm)
target_link_libraries(RayTracingCpp PRIVATE glfw)
target_link_libraries(RayTracingCpp PRIVATE Vulkan::Vulkan)