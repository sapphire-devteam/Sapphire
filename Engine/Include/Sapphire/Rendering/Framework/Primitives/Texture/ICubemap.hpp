// Copyright 2020 Sapphire development team. All Rights Reserved.

#pragma once

#ifndef SAPPHIRE_RENDERING_ICUBEMAP_GUARD
#define SAPPHIRE_RENDERING_ICUBEMAP_GUARD

#include <Rendering/Framework/Primitives/IRenderPrimitive.hpp>
#include <Rendering/Framework/Primitives/Texture/RawCubemap.hpp>

namespace Sa
{
	class IRenderInstance;

	class ICubemap : public IRenderPrimitive
	{
	public:
		virtual void Create(const IRenderInstance& _instance, const RawCubemap& _rawCubemap) = 0;
		virtual void Destroy(const IRenderInstance& _instance) = 0;
	};
}

#endif // GUARD