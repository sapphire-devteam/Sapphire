// Copyright 2020 Sapphire development team. All Rights Reserved.

#pragma once

#ifndef SAPPHIRE_RENDERING_IRENDER_MATERIAL_GUARD
#define SAPPHIRE_RENDERING_IRENDER_MATERIAL_GUARD

#include <vector>

#include <Core/Types/IInterface.hpp>

namespace Sa
{
	class IRenderFrame;
	class IRenderPipeline;
	class IRenderInstance;

	class PipelineCreateInfos;

	class IRenderMaterial : public IInterface
	{
	protected:
		IRenderPipeline* mPipeline = nullptr;

	public:
		virtual void CreatePipeline(const IRenderInstance& _instance, const PipelineCreateInfos& _pipelineInfos) = 0;
		SA_ENGINE_API virtual void DestroyPipeline(const IRenderInstance& _instance);

		SA_ENGINE_API virtual void Bind(const IRenderFrame& _frame);
	};
}

#endif // GUARD
