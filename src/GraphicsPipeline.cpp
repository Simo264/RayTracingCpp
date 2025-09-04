#include "GraphicsPipeline.hpp"
#include "Shader.hpp"
#include <cassert>
#include <iostream>

GraphicsPipeline::GraphicsPipeline(std::shared_ptr<Shader> vertex_shader,
                                   std::shared_ptr<Shader> fragment_shader,
                                   VkDevice device,
                                   VkRenderPass render_pass,
                                   VkExtent2D swapchain_extent) :
	__device{ device },
	__graphics_pipeline{ VK_NULL_HANDLE },
	__pipeline_layout{ VK_NULL_HANDLE }
{
  // Constructor creates the full graphics pipeline, combining the fixed-function and programmable stages into a single object. 
  // Its role is to orchestrate the entire rendering flow from vertices to pixels, as required to display the output 
  // of our ray tracer.
  // The pipeline is set up for a specific, minimal task: to act as a display mechanism for the image created by 
  // our ray tracer's compute shader. It's not designed to render complex 3D scenes.
  auto vert_shader_stage_info = VkPipelineShaderStageCreateInfo{};
  vert_shader_stage_info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
  vert_shader_stage_info.stage = VK_SHADER_STAGE_VERTEX_BIT;
  vert_shader_stage_info.module = vertex_shader->getShaderModule();
  vert_shader_stage_info.pName = "main";
  auto frag_shader_stage_info = VkPipelineShaderStageCreateInfo{};
  frag_shader_stage_info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
  frag_shader_stage_info.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
  frag_shader_stage_info.module = fragment_shader->getShaderModule();
  frag_shader_stage_info.pName = "main";
	
  auto shader_stages = std::array<VkPipelineShaderStageCreateInfo, 2>{ vert_shader_stage_info, frag_shader_stage_info };

  auto vertex_input_info = VkPipelineVertexInputStateCreateInfo{};
  vertex_input_info.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
  auto input_assembly = VkPipelineInputAssemblyStateCreateInfo{};
  input_assembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
  input_assembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
  input_assembly.primitiveRestartEnable = VK_FALSE;

  auto viewport = VkViewport{};
  viewport.x = 0.0f;
  viewport.y = 0.0f;
  viewport.width = static_cast<float>(swapchain_extent.width);
  viewport.height = static_cast<float>(swapchain_extent.height);
  viewport.minDepth = 0.0f;
  viewport.maxDepth = 1.0f;
  auto scissor = VkRect2D{};
  scissor.offset = { 0, 0 };
  scissor.extent = swapchain_extent;
  auto viewport_state = VkPipelineViewportStateCreateInfo{};
  viewport_state.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
  viewport_state.viewportCount = 1;
  viewport_state.pViewports = &viewport;
  viewport_state.scissorCount = 1;
  viewport_state.pScissors = &scissor;

  auto rasterizer = VkPipelineRasterizationStateCreateInfo{};
  rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
  rasterizer.depthClampEnable = VK_FALSE;
  rasterizer.rasterizerDiscardEnable = VK_FALSE;
  rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
  rasterizer.lineWidth = 1.0f;
  rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
  rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;
  rasterizer.depthBiasEnable = VK_FALSE;

  auto multisampling = VkPipelineMultisampleStateCreateInfo{};
  multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
  multisampling.sampleShadingEnable = VK_FALSE;
  multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

  auto color_blend_attachment = VkPipelineColorBlendAttachmentState{};
  color_blend_attachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
  color_blend_attachment.blendEnable = VK_FALSE;
  auto color_blending = VkPipelineColorBlendStateCreateInfo{};
  color_blending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
  color_blending.logicOpEnable = VK_FALSE;
  color_blending.attachmentCount = 1;
  color_blending.pAttachments = &color_blend_attachment;

  auto pipeline_layout_info = VkPipelineLayoutCreateInfo{};
  pipeline_layout_info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
  pipeline_layout_info.setLayoutCount = 0; // nessun descriptor set per ora
  pipeline_layout_info.pSetLayouts = nullptr;
  pipeline_layout_info.pushConstantRangeCount = 0;
  pipeline_layout_info.pPushConstantRanges = nullptr;
  auto result = vkCreatePipelineLayout(__device, &pipeline_layout_info, nullptr, &__pipeline_layout);
  assert(result == VK_SUCCESS && "failed to create pipeline layout!");

  auto pipeline_info = VkGraphicsPipelineCreateInfo{};
  pipeline_info.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
  pipeline_info.stageCount = static_cast<uint32_t>(shader_stages.size());
  pipeline_info.pStages = shader_stages.data();
  pipeline_info.pVertexInputState = &vertex_input_info;
  pipeline_info.pInputAssemblyState = &input_assembly;
  pipeline_info.pViewportState = &viewport_state;
  pipeline_info.pRasterizationState = &rasterizer;
  pipeline_info.pMultisampleState = &multisampling;
  pipeline_info.pColorBlendState = &color_blending;
  pipeline_info.layout = __pipeline_layout;
  pipeline_info.renderPass = render_pass; // Assicurati che il render pass sia già stato creato
  pipeline_info.subpass = 0;
  pipeline_info.basePipelineHandle = VK_NULL_HANDLE;

  result = vkCreateGraphicsPipelines(__device, VK_NULL_HANDLE, 1, &pipeline_info, nullptr, &__graphics_pipeline);
  assert(result == VK_SUCCESS && "failed to create graphics pipeline!");

  std::cout << "Graphics pipeline created successfully!" << std::endl;
}


void GraphicsPipeline::destroy() const
{
	if(__graphics_pipeline != VK_NULL_HANDLE)
		vkDestroyPipeline(__device, __graphics_pipeline, nullptr);

	if (__pipeline_layout != VK_NULL_HANDLE)
		vkDestroyPipelineLayout(__device, __pipeline_layout, nullptr);
}
