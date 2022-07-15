// Vulkan rendering initialization

#include "engine.h"
#include "render.h"

VkInstance g_vulkan_inst;

// Gets Vulkan instance extensions
const char **get_extensions(uint32_t *extension_count)
{
	const char **extensions;
	uint32_t i;

	if (!extension_count)
		return NULL;

	*extension_count = 0;
	PURPL_ASSERT(SDL_Vulkan_GetInstanceExtensions(NULL, extension_count, NULL));
	extensions = util_alloc(*extension_count, sizeof(const char *), NULL);
	SDL_Vulkan_GetInstanceExtensions(NULL, extension_count, extensions);

	return extensions;
}

// Debug messenger callback
VkBool32 VKAPI_CALL debug_messenger_callback(VkDebugUtilsMessageSeverityFlagBitsEXT severity,
					     VkDebugUtilsMessageTypeFlagsEXT types,
					     const VkDebugUtilsMessengerCallbackDataEXT *data, void *user)
{
	char *severity_str;
	char *type_str;

	type_str = NULL;
	if (types & VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT)
		type_str = util_append(type_str, "GENERAL ");
	if (types & VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT)
		type_str = util_append(type_str, "VALIDATION ");
	if (types & VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT)
		type_str = util_append(type_str, "PERFORMANCE ");
	if (!type_str)
		type_str = util_alloc(1, sizeof(char), NULL);

	severity_str = NULL;
	if (severity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT)
		severity_str = util_append(severity_str, "VERBOSE ");
	if (severity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT)
		severity_str = util_append(severity_str, "INFO ");
	if (severity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT)
		severity_str = util_append(severity_str, "WARNING ");
	if (severity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT)
		severity_str = util_append(severity_str, "ERROR ");
	if (!severity_str)
		severity_str = util_alloc(1, sizeof(char), NULL);

	PURPL_LOG(RENDER_LOG_PREFIX "VULKAN %s%sMESSAGE: %s\n", type_str, severity_str, data->pMessage);

	free(type_str);
	free(severity_str);

	return true;
}

bool engine_vulkan_init(void)
{
	VkResult result;
	VkApplicationInfo app_info = { 0 };
	VkDebugUtilsMessengerCreateInfoEXT debug_messenger_info = { 0 };
	VkInstanceCreateInfo inst_info = { 0 };
	const char **extensions;
	uint32_t extension_count;
	uint32_t i;

	const char *validation_layers[] = { "VK_LAYER_KHRONOS_validation" };

	PURPL_LOG(RENDER_LOG_PREFIX "Loading Vulkan\n");
	SDL_Vulkan_LoadLibrary(NULL);
	gladLoaderLoadVulkan(NULL, NULL, NULL);

	PURPL_LOG(RENDER_LOG_PREFIX "Setting VkApplicationInfo fields\n");
	app_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	app_info.pEngineName = "Purpl Engine";
	app_info.pApplicationName = g_engine->game->game;
	app_info.apiVersion = VK_API_VERSION_1_3;
	app_info.applicationVersion = VK_MAKE_API_VERSION(0, PURPL_VERSION_MAJOR(g_engine->game->version),
							  PURPL_VERSION_MINOR(g_engine->game->version),
							  PURPL_VERSION_PATCH(g_engine->game->version));
	app_info.engineVersion = VK_MAKE_API_VERSION(0, PURPL_VERSION_MAJOR(PURPL_VERSION),
						     PURPL_VERSION_MINOR(PURPL_VERSION),
						     PURPL_VERSION_PATCH(PURPL_VERSION));

	PURPL_LOG(RENDER_LOG_PREFIX "Setting VkDebugUtilsMessengerCreateInfoEXT fields\n");
	debug_messenger_info.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
	debug_messenger_info.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_FLAG_BITS_MAX_ENUM_EXT;
	debug_messenger_info.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_FLAG_BITS_MAX_ENUM_EXT;
	debug_messenger_info.pfnUserCallback = debug_messenger_callback;

	inst_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	inst_info.pApplicationInfo = &app_info;
	inst_info.pNext = &debug_messenger_info;

	PURPL_LOG(RENDER_LOG_PREFIX "Getting list of extensions\n");
	extension_count = 0;
	extensions = get_extensions(&extension_count);
	for (i = 0; i < extension_count; i++)
		PURPL_LOG(RENDER_LOG_PREFIX "\t%s\n", extensions[i]);
	inst_info.ppEnabledExtensionNames = extensions;
	inst_info.enabledExtensionCount = extension_count;
	inst_info.ppEnabledLayerNames = validation_layers;
	// Set the number of layers in devmode, otherwise it'll be zero and they'll be ignored
	if (g_engine->dev)
		inst_info.enabledLayerCount = PURPL_ARRSIZE(validation_layers);

	PURPL_LOG(RENDER_LOG_PREFIX "Creating VkInstance\n");
	result = vkCreateInstance(&inst_info, NULL, &g_vulkan_inst);
	free(extensions);
	if (result != VK_SUCCESS) {
		PURPL_LOG(RENDER_LOG_PREFIX "Failed to create VkInstance: VkResult %d\n", result);
		engine_vulkan_shutdown();
		return false;
	}

	return true;
}

bool engine_vulkan_begin_frame(uint64_t delta)
{
	return true;
}

bool engine_vulkan_end_frame(uint64_t delta)
{
	return true;
}

void engine_vulkan_shutdown(void)
{
	if (g_vulkan_inst) {
		PURPL_LOG(RENDER_LOG_PREFIX "Destroying VkInstance\n");
		vkDestroyInstance(g_vulkan_inst, NULL);
	}
}
