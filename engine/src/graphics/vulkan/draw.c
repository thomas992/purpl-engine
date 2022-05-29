// Draw functions
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

#include "purpl/graphics/vulkan/draw.h"

bool vulkan_draw(void)
{
	PURPL_ALIAS_GRAPHICS_DATA(vulkan);

	u32 target_image_idx = 0;
	VkCommandBufferBeginInfo cmd_buf_begin_info = { 0 };
	VkClearValue clear_value;
	VkRenderPassBeginInfo renderpass_begin_info = { 0 };
	VkSubmitInfo submit_info = { 0 };
	VkPipelineStageFlags wait_stage;
	VkPresentInfoKHR present_info = { 0 };
	VkResult res;

	res = vkWaitForFences(vulkan->device, 1, &vulkan->render_fence, true, 1000000000);
	if (res != VK_SUCCESS) {
		PURPL_LOG_ERROR(purpl_inst->logger, "Failed to wait for render fence: VkResult %d", res);
		return false;
	}

	res = vkAcquireNextImageKHR(vulkan->device, vulkan->swapchain, 1000000000, vulkan->present_semaphore, NULL,
				    &target_image_idx);
	if (res != VK_SUCCESS) {
		if (res == VK_ERROR_SURFACE_LOST_KHR) {
			vulkan_recreate_surface();
			return true;
		} else if (res == VK_ERROR_OUT_OF_DATE_KHR || res == VK_SUBOPTIMAL_KHR) {
			vulkan_recreate_swapchain();
			return true;
		} else {
			PURPL_LOG_ERROR(purpl_inst->logger, "Failed to get index of next image from swapchain: VkResult %d",
					res);
			return false;
		}
	}

	res = vkResetFences(vulkan->device, 1, &vulkan->render_fence);
	if (res != VK_SUCCESS) {
		PURPL_LOG_ERROR(purpl_inst->logger, "Failed to reset render fence: VkResult %d", res);
		return false;
	}

	res = vkResetCommandBuffer(vulkan->cmd_buf, 0);
	if (res != VK_SUCCESS) {
		PURPL_LOG_ERROR(purpl_inst->logger, "Failed to reset command buffer: VkResult %d", res);
		return false;
	}

	cmd_buf_begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	cmd_buf_begin_info.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

	res = vkBeginCommandBuffer(vulkan->cmd_buf, &cmd_buf_begin_info);
	if (res != VK_SUCCESS) {
		PURPL_LOG_ERROR(purpl_inst->logger, "Failed to begin recording to command buffer: VkResult %d", res);
		return false;
	}

	// Clear the screen to purple
	clear_value.color = (VkClearColorValue){ { 0.4f, 0.1f, 1.0f, 0.0f } };

	renderpass_begin_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;

	renderpass_begin_info.renderPass = vulkan->renderpass;
	renderpass_begin_info.renderArea.offset.x = 0;
	renderpass_begin_info.renderArea.offset.y = 0;
	renderpass_begin_info.renderArea.extent = vulkan->swapchain_extent;
	renderpass_begin_info.framebuffer = vulkan->framebuffers[target_image_idx];

	renderpass_begin_info.clearValueCount = 1;
	renderpass_begin_info.pClearValues = &clear_value;

	vkCmdBeginRenderPass(vulkan->cmd_buf, &renderpass_begin_info, VK_SUBPASS_CONTENTS_INLINE);
	vkCmdEndRenderPass(vulkan->cmd_buf);

	res = vkEndCommandBuffer(vulkan->cmd_buf);
	if (res != VK_SUCCESS) {
		PURPL_LOG_ERROR(purpl_inst->logger, "Failed to end recording to command buffer: VkResult %d", res);
		return false;
	}

	submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	wait_stage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	submit_info.pWaitDstStageMask = &wait_stage;
	
	submit_info.waitSemaphoreCount = 1;
	submit_info.pWaitSemaphores = &vulkan->present_semaphore;

	submit_info.signalSemaphoreCount = 1;
	submit_info.pSignalSemaphores = &vulkan->render_semaphore;

	submit_info.commandBufferCount = 1;
	submit_info.pCommandBuffers = &vulkan->cmd_buf;

	res = vkQueueSubmit(vulkan->graphics_queue, 1, &submit_info, vulkan->render_fence);
	if (res != VK_SUCCESS) {
		PURPL_LOG_ERROR(purpl_inst->logger, "Failed to submit commands: VkResult %d", res);
		return false;
	}

	present_info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

	present_info.swapchainCount = 1;
	present_info.pSwapchains = &vulkan->swapchain;
	
	present_info.waitSemaphoreCount = 1;
	present_info.pWaitSemaphores = &vulkan->render_semaphore;

	present_info.pImageIndices = &target_image_idx;

	res = vkQueuePresentKHR(vulkan->graphics_queue, &present_info);
	if (res != VK_SUCCESS) {
		if (res == VK_ERROR_OUT_OF_DATE_KHR || res == VK_SUBOPTIMAL_KHR) {
			vulkan_recreate_swapchain();
			return true;
		} else {
			PURPL_LOG_ERROR(purpl_inst->logger, "Failed to present image %u: VkResult %d", target_image_idx + 1, res);
			return false;
		}
	}

	return true;
}
