// Copyright 2020 Sapphire development team. All Rights Reserved.

#pragma once

#ifndef SAPPHIRE_RENDERING_GLFW_WINDOW_GUARD
#define SAPPHIRE_RENDERING_GLFW_WINDOW_GUARD

#include <Rendering/Window/IWindow.hpp>

#if SA_WINDOW_API == SA_GLFW

namespace Sa
{
	class GLFWWindow : public IWindow
	{
		static uint32 sInitCount;

		struct GLFWwindow* mHandle = nullptr;

	public:
		SA_ENGINE_API void Create(uint32 _width, uint32 _height, const char* _name = "Main Window") override final;
		SA_ENGINE_API void Destroy() override final;

		SA_ENGINE_API bool ShouldClose() const override final;
	};
}

#endif

#endif // GUARD