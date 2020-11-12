// Copyright 2020 Sapphire development team. All Rights Reserved.

#pragma once

#ifndef SAPPHIRE_RENDERING_RAW_TEXTURE_GUARD
#define SAPPHIRE_RENDERING_RAW_TEXTURE_GUARD

#include <Maths/Space/Vector2.hpp>

#include <Rendering/Framework/Misc/RenderFormat.hpp>

namespace Sa
{
	struct RawTexture
	{
		Vec2ui extent;

		std::vector<char> data;

		uint32 mipLevels = 1u;

		RenderFormat format = RenderFormat::RGBA_32;
	};
}

#endif // GUARD
