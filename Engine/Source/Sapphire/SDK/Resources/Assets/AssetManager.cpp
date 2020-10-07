// Copyright 2020 Sapphire development team. All Rights Reserved.

#include <SDK/Resources/Assets/AssetManager.hpp>

#include <Core/Algorithms/Move.hpp>

namespace Sa
{
	AssetManager::AssetManager(const IRenderInstance& _instance) noexcept :
		mInstance{ _instance },
		meshMgr{ *this },
		shaderMgr{ *this },
		textureMgr{ *this },
		renderMaterialMgr{ *this }
	{
	}

	AssetManager::~AssetManager()
	{
		Clear();
	}

	const IRenderInstance& AssetManager::GetRenderInstance() const
	{
		return mInstance;
	}

	IRenderPrimitive* AssetManager::Load(const std::string& _filePath, bool _bPreLoaded)
	{
		AssetType assetType = IAsset::GetAssetType(_filePath);

		return Load(_filePath, assetType, _bPreLoaded);
	}

	IRenderPrimitive* AssetManager::Load(const std::string& _filePath, AssetType _assetType, bool _bPreLoaded)
	{
		switch (_assetType)
		{
			case Sa::AssetType::Mesh:
				return meshMgr.Load(_filePath, _bPreLoaded);
			case Sa::AssetType::Shader:
				return shaderMgr.Load(_filePath, _bPreLoaded);
			case Sa::AssetType::Texture:
				return textureMgr.Load(_filePath, _bPreLoaded);
			case Sa::AssetType::RenderMaterial:
				return renderMaterialMgr.Load(_filePath, _bPreLoaded);
			default:
			{
				SA_LOG("Wrong asset type!", Error, SDK_Asset);
				return nullptr;
			}
		}

		return nullptr;
	}

	void AssetManager::UnLoad(IRenderPrimitive* _primitive)
	{
		if (dynamic_cast<IMesh*>(_primitive))
			UnLoad(_primitive, AssetType::Mesh);
		else if (dynamic_cast<IShader*>(_primitive))
			UnLoad(_primitive, AssetType::Shader);
		else if (dynamic_cast<ITexture*>(_primitive))
			UnLoad(_primitive, AssetType::Texture);
		else if (dynamic_cast<IRenderMaterial*>(_primitive))
			UnLoad(_primitive, AssetType::RenderMaterial);
		else
			SA_LOG("Wrong asset type!", Error, SDK_Asset);
	}

	void AssetManager::UnLoad(IRenderPrimitive* _primitive, AssetType _assetType)
	{
		switch (_assetType)
		{
			case Sa::AssetType::Texture:
				textureMgr.Unload(&_primitive->As<ITexture>());
			case Sa::AssetType::Shader:
				shaderMgr.Unload(&_primitive->As<IShader>());
			case Sa::AssetType::RenderMaterial:
				renderMaterialMgr.Unload(&_primitive->As<IRenderMaterial>());
			case Sa::AssetType::Mesh:
				meshMgr.Unload(&_primitive->As<IMesh>());
			default:
				SA_LOG("Wrong asset type!", Warning, SDK_Asset);
		}
	}


	void AssetManager::Clear()
	{
		meshMgr.Clear();
		shaderMgr.Clear();
		textureMgr.Clear();
		renderMaterialMgr.Clear();
	}
}