#include "VulkanApp.hpp"

int main()
{
  VulkanApp app(640u, 480u, "output.png");
  app.run();

  return 0;
}