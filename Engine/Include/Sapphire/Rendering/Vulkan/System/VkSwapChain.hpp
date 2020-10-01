// Copyright 2020 Sapphire development team. All Rights Reserved.

#pragma once

#ifndef SAPPHIRE_RENDERING_VK_SWAP_CHAIN_GUARD
#define SAPPHIRE_RENDERING_VK_SWAP_CHAIN_GUARD

#include <vector>

#include <Core/Types/Int.hpp>

#include <Rendering/Image/ImageExtent.hpp>
#include <Rendering/Vulkan/System/VkRenderFrame.hpp>

#if SA_RENDERING_API == SA_VULKAN

namespace Sa
{
	class VkDevice;
	class VkRenderSurface;
	class VkRenderPass;
	class VkQueueFamilyIndices;

	class VkSwapChain
	{
		VkSwapchainKHR mHandle = VK_NULL_HANDLE;

		ImageExtent mExtent;
		VkFormat mImageFormat;

		uint32 mFrameIndex = 0u;
		uint32 mImageIndex = 0u;

		VkImage mDepthImage = VK_NULL_HANDLE;
		VkDeviceMemory mDepthImageMemory = VK_NULL_HANDLE;
		VkImageView mDepthImageView = VK_NULL_HANDLE;

		std::vector<VkImage> mImages;
		std::vector<VkImageView> mImageViews;

		std::vector<VkFramebuffer> mFrameBuffers;
		std::vector<VkCommandBuffer> mGraphicsCommandBuffers;

		std::vector<VkSemaphore> mAcquireSemaphores;
		std::vector<VkSemaphore> mPresentSemaphores;

		std::vector<VkFence> mMainFences;

		std::vector<VkUniformBuffer> mUniformBuffers;

		struct SupportDetails
		{
			VkSurfaceCapabilitiesKHR capabilities;
			std::vector<VkSurfaceFormatKHR> formats;
			std::vector<VkPresentModeKHR> presentModes;
		};

		static SupportDetails QuerySupportDetails(VkPhysicalDevice _device, const VkRenderSurface& _surface);
		VkSurfaceFormatKHR ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
		VkPresentModeKHR ChooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
		ImageExtent ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);

		uint32 CreateSwapChainKHR(const VkDevice& _device, const VkRenderSurface& _surface, const VkQueueFamilyIndices& _queueFamilyIndices);
		void DestroySwapChainKHR(const VkDevice& _device);

		void CreateImageView(const VkDevice& _device, uint32 _imageNum);
		void DestroyImageView(const VkDevice& _device);

		void CreateCommandBuffers(const VkDevice& _device, uint32 _imageNum);
		void DestroyCommandBuffers(const VkDevice& _device);

		void CreateUniformBuffers(const VkDevice& _device, uint32 _imageNum);
		void DestroyUniformBuffers(const VkDevice& _device);

		void CreateDepthResources(const VkDevice& _device);
		void DestroyDepthResources(const VkDevice& _device);

		void CreateSemaphores(const VkDevice& _device, uint32 _imageNum);
		void DestroySemaphores(const VkDevice& _device);

		void CreateFences(const VkDevice& _device, uint32 _imageNum);
		void DestroyFences(const VkDevice& _device);

	public:
		uint32 GetImageNum() const noexcept;
		VkFormat GetImageFormat() const noexcept;

		const ImageExtent& GetImageExtent() const noexcept;

		const std::vector<VkUniformBuffer>& GetUniformBuffers() const noexcept;

		VkRenderFrame GetRenderFrame() const noexcept;

		void Create(const VkDevice& _device, const VkRenderSurface& _surface, const VkQueueFamilyIndices& _queueFamilyIndices);
		void Destroy(const VkDevice& _device);

		// Optimized re-creation.
		void ReCreate(const VkDevice& _device, const VkRenderSurface& _surface, const VkQueueFamilyIndices& _queueFamilyIndices);

		void CreateFrameBuffers(const VkDevice& _device, const VkRenderPass& _renderPass);
		void DestroyFrameBuffers(const VkDevice& _device);

		// TODO: Remove SA_ENGINE_API.
		SA_ENGINE_API VkRenderFrame Update(const VkDevice& _device);

		static bool CheckSupport(VkPhysicalDevice _device, const VkRenderSurface& _surface);
	};
}

#endif

#endif // GUARD