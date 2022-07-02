-- This file contains options for the engine's build

-- Vulkan SDK location
if _TARGET_OS == "windows" then
	PURPL_VULKAN_SDK = "C:/VulkanSDK/1.3.216.0"
else
	PURPL_VULKAN_SDK = "/usr"
end

-- The name of the launcher executable and the game
PURPL_GAME_NAME = "purpl"
