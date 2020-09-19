// Copyright 2020 Sapphire development team. All Rights Reserved.

#include <Rendering/Vulkan/Primitives/VkSwapChain.hpp>

#include <Maths/Misc/Maths.hpp>

#include <Rendering/Vulkan/VkMacro.hpp>
#include <Rendering/Vulkan/Primitives/VkDevice.hpp>
#include <Rendering/Vulkan/Primitives/VkRenderPass.hpp>
#include <Rendering/Vulkan/Primitives/VkRenderSurface.hpp>
#include <Rendering/Vulkan/Queue/VkCommandBuffer.hpp>

#include <Rendering/Framework/Model/UniformBufferObject.hpp>

#if SA_RENDERING_API == SA_VULKAN

namespace Sa
{
	uint32 VkSwapChain::GetImageNum() const noexcept
	{
		return static_cast<uint32>(mImages.size());
	}

	VkFormat VkSwapChain::GetImageFormat() const noexcept
	{
		return mImageFormat;
	}

	const ImageExtent& VkSwapChain::GetImageExtent() const noexcept
	{
		return mExtent;
	}

	const std::vector<VkBuffer>& VkSwapChain::GetUniformBuffers() const noexcept
	{
		return mUniformBuffers;
	}

	VkRenderFrame VkSwapChain::GetRenderFrame() const noexcept
	{
		return VkRenderFrame
		{
			mFrameIndex,
			mImages[mImageIndex],
			mImageViews[mImageIndex],
			mFrameBuffers[mFrameIndex],
			mGraphicsCommandBuffers[mFrameIndex],
			mAcquireSemaphores[mFrameIndex],
			mPresentSemaphores[mFrameIndex],
			mMainFences[mFrameIndex],
			mUniformBuffers[mFrameIndex]
		};
	}

	void VkSwapChain::Create(const VkDevice& _device, const VkRenderSurface& _surface, const VkQueueFamilyIndices& _queueFamilyIndices)
	{
		// Query infos.
		SupportDetails swapChainSupport = QuerySupportDetails(_device, _surface);

		VkSurfaceFormatKHR surfaceFormat = ChooseSwapSurfaceFormat(swapChainSupport.formats);
		VkPresentModeKHR presentMode = ChooseSwapPresentMode(swapChainSupport.presentModes);

		mImageFormat = surfaceFormat.format;
		mExtent = ChooseSwapExtent(swapChainSupport.capabilities);


		// Min image count to avoid driver blocking.
		uint32 imageNum = swapChainSupport.capabilities.minImageCount + 1;

		if (swapChainSupport.capabilities.maxImageCount > 0 && imageNum > swapChainSupport.capabilities.maxImageCount)
			imageNum = swapChainSupport.capabilities.maxImageCount;


		VkSwapchainCreateInfoKHR swapChainCreateInfo
		{
			VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,					// sType.
			nullptr,														// pNext.
			0,																// flags.
			_surface,														// surface.
			imageNum,														// minImageCount.
			surfaceFormat.format,											// imageFormat.
			surfaceFormat.colorSpace,										// imageColorSpace.
			mExtent,														// imageExtent.
			1,																// imageArrayLayers.
			VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,							// imageUsage.
			VK_SHARING_MODE_EXCLUSIVE, 										// imageSharingMode.
			0,																// queueFamilyIndexCount.
			nullptr,														// pQueueFamilyIndices.
			swapChainSupport.capabilities.currentTransform,					// preTransform.
			VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,								// compositeAlpha.
			presentMode,													// presentMode.
			VK_TRUE,														// clipped.
			VK_NULL_HANDLE													// oldSwapchain.
		};

		if (_queueFamilyIndices.graphicsFamily != _queueFamilyIndices.presentFamily)
		{
			swapChainCreateInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
			swapChainCreateInfo.queueFamilyIndexCount = 2;
			swapChainCreateInfo.pQueueFamilyIndices = &_queueFamilyIndices.graphicsFamily;
		}

		SA_VK_ASSERT(vkCreateSwapchainKHR(_device, &swapChainCreateInfo, nullptr, &mHandle),
			CreationFailed, Rendering, L"Failed to create swap chain!");


		// Create images.
		vkGetSwapchainImagesKHR(_device, mHandle, &imageNum, nullptr);
		mImages.resize(imageNum);
		vkGetSwapchainImagesKHR(_device, mHandle, &imageNum, mImages.data());


		// Create Image views.
		mImageViews.resize(imageNum);

		for (uint32 i = 0; i < imageNum; i++)
		{
			const VkImageViewCreateInfo imageViewCreateInfo
			{
				VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,									// sType.
				nullptr,																	// pNext.
				VK_IMAGE_VIEW_CREATE_FRAGMENT_DENSITY_MAP_DYNAMIC_BIT_EXT,					// flags.
				mImages[i],																	// image.
				VK_IMAGE_VIEW_TYPE_2D,														// viewType.
				mImageFormat,																// format.

				VkComponentMapping															// components.
				{
					VK_COMPONENT_SWIZZLE_IDENTITY,										// r.
					VK_COMPONENT_SWIZZLE_IDENTITY,										// g.
					VK_COMPONENT_SWIZZLE_IDENTITY,										// b.
					VK_COMPONENT_SWIZZLE_IDENTITY										// a.
				},
				VkImageSubresourceRange														// subresourceRange.
				{
					VK_IMAGE_ASPECT_COLOR_BIT,											// aspectMask.
					0,																	// baseMipLevel.
					1,																	// levelCount.
					0,																	// baseArrayLayer.
					1																	// layerCount.
				}
			};

			SA_VK_ASSERT(vkCreateImageView(_device, &imageViewCreateInfo, nullptr, &mImageViews[i]),
				CreationFailed, Rendering, L"Failed to create image views!");
		}


		// Create Command Buffers.
		mGraphicsCommandBuffers.resize(imageNum);

		const VkCommandBufferAllocateInfo commandBufferAllocInfo
		{
			VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,							// sType.
			nullptr,																// nullptr.
			_device.GetGraphicsQueue().GetCommandPool(),							// commandPool.
			VK_COMMAND_BUFFER_LEVEL_PRIMARY,										// level.
			imageNum																// commandBufferCount.
		};

		SA_VK_ASSERT(vkAllocateCommandBuffers(_device, &commandBufferAllocInfo, mGraphicsCommandBuffers.data()),
			CreationFailed, Rendering, L"Failed to allocate command buffers!");


		// Uniform buffers creation.
		mUniformBuffers.resize(imageNum);
		
		for (uint32 i = 0; i < imageNum; i++)
		{
			mUniformBuffers[i].Create(_device, sizeof(UniformBufferObject), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
				VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
		}


		// Semaphore Creation.
		mAcquireSemaphores.resize(imageNum);
		mPresentSemaphores.resize(imageNum);

		const VkSemaphoreCreateInfo semaphoreCreateInfo
		{
			VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,					// sType.
			nullptr,													// pNext.
			0,															// flags.
		};

		for (uint32 i = 0u; i < imageNum; ++i)
		{
			SA_VK_ASSERT(vkCreateSemaphore(_device, &semaphoreCreateInfo, nullptr, &mAcquireSemaphores[i]),
				CreationFailed, Rendering, L"Failed to create semaphore!");
			SA_VK_ASSERT(vkCreateSemaphore(_device, &semaphoreCreateInfo, nullptr, &mPresentSemaphores[i]),
				CreationFailed, Rendering, L"Failed to create semaphore!");
		}


		// Fence Creation.
		mMainFences.resize(imageNum);

		const VkFenceCreateInfo fenceCreateInfo
		{
			VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,					// sType.
			nullptr,												// pNext.
			VK_FENCE_CREATE_SIGNALED_BIT							// flags.
		};

		for (uint32 i = 0u; i < imageNum; ++i)
		{
			SA_VK_ASSERT(vkCreateFence(_device, &fenceCreateInfo, nullptr, &mMainFences[i]),
				CreationFailed, Rendering, L"Failed to create fence!");
		}

		CreateDepthResources(_device);
	}
	void VkSwapChain::Destroy(const VkDevice& _device)
	{
		// Destroy Semaphores and Fences.
		const uint32 imageNum = GetImageNum();

		for (uint32 i = 0; i < imageNum; i++)
		{
			vkDestroyFence(_device, mMainFences[i], nullptr);

			vkDestroySemaphore(_device, mAcquireSemaphores[i], nullptr);
			vkDestroySemaphore(_device, mPresentSemaphores[i], nullptr);

			mUniformBuffers[i].Destroy(_device);

			vkDestroyImageView(_device, mImageViews[i], nullptr);
		}

		DestroyFrameBuffers(_device);

		DestroyDepthResources(_device);

		// Manually free command buffers (useful for resize).
		vkFreeCommandBuffers(_device, _device.GetGraphicsQueue().GetCommandPool(), imageNum, mGraphicsCommandBuffers.data());

		vkDestroySwapchainKHR(_device, mHandle, nullptr);
	}

	void VkSwapChain::ReCreate(const VkDevice& _device, const VkRenderSurface& _surface, const VkQueueFamilyIndices& _queueFamilyIndices)
	{
		Destroy(_device);

		Create(_device, _surface, _queueFamilyIndices);
	}

	void VkSwapChain::CreateFrameBuffers(const VkDevice& _device, const VkRenderPass& _renderPass)
	{
		mFrameBuffers.resize(GetImageNum());

		for (uint32 i = 0u; i < mFrameBuffers.size(); ++i)
		{
			const VkImageView attachements[]{ mImageViews[i], mDepthImageView };
		
			const VkFramebufferCreateInfo framebufferCreateInfo
			{
				VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,			// sType.
				nullptr,											// pNext.
				0,													// flags.
				_renderPass,							            // renderPass.
				sizeof(attachements) / sizeof(VkImageView),			// attachmentCount.
				attachements,										// pAttachments.
				mExtent.width,										// width.
				mExtent.height,										// height.
				1													// layers.

			};

			SA_VK_ASSERT(vkCreateFramebuffer(_device, &framebufferCreateInfo, nullptr, &mFrameBuffers[i]),
				CreationFailed, Rendering, L"Failed to create framebuffer!");
		}
	}
	void VkSwapChain::DestroyFrameBuffers(const VkDevice& _device)
	{
		for (uint32 i = 0u; i < mFrameBuffers.size(); ++i)
			vkDestroyFramebuffer(_device, mFrameBuffers[i], nullptr);

		mFrameBuffers.clear();
	}

	VkRenderFrame VkSwapChain::Update(const VkDevice& _device)
	{
		// Get current frame components.
		VkRenderFrame frame = GetRenderFrame();


		const VkPipelineStageFlags waitStages = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;

		// Submit previous graphic.
		const VkSubmitInfo submitInfo
		{
			VK_STRUCTURE_TYPE_SUBMIT_INFO,						// sType.
			nullptr,											// pNext.
			1,													// waitSemaphoreCount.
			&frame.acquireSemaphores,							// pWaitSemaphores.
			&waitStages,										// pWaitDstStageMask.
			1,													// commandBufferCount.
			&frame.graphicsCommandBuffer,						// pCommandBuffers.
			1,													// signalSemaphoreCount.
			&frame.presentSemaphores,							// pSignalSemaphores.
		};

		SA_VK_ASSERT(vkQueueSubmit(_device.GetGraphicsQueue(), 1, &submitInfo, frame.mainFence),
			LibCommandFailed, Rendering, L"Failed to submit graphics queue!");

		// Submit previous present.
		const VkPresentInfoKHR presentInfo
		{
			VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,					// sType.
			nullptr,											// pNext.
			1,													// waitSemaphoreCount.
			&frame.presentSemaphores,							// pWaitSemaphores.
			1,													// swapchainCount.
			&mHandle,											// pSwapchains
			&mImageIndex,										// pImageIndices.
			nullptr												// pResults.
		};

		SA_VK_ASSERT(vkQueuePresentKHR(_device.GetPresentQueue(), &presentInfo),
			LibCommandFailed, Rendering, L"Failed to submit present queue!");


		// Increment new frame.
		mFrameIndex = (mFrameIndex + 1) % GetImageNum();

		// Get new current frame components.
		frame = GetRenderFrame();


		// Wait current Fence.
		vkWaitForFences(_device, 1, &frame.mainFence, true, UINT64_MAX);

		// Reset current Fence.
		vkResetFences(_device, 1, &frame.mainFence);

		// Reset Command Buffer.
		vkResetCommandBuffer(frame.graphicsCommandBuffer, VK_COMMAND_BUFFER_RESET_RELEASE_RESOURCES_BIT);

		SA_VK_ASSERT(vkAcquireNextImageKHR(_device, mHandle, UINT64_MAX, frame.acquireSemaphores, VK_NULL_HANDLE, &mImageIndex),
			LibCommandFailed, Rendering, L"Failed to aquire next image!");

		return frame;
	}

	VkSurfaceFormatKHR VkSwapChain::ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats)
	{
		// Prefered
		for (uint32 i = 0; i < availableFormats.size(); ++i)
		{
			if (availableFormats[i].format == VK_FORMAT_B8G8R8A8_SRGB && availableFormats[i].colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
				return availableFormats[i];
		}

		// Default.
		return availableFormats[0];
	}

	VkPresentModeKHR VkSwapChain::ChooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes)
	{
		// Prefered.
		for (uint32 i = 0; i < availablePresentModes.size(); ++i)
		{
			if (availablePresentModes[i] == VK_PRESENT_MODE_MAILBOX_KHR)
				return availablePresentModes[i];
		}

		// Default FIFO always supported.
		return VK_PRESENT_MODE_FIFO_KHR;
	}

	ImageExtent VkSwapChain::ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities)
	{
		if (capabilities.currentExtent.width != UINT32_MAX)
			return capabilities.currentExtent;
		else
		{
			// TODO: Clean.
			VkExtent2D actualExtent = { 1920, 1080 };

			actualExtent.width = Maths::Max(capabilities.minImageExtent.width, Maths::Min(capabilities.maxImageExtent.width, actualExtent.width));
			actualExtent.height = Maths::Max(capabilities.minImageExtent.height, Maths::Min(capabilities.maxImageExtent.height, actualExtent.height));

			return actualExtent;
		}
	}

	VkSwapChain::SupportDetails VkSwapChain::QuerySupportDetails(VkPhysicalDevice _device, const VkRenderSurface& _surface)
	{
		SupportDetails details;

		// KHR Capabilities.
		vkGetPhysicalDeviceSurfaceCapabilitiesKHR(_device, _surface, &details.capabilities);


		// KHR Formats.
		uint32 formatCount = 0u;
		vkGetPhysicalDeviceSurfaceFormatsKHR(_device, _surface, &formatCount, nullptr);

		if (formatCount != 0)
		{
			details.formats.resize(formatCount);
			vkGetPhysicalDeviceSurfaceFormatsKHR(_device, _surface, &formatCount, details.formats.data());
		}


		// KHR Present Modes.
		uint32 presentModeCount = 0u;
		vkGetPhysicalDeviceSurfacePresentModesKHR(_device, _surface, &presentModeCount, nullptr);

		if (presentModeCount != 0)
		{
			details.presentModes.resize(presentModeCount);
			vkGetPhysicalDeviceSurfacePresentModesKHR(_device, _surface, &presentModeCount, details.presentModes.data());
		}

		return details;
	}

	bool VkSwapChain::CheckSupport(VkPhysicalDevice _device, const VkRenderSurface& _surface)
	{
		SupportDetails details = QuerySupportDetails(_device, _surface);

		return !details.formats.empty() && !details.presentModes.empty();
	}

	void VkSwapChain::CreateDepthResources(const VkDevice& _device)
	{
		// TODO: Add flexibility. See https://vulkan-tutorial.com/Depth_buffering.

		const VkImageCreateInfo imageCreateInfo
		{
			VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,					// sType.
			nullptr,												// pNext.
			0,														// flags.
			VK_IMAGE_TYPE_2D,										// imageType.
			VK_FORMAT_D32_SFLOAT,									// format.

			mExtent,

			1,														// mipLevels.
			1,														// arrayLayers.
			VK_SAMPLE_COUNT_1_BIT,									// samples.
			VK_IMAGE_TILING_OPTIMAL,								// tiling.

			VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,			// usage.

			VK_SHARING_MODE_EXCLUSIVE,								// sharingMode.
			VkQueueFamilyIndices::familyNum,						// queueFamilyIndexCount.
			_device.GetQueueFamilyIndices().GetFamilies(),			// pQueueFamilyIndices.
			VK_IMAGE_LAYOUT_UNDEFINED,								// initialLayout.
		};

		SA_VK_ASSERT(vkCreateImage(_device, &imageCreateInfo, nullptr, &mDepthImage),
			CreationFailed, Rendering, L"Failed to create death image!")


		VkMemoryRequirements memRequirements;
		vkGetImageMemoryRequirements(_device, mDepthImage, &memRequirements);

		uint32 memoryTypeIndex = VkBuffer::FindMemoryType(_device, memRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

		const VkMemoryAllocateInfo memoryAllocInfo
		{
			VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,					// sType.
			nullptr,												// pNext.
			memRequirements.size,									// allocationSize.
			memoryTypeIndex,										// memoryTypeIndex.
		};

		SA_VK_ASSERT(vkAllocateMemory(_device, &memoryAllocInfo, nullptr, &mDepthImageMemory),
			MemoryAllocationFailed, Rendering, L"Failed to allocate image memory!");

		vkBindImageMemory(_device, mDepthImage, mDepthImageMemory, 0);


		// Create image view.
		const VkImageViewCreateInfo imageViewCreateInfo
		{
			VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,									// sType.
			nullptr,																	// pNext.
			VK_IMAGE_VIEW_CREATE_FRAGMENT_DENSITY_MAP_DYNAMIC_BIT_EXT,					// flags.
			mDepthImage,																// image.
			VK_IMAGE_VIEW_TYPE_2D,														// viewType.
			VK_FORMAT_D32_SFLOAT,														// format.

			VkComponentMapping															// components.
			{
				VK_COMPONENT_SWIZZLE_IDENTITY,										// r.
				VK_COMPONENT_SWIZZLE_IDENTITY,										// g.
				VK_COMPONENT_SWIZZLE_IDENTITY,										// b.
				VK_COMPONENT_SWIZZLE_IDENTITY										// a.
			},
			VkImageSubresourceRange														// subresourceRange.
			{
				VK_IMAGE_ASPECT_DEPTH_BIT,											// aspectMask.
				0,																	// baseMipLevel.
				1,																	// levelCount.
				0,																	// baseArrayLayer.
				1																	// layerCount.
			}
		};

		SA_VK_ASSERT(vkCreateImageView(_device, &imageViewCreateInfo, nullptr, &mDepthImageView),
			CreationFailed, Rendering, L"Failed to create image views!");


		// Transition.
		Sa::VkCommandBuffer commandBuffer = VkCommandBuffer::BeginSingleTimeCommands(_device, _device.GetGraphicsQueue());


		VkImageMemoryBarrier barrier
		{
			VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,							// sType.
			nullptr,														// pNext.
			0,																// srcAccessMask.
			
			VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT |					// dstAccessMask.
			VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT,
			
			VK_IMAGE_LAYOUT_UNDEFINED,										// oldLayout.
			VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,				// newLayout.
			VK_QUEUE_FAMILY_IGNORED,										// srcQueueFamilyIndex.
			VK_QUEUE_FAMILY_IGNORED,										// dstQueueFamilyIndex.
			mDepthImage,													// image.

			VkImageSubresourceRange											// subresourceRange.
			{
				VK_IMAGE_ASPECT_DEPTH_BIT,						// aspectMask.
				0,												// baseMipLevel.
				1,												// levelCount.
				0,												// baseArrayLayer.
				1,												// layerCount.
			}
		};

		vkCmdPipelineBarrier(commandBuffer,
			VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
			VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT,
			0, 0, nullptr, 0, nullptr, 1, &barrier
		);


		VkCommandBuffer::EndSingleTimeCommands(_device, commandBuffer, _device.GetGraphicsQueue());
	}

	void VkSwapChain::DestroyDepthResources(const VkDevice& _device)
	{
		vkDestroyImageView(_device, mDepthImageView, nullptr);

		vkDestroyImage(_device, mDepthImage, nullptr);

		vkFreeMemory(_device, mDepthImageMemory, nullptr);
	}
}

#endif
