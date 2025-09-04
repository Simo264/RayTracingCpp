#include "RenderPass.hpp"

#include <cassert>
#include <iostream>

RenderPass::RenderPass(VkDevice device, VkFormat swap_chain_image_format) :
	__render_pass{ VK_NULL_HANDLE },
	__device{ device }
{
  // Its primary purpose is to prepare a single color attachment and a corresponding subpass that will be used by the 
  // graphics pipeline to display the results of your ray tracing.
  // This minimal setup is perfect for our needs, as the graphics pipeline will only be responsible for drawing a 
  // screen - filling quad textured with the ray traced image, not complex 3D geometry.


  auto color_attachment = VkAttachmentDescription{};
  color_attachment.format = swap_chain_image_format; // Usa il formato della swap chain
  color_attachment.samples = VK_SAMPLE_COUNT_1_BIT; // 1 sample, no multisampling
  color_attachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR; // Cancella l'immagine all'inizio del render pass
  color_attachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE; // Salva l'immagine alla fine del render pass
  color_attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE; // Ignora lo stencil buffer
  color_attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
  color_attachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED; // Layout iniziale sconosciuto
  color_attachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR; // Layout finale per la presentazione
  auto color_attachment_ref = VkAttachmentReference{};
  color_attachment_ref.attachment = 0; // Si riferisce al primo attachment
  color_attachment_ref.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

  auto subpass = VkSubpassDescription{};
  subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
  subpass.colorAttachmentCount = 1;
  subpass.pColorAttachments = &color_attachment_ref;

  auto render_pass_info = VkRenderPassCreateInfo{};
  render_pass_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
  render_pass_info.attachmentCount = 1;
  render_pass_info.pAttachments = &color_attachment;
  render_pass_info.subpassCount = 1;
  render_pass_info.pSubpasses = &subpass;

  auto result = vkCreateRenderPass(device, &render_pass_info, nullptr, &__render_pass);
  assert(result == VK_SUCCESS && "failed to create render pass!");

  std::cout << "Render pass created successfully!" << std::endl;
}

void RenderPass::destroy() const
{
	if(__render_pass != VK_NULL_HANDLE)
		vkDestroyRenderPass(__device, __render_pass, nullptr);
}
