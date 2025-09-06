#include "VulkanApp.hpp"

int main()
{
  VulkanApp app(640u, 480u, "hello.png");
  app.run();

  return 0;
}