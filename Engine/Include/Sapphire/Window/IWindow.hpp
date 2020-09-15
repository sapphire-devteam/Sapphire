// Copyright 2020 Sapphire development team. All Rights Reserved.

#pragma once

#ifndef SAPPHIRE_WINDOW_IWINDOW_GUARD
#define SAPPHIRE_WINDOW_IWINDOW_GUARD

#include <Core/Types/Int.hpp>

#include <Window/Config.hpp>

namespace Sa
{
	class RenderSurface;
	class IRenderInstance;

	class IWindow
	{
	public:
		virtual void Create(uint32 _width, uint32 _height, const char* _name = "Window") = 0;
		virtual void Destroy() = 0;

		virtual bool ShouldClose() const = 0;
	};
}

#endif // GUARD