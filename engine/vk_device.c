// Vulkan device selection and creation

#include "engine.h"
#include "render.h"

VkPhysicalDevice g_vulkan_phys_device;
VkDevice g_vulkan_device;

static uint32_t score_device(VkDevice device, uint32_t i) {}

bool engine_vulkan_choose_device(void)
{
	VkResult result;
	uint32_t device_count;
	VkPhysicalDevice *physical_devices;
	uint32_t i;
	uint32_t score;
	uint32_t best_score;
	int64_t best_idx;

	PURPL_LOG(RENDER_LOG_PREFIX "Getting devices\n");
	vkEnumeratePhysicalDevices(g_vulkan_inst, &device_count, NULL);
	physical_devices = util_alloc(device_count, sizeof(VkPhysicalDevice), NULL);
	result = vkEnumeratePhysicalDevices(g_vulkan_inst, &device_count, physical_devices);
	if (result != VK_SUCCESS) {
		PURPL_LOG(RENDER_LOG_PREFIX "Failed to enumerate devices: VkResult %d\n", result);
		return false;
	}
	PURPL_LOG(RENDER_LOG_PREFIX "Got %d devices\n", device_count);

	if (g_engine->device_idx > 0 && g_engine->device_idx <= device_count) {
		PURPL_LOG(
			RENDER_LOG_PREFIX
			"Device index %d is within valid range, assuming it works. If not, change device_index to -1 in the settings\n");
		g_vulkan_phys_device = physical_devices[g_engine->device_idx];
		free(physical_devices);
		return true;
	}

	PURPL_LOG(RENDER_LOG_PREFIX "Choosing graphics device to use\n");
	g_vulkan_phys_device = NULL;
	score = 0;
	best_score = 0;
	best_idx = -1;
	for (i = 0; i < device_count; i++) {
		score = score_device(physical_devices[i], i);
		if (score > best_score) {
			best_score = score;
			best_idx = i;
		}
	}
}

bool engine_vulkan_create_device(void)
{

}
