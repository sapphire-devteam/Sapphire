// Copyright 2020 Sapphire development team. All Rights Reserved.

#include <SDK/Assets/RenderMaterial/RenderMaterialAsset.hpp>

#include <tiny_obj_loader.h>

#include <Core/Algorithms/Move.hpp>

#include <Core/Algorithms/SizeOf.hpp>

#include <Rendering/Framework/System/IRenderSurface.hpp>

#include <SDK/Assets/AssetManager.hpp>


// TODO: REMOVE LATER.
#include <Rendering/Framework/System/RenderPass/IRenderPass.hpp>
#include <Rendering/Framework/Primitives/Camera/ICamera.hpp>


namespace Sa
{
	RenderMaterialAsset::RenderMaterialAsset(IResourceMgrBase& _manager) noexcept : IAsset(_manager, AssetType::RenderMaterial)
	{
	}

	RenderMaterialAsset::RenderMaterialAsset(RenderMaterialAsset&& _other) noexcept :
		IAsset(Move(_other)),
		mRawData{ Move(_other.mRawData) },
		vertexShaderPath{ Move(_other.vertexShaderPath) },
		fragmentShaderPath{ Move(_other.fragmentShaderPath) },
		texturePaths{ Move(_other.texturePaths) }
	{
		_other.UnLoad(false);
	}

	RenderMaterialAsset::RenderMaterialAsset(IResourceMgrBase& _manager, RawT&& _raw) noexcept :
		IAsset(_manager, AssetType::RenderMaterial),
		mRawData{ Move(_raw) }
	{
	}

	RenderMaterialAsset::~RenderMaterialAsset()
	{
		UnLoad_Internal(true);
	}


	IRenderMaterial* RenderMaterialAsset::GetResource() const
	{
		IRenderMaterial* result = nullptr;
		auto& manager = mManager.As<ResourceMgr<RenderMaterialAsset>>();

		result = manager.Query(mFilePath);

		if (result)
			return result;

		return manager.Load(*this);
	}

	bool RenderMaterialAsset::IsValid() const noexcept
	{
		return !vertexShaderPath.empty() && !fragmentShaderPath.empty();
	}

	std::vector<AssetPathDependency> RenderMaterialAsset::GetPathDependencies() const noexcept
	{
		std::vector<AssetPathDependency> result = IAsset::GetPathDependencies();

		result.reserve(texturePaths.size() + 2);

		result.emplace_back(AssetPathDependency{ vertexShaderPath, AssetType::Shader });
		result.emplace_back(AssetPathDependency{ fragmentShaderPath, AssetType::Shader });

		for (auto it = texturePaths.begin(); it != texturePaths.end(); ++it)
			result.emplace_back(AssetPathDependency{ *it, AssetType::Texture });

		return result;
	}

	bool RenderMaterialAsset::Load_Internal(std::istringstream&& _hStream, std::fstream& _fStream)
	{
		std::string line;
		
		// Shaders.
		{
			if (!std::getline(_fStream, line))
			{
				SA_LOG("Can't parse shader paths", Warning, SDK_Asset);
				return false;
			}

			std::istringstream stream(line);

			if (!(stream >> vertexShaderPath >> fragmentShaderPath))
			{
				SA_LOG("Can't parse shader paths!", Warning, SDK_Asset);
				return false;
			}
		}


		// Textures.
		{
			if (!std::getline(_fStream, line))
			{
				SA_LOG("Can't parse textures paths", Warning, SDK_Asset);
				return false;
			}

			uint32 size = 0u;
			std::istringstream stream(line);

			if (!(stream >> size))
			{
				SA_LOG("Can't parse texture path size!", Warning, SDK_Asset);
				return false;
			}

			texturePaths.resize(size);

			for (auto it = texturePaths.begin(); it != texturePaths.end(); ++it)
			{
				if (!(stream >> *it))
				{
					SA_LOG("Can't parse texture path!", Warning, SDK_Asset);
					return false;
				}
			}
		}

		// Data
		{
			_fStream.read(reinterpret_cast<char*>(&mRawData.bDynamicViewport), sizeof(bool));


			uint32 shaderSize = 0u;
			_fStream.read(reinterpret_cast<char*>(&shaderSize), sizeof(uint32));

			mRawData.shaders.resize(shaderSize);

			for (auto it = mRawData.shaders.begin(); it != mRawData.shaders.end(); ++it)
			{
				_fStream.read(const_cast<char*>(reinterpret_cast<const char*>(&it->type)), sizeof(uint8));

				uint32 size = 0u;
				_fStream.read(reinterpret_cast<char*>(&size), sizeof(uint32));

				it->specConstants.resize(size);

				_fStream.read(reinterpret_cast<char*>(it->specConstants.data()), BitSizeOf(it->specConstants));
			}


			uint32 size = offsetof(RawMaterial, pushConstInfos) - offsetof(RawMaterial, matConstants);
			_fStream.read(reinterpret_cast<char*>(&mRawData.matConstants), size);

			uint32 pushConstSize = 0u;
			_fStream.read(reinterpret_cast<char*>(&pushConstSize), sizeof(uint32));

			if (pushConstSize > 0)
			{
				mRawData.pushConstInfos.resize(pushConstSize);

				_fStream.read(reinterpret_cast<char*>(mRawData.pushConstInfos.data()), BitSizeOf(mRawData.pushConstInfos));
			}
		}

		return true;
	}

	void RenderMaterialAsset::UnLoad_Internal(bool _bFreeResources)
	{
		vertexShaderPath.clear();
		fragmentShaderPath.clear();

		texturePaths.clear();
	}


	void RenderMaterialAsset::Save_Internal(std::fstream& _fStream) const
	{
		// Header.
		_fStream << '\n';

		// Shaders.
		_fStream << vertexShaderPath << ' ' << fragmentShaderPath << '\n';


		// Textures.
		_fStream << texturePaths.size() << ' ';

		for (auto it = texturePaths.begin(); it != texturePaths.end(); ++it)
			_fStream << *it << ' ';

		_fStream << '\n';


		// Data.
		_fStream.write(reinterpret_cast<const char*>(&mRawData.bDynamicViewport), sizeof(bool));


		uint32 shaderSize = SizeOf(mRawData.shaders);
		_fStream.write(reinterpret_cast<const char*>(&shaderSize), sizeof(uint32));


		for (auto it = mRawData.shaders.begin(); it != mRawData.shaders.end(); ++it)
		{
			_fStream.write(reinterpret_cast<const char*>(&it->type), sizeof(uint8));

			uint32 size = SizeOf(it->specConstants);
			_fStream.write(reinterpret_cast<const char*>(&size), sizeof(uint32));

			_fStream.write(reinterpret_cast<const char*>(it->specConstants.data()), BitSizeOf(it->specConstants));
		}


		uint32 size = offsetof(RawMaterial, pushConstInfos) - offsetof(RawMaterial, matConstants);
		_fStream.write(reinterpret_cast<const char*>(&mRawData.matConstants), size);

		uint32 pushConstSize = SizeOf(mRawData.pushConstInfos);
		_fStream.write(reinterpret_cast<const char*>(&pushConstSize), sizeof(uint32));

		if(pushConstSize > 0)
			_fStream.write(reinterpret_cast<const char*>(mRawData.pushConstInfos.data()), BitSizeOf(mRawData.pushConstInfos));
	}

	IRenderMaterial* RenderMaterialAsset::Create(const IRenderInstance& _instance) const
	{
		IRenderMaterial* result = IRenderMaterial::CreateInstance();

		AssetManager& assetMgr = mManager.As<ResourceMgr<RenderMaterialAsset>>().GetAssetMgr();

		// TODO: Remove later.
		mRawData.renderPass = IRenderPass::mainRenderPass;
		mRawData.cameras = { ICamera::mainCamera };
		mRawData.bDynamicViewport = false;


		// Shaders.
		mRawData.shaders[0].shader = assetMgr.shaderMgr.Load(vertexShaderPath);
		SA_ASSERT(mRawData.shaders[0].shader, Nullptr, SDK_Asset, L"Shader asset nulltpr! Path invalid");

		mRawData.shaders[1].shader = assetMgr.shaderMgr.Load(fragmentShaderPath);
		SA_ASSERT(mRawData.shaders[1].shader, Nullptr, SDK_Asset, L"Shader asset nulltpr! Path invalid");


		// Textures
		for (uint32 i = 0u; i < SizeOf(texturePaths); ++i)
		{
			if (texturePaths[i].empty())
				continue;

			mRawData.textures.data[i] = assetMgr.textureMgr.Load(texturePaths[i]);
			SA_ASSERT(mRawData.textures.data[i], Nullptr, SDK_Asset, L"Texture asset nulltpr! Path invalid");
		}

		result->Create(_instance, mRawData);

		return result;
	}


	RenderMaterialAsset& RenderMaterialAsset::operator=(RenderMaterialAsset&& _rhs)
	{
		mRawData = Move(_rhs.mRawData);

		_rhs.UnLoad(false);

		return *this;
	}
}