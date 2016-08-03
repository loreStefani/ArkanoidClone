#include "Camera.h"
#include <cmath>

using namespace ArkanoidEngine;

void Camera::makePerspective(float aspect, float fovY, float near, float far)
{
	float top = std::tan(fovY / 2.0f)*near;
	float right = aspect *top;
	makePerspective(-right, right, -top, top, near, far);
}

void Camera::makePerspective(float left, float right, float bottom, float top, float near, float far)
{
	float doubleN = near*2.0f;
	float rightMinusLeft = right - left;
	float topMinusBottom = top - bottom;
	float farMinusNear = far - near;

	m_projection = XMFLOAT4X4{
		doubleN / rightMinusLeft, 0.0f, 0.0f, 0.0f,
		0.0f, doubleN / topMinusBottom, 0.0f, 0.0f,
		-(right + left) / rightMinusLeft, -(top + bottom) / topMinusBottom, far / farMinusNear, 1,
		0,	0,	-far*near / farMinusNear, 0
	};
}

void Camera::lookAt(XMFLOAT3 eyePos, XMFLOAT3 target, XMFLOAT3 up)
{
	m_position = eyePos;

	XMVECTOR eyePosXM = XMLoadFloat3(&eyePos);
	XMVECTOR targetXM = XMLoadFloat3(&target);
	XMVECTOR upXM = XMLoadFloat3(&up);
	XMVECTOR lookXM = XMVectorSubtract(targetXM, eyePosXM);
	lookXM = XMVector3Normalize(lookXM);
	XMVECTOR rightXM = XMVector3Cross(upXM, lookXM);
	rightXM = XMVector3Normalize(rightXM);
	upXM = XMVector3Cross(lookXM, rightXM);

	float eDotRight = XMVectorGetX(XMVector3Dot(eyePosXM, rightXM));
	float eDotUp = XMVectorGetX(XMVector3Dot(eyePosXM, upXM));
	float eDotLook = XMVectorGetX(XMVector3Dot(eyePosXM, lookXM));

	XMFLOAT3 rightXM3;
	XMFLOAT3 upXM3;
	XMFLOAT3 lookXM3;
	XMStoreFloat3(&rightXM3, rightXM);
	XMStoreFloat3(&upXM3, upXM);
	XMStoreFloat3(&lookXM3, lookXM);

	m_view = XMFLOAT4X4{
		rightXM3.x, upXM3.x, lookXM3.x, 0.0f,
		rightXM3.y, upXM3.y, lookXM3.y, 0.0f,
		rightXM3.z, upXM3.z, lookXM3.z, 0.0f,
		-eDotRight, -eDotUp, -eDotLook, 1.0f };
}

XMFLOAT4X4 Camera::viewProj() const
{
	XMMATRIX proj = XMLoadFloat4x4(&m_projection);
	XMMATRIX view = XMLoadFloat4x4(&m_view);

	XMFLOAT4X4 viewProj;
	XMStoreFloat4x4(&viewProj, XMMatrixMultiply(view, proj));
	return viewProj;
}