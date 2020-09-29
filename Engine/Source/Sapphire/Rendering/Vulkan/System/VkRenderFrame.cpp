// Copyright 2020 Sapphire development team. All Rights Reserved.

#include <Rendering/Vulkan/System/VkRenderFrame.hpp>

#if SA_RENDERING_API == SA_VULKAN

namespace Sa
{
	VkRenderFrame::VkRenderFrame(uint32 _frameIndex,
		VkImage _image,
		VkImageView _imageView,
		VkFramebuffer _frameBuffer,
		VkCommandBuffer _graphicsCommandBuffer,
		VkSemaphore _acquireSemaphores,
		VkSemaphore _presentSemaphores,
		VkFence _mainFence,
		VkUniformBuffer _uniformBuffer) noexcept :
		frameIndex{ _frameIndex },
		image{ _image },
		imageView{ _imageView },
		frameBuffer{ _frameBuffer },
		graphicsCommandBuffer{ _graphicsCommandBuffer },
		acquireSemaphores{ _acquireSemaphores },
		presentSemaphores{ _presentSemaphores },
		mainFence{ _mainFence },
		uniformBuffer{ _uniformBuffer }
	{
	}
}

#endif
