// Copyright 2020 Sapphire development team. All Rights Reserved.

#pragma once

#ifndef SAPPHIRE_SDK_TEXTURE_ASSET_INFOS_GUARD
#define SAPPHIRE_SDK_TEXTURE_ASSET_INFOS_GUARD

#include <SDK/Resources/Assets/IAssetInfos.hpp>
#include <SDK/Resources/Assets/Texture/TextureChannel.hpp>

namespace Sa
{
	struct TextureImportInfos : public IAssetImportInfos
	{
		std::string outFilePath;
		
		TextureChannel channel = TextureChannel::RGBA;

		bool bFlipVertically = true;

		SA_ENGINE_API TextureImportInfos() = default;
		SA_ENGINE_API TextureImportInfos(const std::string& _outFilePath, TextureChannel _channel = TextureChannel::RGBA,
			bool _bFlipVertically = true, bool _bKeepLoaded = true) noexcept;
	};

	struct TextureCreateInfos : public IAssetCreateInfos
	{
		uint32 width = 0;
		uint32 height = 0;

		char* data = nullptr;

		TextureChannel channel = TextureChannel::Unknown;
	};
}

#endif // GUARD