// Copyright 2020 Sapphire development team. All Rights Reserved.

#include <Rendering/Vulkan/Primitives/Texture/VkTexture.hpp>

#include <Rendering/Vulkan/System/VkRenderInstance.hpp>
#include <Rendering/Vulkan/Buffer/VkBuffer.hpp>
#include <Rendering/Vulkan/Buffer/VkCommandBuffer.hpp>

#if SA_RENDERING_API == SA_VULKAN

namespace Sa
{
	void VkTexture::Create(const IRenderInstance& _instance, const RawTexture& _rawTexture)
	{
		VkFormat format = API_GetFormat(_rawTexture.channel);
		uint32 textureSize = _rawTexture.GetSize();

		const VkDevice& device = _instance.As<VkRenderInstance>().GetDevice();

		VkBuffer stagingBuffer;
		stagingBuffer.Create(device, textureSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
			_rawTexture.data);

		VkExtent3D textureExtent{ _rawTexture.width, _rawTexture.height, 1 };

		uint32 mipLevels = 1u;
		VkImageBufferCreateInfos imageBufferCreateInfos;

		if(_rawTexture.type == TextureType::Simple)
			mipLevels = ComputeMipMapLevels(_rawTexture.width, _rawTexture.height);
		else if (_rawTexture.type == TextureType::Cubemap)
			imageBufferCreateInfos = VkImageBufferCreateInfos::CreateCubeMapInfos();

		imageBufferCreateInfos.format = format;
		imageBufferCreateInfos.extent = textureExtent;

		imageBufferCreateInfos.usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
		imageBufferCreateInfos.mipMapLevels = mipLevels;

		if (mipLevels > 1)
			imageBufferCreateInfos.usage |= VK_IMAGE_USAGE_TRANSFER_SRC_BIT;

		mBuffer.Create(device, imageBufferCreateInfos);


		// Copy image to shader.
		const VkTransitionImageInfos undefToDstTransitionInfos
		{
			VK_IMAGE_LAYOUT_UNDEFINED,
			VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
			mipLevels,
			imageBufferCreateInfos.layerNum,
		};

		mBuffer.TransitionImageLayout(device, undefToDstTransitionInfos);

		mBuffer.CopyBufferToImage(device, stagingBuffer, textureExtent, imageBufferCreateInfos.layerNum);
		stagingBuffer.Destroy(device);

		if (mipLevels > 1)
		{
			// Will transition image layout as read only at the end.
			mBuffer.GenerateMipmaps(device, format, _rawTexture.width, _rawTexture.height, mipLevels);
			// TODO: Compute mipmap only once and save mipmap levels in TextureAsset.
		}
		else
		{
			const VkTransitionImageInfos dstToReadTransitionInfos
			{
				VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
				VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
				mipLevels,
				imageBufferCreateInfos.layerNum,
			};

			mBuffer.TransitionImageLayout(device, dstToReadTransitionInfos);
		}



		// CreateTextureSampler. // TODO: Sampler is not link to 1 image: Use 1 for multiple image!
		const VkSamplerCreateInfo samplerCreateInfo
		{
			VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO,						// sType.
			nullptr,													// pNext.
			0,															// flags.
			VK_FILTER_LINEAR,											// magFilter.
			VK_FILTER_LINEAR,											// minFilter.
			VK_SAMPLER_MIPMAP_MODE_LINEAR,								// mipmapMode.
			VK_SAMPLER_ADDRESS_MODE_REPEAT,								// addressModeU.
			VK_SAMPLER_ADDRESS_MODE_REPEAT,								// addressModeV.
			VK_SAMPLER_ADDRESS_MODE_REPEAT,								// addressModeW.
			0.0f,														// mipLodBias.
			VK_TRUE,													// anisotropyEnable.
			16.0f,														// maxAnisotropy.
			VK_FALSE,													// compareEnable.
			VK_COMPARE_OP_ALWAYS,										// compareOp.
			0.0f,														// minLod.
			static_cast<float>(mipLevels),								// maxLod.
			VK_BORDER_COLOR_INT_OPAQUE_BLACK,							// borderColor
			VK_FALSE,													// unnormalizedCoordinates.
		};

		SA_VK_ASSERT(vkCreateSampler(device, &samplerCreateInfo, nullptr, &mSampler),
			CreationFailed, Rendering, L"Failed to create texture sampler!")
	}

	void VkTexture::Destroy(const IRenderInstance& _instance)
	{
		const VkDevice& device = _instance.As<VkRenderInstance>().GetDevice();

		vkDestroySampler(device, mSampler, nullptr);
		mSampler = VK_NULL_HANDLE;

		mBuffer.Destroy(device);
	}

	VkDescriptorImageInfo VkTexture::CreateDescriptorImageInfo() const noexcept
	{
		return VkDescriptorImageInfo
		{
			mSampler,											// sampler.
			mBuffer,											// imageView.
			VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL			// imageLayout.
		};
	}
	VkWriteDescriptorSet VkTexture::CreateWriteDescriptorSet(VkDescriptorSet _descriptorSet, uint32 _binding, uint32 _arrayElem) noexcept
	{
		return VkWriteDescriptorSet
		{
			VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,				// sType.
			nullptr,											// pNext.
			_descriptorSet,										// dstSet.
			_binding,											// dstBinding.
			_arrayElem,											// dstArrayElement.
			1,													// descriptorCount.
			VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,			// descriptorType.
			nullptr,											// pImageInfo.			// Will be set in pipeline.
			nullptr,											// pBufferInfo.
			nullptr												// pTexelBufferView.
		};
	}
}

#endif
