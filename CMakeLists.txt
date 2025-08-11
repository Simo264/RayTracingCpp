cmake_minimum_required(VERSION 3.13) # CMP0077 è disponibile da 3.13
cmake_policy(SET CMP0077 NEW)

project(RayTracingCpp)

# specify the C++ standard
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


add_executable(RayTracingCpp
  src/stb_image_write.h 
  src/HittableObject.hpp
  src/Ray.hpp
  src/Interval.hpp
  src/Sphere.hpp
  src/Sphere.cpp
  src/Plane.hpp
  src/Plane.cpp
  src/Scene.hpp
  src/Scene.cpp
  src/Camera.hpp
  src/Camera.cpp
  src/main.cpp
)

target_link_libraries(RayTracingCpp PRIVATE glm::glm)