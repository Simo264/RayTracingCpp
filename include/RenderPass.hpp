#pragma once

#include <vulkan/vulkan.h>


class RenderPass
{
public:
  RenderPass(VkDevice device, VkFormat swap_chain_image_format);
  ~RenderPass() = default;

  auto getVkRenderPass() const { return __render_pass; }

  void destroy() const;

  RenderPass(const RenderPass&) = delete;
  RenderPass& operator=(const RenderPass&) = delete;

private:
  VkDevice __device;
  VkRenderPass __render_pass;
};
