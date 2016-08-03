#pragma once
#include "MathCommon.h"

namespace ArkanoidEngine
{
	class Camera
	{
	public:
		//ctors
		explicit Camera() = default;

		//dtor
		~Camera() = default;

		//copy
		Camera(const Camera&) = default;
		Camera& operator=(const Camera&) = default;

		//move
		Camera(Camera&&) = default;
		Camera& operator=(Camera&&) = default;

		XMFLOAT4X4& projection();
		const XMFLOAT4X4& projection() const;
		
		XMFLOAT4X4& view();
		const XMFLOAT4X4& view() const;

		XMFLOAT4X4 viewProj() const;

		XMFLOAT3 position()const;

		//define view transform
		void lookAt(XMFLOAT3 eyePos,
					XMFLOAT3 target = XMFLOAT3{ 0.0f, 0.0f, 0.0f },
					XMFLOAT3 up = XMFLOAT3{ 0.0f, 1.0f, 0.0 });

		//define perspective transform
		void makePerspective(float aspect, float fovY, float near, float far);
		void makePerspective(float left, float right, float bottom, float top, float near, float far);

	private:
		XMFLOAT3 m_position{};
		XMFLOAT4X4 m_view{};
		XMFLOAT4X4 m_projection{};
	};
	
	inline XMFLOAT4X4& Camera::projection()
	{
		return m_projection;
	}

	inline XMFLOAT4X4& Camera::view()
	{
		return m_view;
	}
	
	inline const XMFLOAT4X4& Camera::projection() const
	{
		return m_projection;
	}

	inline const XMFLOAT4X4& Camera::view() const
	{
		return m_view;
	}

	inline XMFLOAT3 Camera::position()const
	{
		return m_position;
	}
}