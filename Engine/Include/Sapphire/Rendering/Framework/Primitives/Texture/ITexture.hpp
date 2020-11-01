// Copyright 2020 Sapphire development team. All Rights Reserved.

#pragma once

#ifndef SAPPHIRE_RENDERING_ITEXTURE_GUARD
#define SAPPHIRE_RENDERING_ITEXTURE_GUARD

#include <Rendering/Framework/Primitives/IRenderPrimitive.hpp>
#include <Rendering/Framework/Primitives/Texture/RawTexture.hpp>

namespace Sa
{
	class IRenderInstance;

	class ITexture : public IRenderPrimitive
	{
	protected:
		virtual void Create(const IRenderInstance& _instance, const RawTexture& _rawTexture) = 0;

	public:
		// TODO: CLEAN LATER.
		SA_ENGINE_API static ITexture* brdfLUT;

		static ITexture* CreateInstance(const IRenderInstance& _instance, const RawTexture& _rawTexture);

		virtual void Destroy(const IRenderInstance& _instance) = 0;
	};
}

#endif // GUARD