// Copyright 2020 Sapphire development team. All Rights Reserved.

#pragma once

#ifndef SAPPHIRE_RENDERING_ISHADER_GUARD
#define SAPPHIRE_RENDERING_ISHADER_GUARD

#include <string>

#include <Core/Types/IInterface.hpp>

namespace Sa
{
	class IRenderInstance;

	class IShader : public IInterface
	{
	protected:
		virtual void Create(const IRenderInstance& _instance, const std::vector<char>& _code) = 0;

	public:
		static IShader* CreateInstance(const IRenderInstance& _instance, const std::vector<char>& _code);

		virtual void Destroy(const IRenderInstance& _instance) = 0;
	};
}

#endif // GUARD