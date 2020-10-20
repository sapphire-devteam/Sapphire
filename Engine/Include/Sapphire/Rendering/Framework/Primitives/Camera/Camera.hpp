// Copyright 2020 Sapphire development team. All Rights Reserved.

#pragma once

#ifndef SAPPHIRE_RENDERING_CAMERA_GUARD
#define SAPPHIRE_RENDERING_CAMERA_GUARD

#include <Maths/Space/Transform.hpp>

namespace Sa
{
	class Camera
	{
		TransffPRS mTransf;
		
		float mFOV = 90.0f;
		float mNear = 0.35f;
		float mFar = 25.0f;

		bool mViewDirty = true;
		bool mProjDirty = true;

	public:
		// Camera storage buffer ID.
		const uint32 ID = 0;

		Camera(uint32 _ID);
		Camera(Camera&&) = default;
		Camera(const Camera&) = default;

		bool IsViewDirty() const noexcept;
		bool IsProjDirty() const noexcept;

		const Vec3f& GetPosition() const noexcept;
		const Quatf& GetRotation() const noexcept;
		const Vec3f& GetScale() const noexcept;
		// TODO: REMOVE SA_ENGINE_API
		SA_ENGINE_API const TransffPRS& GetTransform() const noexcept;

		float GetFOV() const noexcept;
		float GetNear() const noexcept;
		float GetFar() const noexcept;

		// TODO: REMOVE SA_ENGINE_API
		SA_ENGINE_API void SetPosition(const Vec3f& _position);
		// TODO: REMOVE SA_ENGINE_API
		SA_ENGINE_API void SetRotation(const Quatf& _rotation);
		// TODO: REMOVE SA_ENGINE_API
		SA_ENGINE_API void SetScale(const Vec3f& _scale);
		// TODO: REMOVE SA_ENGINE_API
		SA_ENGINE_API void SetTransform(const TransffPRS& _tr);

		void SetFOV(float _fov);
		void SetNear(float _near);
		void SetFar(float _far);

		Vec3f ComputeViewPosition() const noexcept;

		Mat4f ComputeViewMatrix() noexcept;
		Mat4f ComputeProjMatrix() noexcept;

		Camera& operator=(Camera&&) noexcept;
		Camera& operator=(const Camera& _rhs) noexcept;

		bool operator==(const Camera& _rhs) const noexcept;
		bool operator!=(const Camera& _rhs) const noexcept;
	};
}

#endif // GUARD