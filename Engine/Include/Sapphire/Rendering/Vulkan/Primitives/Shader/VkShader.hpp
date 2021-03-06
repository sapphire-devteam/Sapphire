// Copyright 2020 Sapphire development team. All Rights Reserved.

#pragma once

#ifndef SAPPHIRE_RENDERING_VK_SHADER_GUARD
#define SAPPHIRE_RENDERING_VK_SHADER_GUARD

#include <Rendering/APIConfig.hpp>

#include <Rendering/Framework/Primitives/Shader/IShader.hpp>

#if SA_RENDERING_API == SA_VULKAN

namespace Sa::Vk
{
	class SA_ENGINE_API Shader : public IShader
	{
		VkShaderModule mHandle = VK_NULL_HANDLE;

	public:
		void Create(const IRenderInstance& _instance, const RawShader& _rawShader) override final;
		void Destroy(const IRenderInstance& _instance) override final;

		operator VkShaderModule() const;
	};
}

#endif

#endif // GUARD
