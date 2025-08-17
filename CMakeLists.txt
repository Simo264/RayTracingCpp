cmake_minimum_required(VERSION 3.16)
cmake_policy(SET CMP0077 NEW)

project(RayTracingCpp)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(FETCHCONTENT_QUIET OFF)

include(FetchContent)
FetchContent_Declare(
  glm
  GIT_REPOSITORY  https://github.com/g-truc/glm.git
  GIT_TAG         bf71a834948186f4097caa076cd2663c69a10e1e #refs/tags/1.0.1
)
FetchContent_MakeAvailable(glm)

set(SOURCES
  src/main.cpp
  src/Camera.cpp
  src/Image.cpp
  src/Random.cpp

  src/Scene/Scene.cpp
  src/Scene/Sphere.cpp
  src/Scene/Plane.cpp
  src/Material/Lambertian.cpp
  src/Material/Metal.cpp

  src/Renderer/Renderer.cpp
)

# Header files (optional to list, but good for IDEs)
set(HEADERS
    include/stb_image_write.h
    include/Camera.hpp
    include/Image.hpp
    include/Random.hpp
    include/Ray.hpp
    include/Interval.hpp
    include/Scene/Scene.hpp
    include/Scene/IHittableObject.hpp
    include/Scene/Sphere.hpp
    include/Scene/Plane.hpp

    include/Material/IMaterial.hpp
    include/Material/Lambertian.hpp
    include/Material/Metal.hpp

    include/Renderer/Renderer.hpp
)

add_executable(RayTracingCpp ${SOURCES} ${HEADERS})

# Add include directories scoped to this target
target_include_directories(RayTracingCpp PRIVATE "include/")

target_link_libraries(RayTracingCpp PRIVATE glm::glm)