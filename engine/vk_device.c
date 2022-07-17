// Vulkan device selection and creation

#include "engine.h"
#include "render.h"

VkPhysicalDevice g_vulkan_phys_device;
VkDevice g_vulkan_device;

// List of queue family indices
typedef struct queue_families {
	uint32_t graphics;
	bool graphics_found;
	uint32_t present;
	bool present_found;
	uint32_t compute;
	bool compute_found;
} queue_families_t;

// Checks if all the queue families have been found
#define CHECK_QUEUE_FAMILIES(indices) ((indices).graphics_found && (indices).present_found && (indices).compute_found)

static queue_families_t find_queue_families(VkPhysicalDevice device)
{
	queue_families_t indices = { 0 };
	VkQueueFamilyProperties *families;
	uint32_t family_count;
	uint32_t i;
	VkBool32 present;

	vkGetPhysicalDeviceQueueFamilyProperties(device, &family_count, NULL);
	families = util_alloc(family_count, sizeof(VkQueueFamilyProperties), NULL);
	vkGetPhysicalDeviceQueueFamilyProperties(device, &family_count, families);

	for (i = 0; i < family_count && !CHECK_QUEUE_FAMILIES(indices); i++) {
		if (families[i].queueFlags & VK_QUEUE_GRAPHICS_BIT && !indices.graphics_found) {
			indices.graphics = i;
			indices.graphics_found = true;
		}
		if (families[i].queueFlags & VK_QUEUE_COMPUTE_BIT && !indices.compute_found) {
			indices.compute = i;
			indices.compute_found = true;
		}

		vkGetPhysicalDeviceSurfaceSupportKHR(device, i, g_vulkan_surface, &present);
		if (present && !indices.present_found) {
			indices.present = i;
			indices.present_found = true;
		}
	}

	free(families);

	return indices;
}

static uint32_t score_device(VkPhysicalDevice device, uint32_t i, VkPhysicalDeviceProperties *properties,
			     VkPhysicalDeviceFeatures *features)
{
	uint32_t score;
	queue_families_t indices;

	PURPL_LOG(RENDER_LOG_PREFIX "Getting properties of device %d\n", i);
	vkGetPhysicalDeviceProperties(device, properties);
	vkGetPhysicalDeviceFeatures(device, features);

	indices = find_queue_families(device);
	if (!CHECK_QUEUE_FAMILIES(indices)) {
		PURPL_LOG(RENDER_LOG_PREFIX "Not all queue families detected for device %d, ignoring\n", i);
		return 0;
	}

	score = 0;

	// Improves performance
	if (indices.graphics == indices.present)
		score += 500;

	if (properties->deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
		score += 1000;

	score += properties->limits.maxImageDimension2D;

	// NVIDIA's drivers are better on Windows, AMD's are better on Linux
#ifdef _WIN32
	if (properties->vendorID == PCI_VENDOR_NVIDIA)
#elif __linux__
	if (properties->vendorID == PCI_VENDOR_AMD)
#else
	if (false)
#endif
		score += 10;

	score *= VK_VERSION_MINOR(properties->apiVersion);

	PURPL_LOG(RENDER_LOG_PREFIX "Device %d information:\n", i);
	PURPL_LOG(RENDER_LOG_PREFIX "\tName: %s\n", properties->deviceName);
	PURPL_LOG(RENDER_LOG_PREFIX "\tPCI ID: %x:%x\n", properties->vendorID, properties->deviceID);
	PURPL_LOG(RENDER_LOG_PREFIX "\tScore: %d\n", score);
	PURPL_LOG(RENDER_LOG_PREFIX "\tQueue family indices:\n");
	PURPL_LOG(RENDER_LOG_PREFIX "\t\tGraphics: %u\n", indices.graphics);
	PURPL_LOG(RENDER_LOG_PREFIX "\t\tPresent: %u\n", indices.present);
	PURPL_LOG(RENDER_LOG_PREFIX "\t\tCompute: %u\n", indices.compute);

	return score;
}

bool engine_vulkan_choose_device(void)
{
	VkResult result;
	uint32_t device_count;
	VkPhysicalDevice *physical_devices;
	uint32_t i;
	uint32_t score;
	VkPhysicalDeviceProperties properties;
	VkPhysicalDeviceFeatures features;
	uint32_t best_score;
	int32_t best_idx;
	VkPhysicalDeviceProperties best_properties;
	VkPhysicalDeviceFeatures best_features;

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
		g_vulkan_phys_device = physical_devices[g_engine->device_idx];
		vkGetPhysicalDeviceProperties(g_vulkan_phys_device, &properties);
		PURPL_LOG(RENDER_LOG_PREFIX "Device %d (%s) is within valid range, assuming it works. If not,"
					    "change device_index to -1 in the settings\n",
			  g_engine->device_idx, properties.deviceName);
		free(physical_devices);
		return true;
	}

	PURPL_LOG(RENDER_LOG_PREFIX "Choosing graphics device to use\n");
	g_vulkan_phys_device = NULL;
	score = 0;
	best_score = 0;
	best_idx = -1;
	for (i = 0; i < device_count; i++) {
		score = score_device(physical_devices[i], i, &properties, &features);
		if (score > best_score) {
			best_score = score;
			best_idx = (int32_t)i;
			best_properties = properties;
			best_features = features;
		}
	}

	g_engine->device_idx = best_idx;
	if (g_engine->device_idx < 0) {
		PURPL_LOG(RENDER_LOG_PREFIX "No suitable device found\n");
		free(physical_devices);
		return false;
	}

	PURPL_LOG(RENDER_LOG_PREFIX "Using device %d (%s, score %u)\n", g_engine->device_idx,
		  best_properties.deviceName);

	g_vulkan_phys_device = physical_devices[g_engine->device_idx];
	free(physical_devices);

	return true;
}

bool engine_vulkan_create_device(void)
{
	VkDeviceCreateInfo device_info = { 0 };
	VkPhysicalDeviceFeatures features = { 0 };
	VkDeviceQueueCreateInfo queue_infos[3];
	queue_families_t indices;
	uint32_t queue_indices[3];
	float queue_priority;
	VkResult result;
	uint8_t i;
	uint8_t j;

	if (!engine_vulkan_choose_device())
		return false;

	PURPL_LOG(RENDER_LOG_PREFIX "Getting queue family indices for device %d\n", g_engine->device_idx);
	indices = find_queue_families(g_vulkan_phys_device);
	queue_indices[0] = indices.graphics;
	queue_indices[1] = indices.present;
	queue_indices[2] = indices.compute;

	PURPL_LOG(RENDER_LOG_PREFIX "Filling VkDeviceQueueCreateInfo structures\n");
	queue_priority = 1;
	memset(queue_infos, 0, sizeof(queue_infos));
	for (i = 0, j = 0; i < 3; i++) {
		if (i > 0 && queue_indices[i - 1] == queue_indices[i])
			j++;
		queue_infos[i - j].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		queue_infos[i - j].queueFamilyIndex = queue_indices[i];
		queue_infos[i - j].queueCount = 1;
		queue_infos[i - j].pQueuePriorities = &queue_priority;
	}

	PURPL_LOG(RENDER_LOG_PREFIX "Setting VkDeviceCreateInfo fields\n");
	device_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	device_info.pEnabledFeatures = &features;
	device_info.queueCreateInfoCount = i - j;
	device_info.pQueueCreateInfos = queue_infos;

	PURPL_LOG(RENDER_LOG_PREFIX "Creating VkDevice\n");
	result = vkCreateDevice(g_vulkan_phys_device, &device_info, NULL, &g_vulkan_device);
	if (result != VK_SUCCESS) {
		PURPL_LOG(RENDER_LOG_PREFIX "vkCreateDevice(0x%" PRIXPTR ", 0x%" PRIXPTR ", 0x%" PRIXPTR ", 0x%" PRIXPTR
					    ") failed: VkResult %d\n",
			  g_vulkan_phys_device, &device_info, NULL, &g_vulkan_device, result);
		return false;
	}

	return true;
}
