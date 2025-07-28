# RayTracingCpp

RayTracingCpp is a ray tracing renderer written in C++ 20.  
The project is structured in two main stages:

- **CPU Version**: Implements the fundamentals of ray tracing.
- **GPU Version**: A Vulkan-based implementation using hardware-accelerated ray tracing via the `VK_KHR_ray_tracing_pipeline` extension.

---

## 🔧 Build Instructions

- cmake -S . -B build/


## 📚 References

- [Ray Tracing in One Weekend](https://raytracing.github.io/books/RayTracingInOneWeekend.html) – Peter Shirley, Trevor David Black, Steve Hollasch
- [Ray Tracing: The Next Week](https://raytracing.github.io/books/RayTracingTheNextWeek.html) – Peter Shirley, Trevor David Black, Steve Hollasch
- [Ray Tracing: The Rest of Your Life](https://raytracing.github.io/books/RayTracingTheRestOfYourLife.html) – Peter Shirley, Trevor David Black, Steve Hollasch
- [Physically Based Rendering](https://pbr-book.org/) – Matt Pharr, Wenzel Jakob, and Greg Humphreys
- [Scratchapixel](https://scratchapixel.com/) – Fundamentals of Computer Graphics
- [NVIDIA Vulkan Ray Tracing Tutorial](https://nvpro-samples.github.io/vk_raytracing_tutorial_KHR/)
