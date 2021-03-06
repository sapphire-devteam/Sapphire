// Copyright 2020 Sapphire development team. All Rights Reserved.

// TODO: CLEAN LATER.
#include <iostream>

#include <Rendering/Vulkan/System/VkValidationLayers.hpp>

#if SA_VK_VALIDATION_LAYERS

namespace Sa::Vk
{
	static constexpr const char* validationLayers[] =
	{
		"VK_LAYER_KHRONOS_validation"
	};

	constexpr uint32 validationLayersSize = sizeof(validationLayers) / sizeof(char*);

	VkDebugUtilsMessengerCreateInfoEXT ValidationLayers::GetDebugUtilsMessengerCreateInfo() noexcept
	{
		// Debug Messenger Info.
		VkDebugUtilsMessengerCreateInfoEXT debugInfo{};

		debugInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
		debugInfo.pNext = nullptr;
		debugInfo.flags = 0u;

		debugInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
			VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
			VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;

		debugInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
			VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
			VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;

		debugInfo.pfnUserCallback = DebugCallback;
		debugInfo.pUserData = nullptr;

		return debugInfo;
	}

	uint32 ValidationLayers::GetLayerNum() noexcept
	{
		return validationLayersSize;
	}

	const char* const* ValidationLayers::GetLayerNames() noexcept
	{
		return validationLayers;
	}

	bool ValidationLayers::CheckValidationSupport() noexcept
	{
		// Query currently supported layers.
		uint32_t layerCount;
		vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

		std::vector<VkLayerProperties> availableLayers(layerCount);
		vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());


		// Check each asked supported.
		for (uint32 i = 0; i < validationLayersSize; ++i)
		{
			bool layerFound = false;

			for (uint32 j = 0; j < availableLayers.size(); ++j)
			{
				// Layer found.
				if (strcmp(validationLayers[i], availableLayers[j].layerName) == 0)
				{
					layerFound = true;
					break;
				}
			}

			// Layer not found.
			if (!layerFound)
				return false;
		}

		return true;
	}

	VKAPI_ATTR VkBool32 VKAPI_CALL ValidationLayers::DebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
		VkDebugUtilsMessageTypeFlagsEXT messageType,
		const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
		void* pUserData)
	{
		(void)messageSeverity;
		(void)messageType;
		(void)pCallbackData;
		(void)pUserData;

		// TODO: FIX.
		//switch (messageSeverity)
		//{
		//	case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
		//		SA_LOG(pCallbackData->pMessage, Infos, Rendering);
		//		break;
		//	case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
		//		SA_LOG(pCallbackData->pMessage, Warning, Rendering);
		//		break;
		//	case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
		//		SA_LOG(pCallbackData->pMessage, Error, Rendering);
		//		break;
		//	default:
		//		SA_LOG(pCallbackData->pMessage, Normal, Rendering);
		//		break;
		//}

		std::cerr << "validation layer: " << pCallbackData->pMessage << std::endl;

		return VK_FALSE;
	}
}

#endif
