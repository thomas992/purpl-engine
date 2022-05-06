// Renderpass stuff
//
// Copyright 2022 MobSlicer152
// This file is part of Purpl Engine
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "purpl/graphics/vulkan/render.h"

bool vulkan_create_default_renderpass(void)
{
	PURPL_ALIAS_GRAPHICS_DATA(vulkan);

	VkAttachmentDescription colour_attachment = { 0 };
	VkAttachmentReference colour_attachment_ref = { 0 };
	VkSubpassDescription subpass_desc = { 0 };
	VkRenderPassCreateInfo renderpass_create_info = { 0 };
	VkResult res;

	PURPL_LOG_INFO(purpl_inst->logger, "Creating default renderpass");

	colour_attachment.format = vulkan->swapchain_format;
	colour_attachment.samples = VK_SAMPLE_COUNT_1_BIT;
	colour_attachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	colour_attachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	colour_attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	colour_attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;

	colour_attachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	colour_attachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

	colour_attachment_ref.attachment = 0; // Index of the attachment in the pAttachments array in the create info for the
					      // renderpass
	colour_attachment_ref.layout = VK_IMAGE_LAYOUT_ATTACHMENT_OPTIMAL;

	subpass_desc.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
	subpass_desc.colorAttachmentCount = 1;
	subpass_desc.pColorAttachments = &colour_attachment_ref;

	renderpass_create_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	renderpass_create_info.attachmentCount = 1;
	renderpass_create_info.pAttachments = &colour_attachment;
	renderpass_create_info.subpassCount = 1;
	renderpass_create_info.pSubpasses = &subpass_desc;

	res = vkCreateRenderPass(vulkan->device, &renderpass_create_info, NULL, &vulkan->renderpass);
	if (res != VK_SUCCESS) {
		PURPL_LOG_ERROR(purpl_inst->logger, "Failed to create default renderpass: VkResult %d", res);
		return false;
	}

	PURPL_LOG_INFO(purpl_inst->logger, "Created default renderpass with handle 0x%" PRIX64, vulkan->renderpass);

	return true;
}

bool vulkan_create_framebuffers(void)
{
	PURPL_ALIAS_GRAPHICS_DATA(vulkan);

	VkFramebufferCreateInfo fb_create_info = { 0 };

	PURPL_LOG_INFO(purpl_inst->logger, "Creating framebuffer");

	fb_create_info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
	fb_create_info.pNext = NULL;

	fb_create_info.renderPass = vulkan->renderpass;
}

