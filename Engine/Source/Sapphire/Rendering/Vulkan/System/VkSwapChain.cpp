// Copyright 2020 Sapphire development team. All Rights Reserved.

#include <Rendering/Vulkan/System/VkSwapChain.hpp>

#include <Core/Algorithms/SizeOf.hpp>

#include <Rendering/Vulkan/System/VkMacro.hpp>
#include <Rendering/Vulkan/System/VkRenderSurface.hpp>
#include <Rendering/Vulkan/System/Device/VkDevice.hpp>

namespace Sa::Vk
{
	RenderFormat SwapChain::GetRenderFormat() const noexcept
	{
		return mFormat;
	}

	void SwapChain::Create(const Device& _device, const RenderSurface& _surface)
	{
		CreateSwapChainKHR(_device, _surface);
	}

	void SwapChain::Destroy(const Device& _device)
	{
		DestroySwapChainKHR(_device);
	}

	void SwapChain::CreateSwapChainKHR(const Device& _device, const RenderSurface& _surface)
	{
		// Query infos.
		RenderSurface::SupportDetails details = _surface.QuerySupportDetails(_device);
		VkSurfaceFormatKHR surfaceFormat = RenderSurface::ChooseSwapSurfaceFormat(details);
		VkPresentModeKHR presentMode = RenderSurface::ChooseSwapPresentMode(details);
		mExtent = RenderSurface::ChooseSwapExtent(details);

		mFormat = API_FromRenderFormat(surfaceFormat.format);

		// Min image count to avoid driver blocking.
		mImageNum = details.capabilities.minImageCount + 1;

		if (details.capabilities.maxImageCount > 0 && mImageNum > details.capabilities.maxImageCount)
			mImageNum = details.capabilities.maxImageCount;

		// Create Swapchain.
		VkSwapchainCreateInfoKHR swapChainCreateInfo{};
		swapChainCreateInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
		swapChainCreateInfo.pNext = nullptr;
		swapChainCreateInfo.flags = 0u;
		swapChainCreateInfo.surface = _surface;
		swapChainCreateInfo.minImageCount = mImageNum;
		swapChainCreateInfo.imageFormat = surfaceFormat.format;
		swapChainCreateInfo.imageColorSpace = surfaceFormat.colorSpace;
		swapChainCreateInfo.imageExtent = VkExtent2D{ mExtent.x, mExtent.y };
		swapChainCreateInfo.imageArrayLayers = 1u;
		swapChainCreateInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
		swapChainCreateInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
		swapChainCreateInfo.queueFamilyIndexCount = 0u;
		swapChainCreateInfo.pQueueFamilyIndices = nullptr;
		swapChainCreateInfo.preTransform = details.capabilities.currentTransform;
		swapChainCreateInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
		swapChainCreateInfo.presentMode = presentMode;
		swapChainCreateInfo.clipped = VK_TRUE;
		swapChainCreateInfo.oldSwapchain = VK_NULL_HANDLE;

		// Queue family setup.
		uint32 familyIndices[]
		{
			_device.queueMgr.graphics.GetFamilyIndex(),
			_device.queueMgr.present.GetFamilyIndex(),
		};

		if (_device.queueMgr.present.IsValid() && familyIndices[0] != familyIndices[1]) // Graphic and present familiy are different.
		{
			swapChainCreateInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
			swapChainCreateInfo.queueFamilyIndexCount = SizeOf(familyIndices);
			swapChainCreateInfo.pQueueFamilyIndices = familyIndices;
		}

		SA_VK_ASSERT(vkCreateSwapchainKHR(_device, &swapChainCreateInfo, nullptr, &mHandle),
			CreationFailed, Rendering, L"Failed to create swap chain!");
	}

	void SwapChain::DestroySwapChainKHR(const Device& _device)
	{
		SA_ASSERT(mHandle != VK_NULL_HANDLE, InvalidParam, Rendering, L"Destroy a null swapchain!");

		vkDestroySwapchainKHR(_device, mHandle, nullptr);
		mHandle = VK_NULL_HANDLE;
	}


	void SwapChain::AddRenderPass(const Device& _device, const RenderPass& _renderPass, const RenderPassDescriptor& _rpDesc)
	{
		std::vector<VkImage> swapChainImages(mImageNum);
		vkGetSwapchainImagesKHR(_device, mHandle, &mImageNum, swapChainImages.data());

		std::vector<FrameBuffer>& frameBuffers = frameBufferImages.emplace_back();
		frameBuffers.reserve(mImageNum);

		for (uint32 i = 0u; i < mImageNum; ++i)
		{
			FrameBuffer& frameBuffer = frameBuffers.emplace_back();
			frameBuffer.Create(_device, _renderPass, _rpDesc, mExtent, swapChainImages[i]);
		}
	}
	
	void SwapChain::RemoveRenderPass(const Device& _device, const RenderPass& _renderPass)
	{
		// TODO: Implement.
	}
}
