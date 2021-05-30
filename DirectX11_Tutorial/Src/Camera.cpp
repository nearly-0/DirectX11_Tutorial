#include "Graphics/Camera.h"
using namespace DirectX;

Camera::Camera()
	: m_position(0.f, 0.f, 0.f)
	, m_rotation(0.f, 0.f, 0.f)
	, m_viewMatrix(XMMATRIX())
{
}

Camera::Camera(const Camera& kOther)
{
	m_position = kOther.m_position;
	m_rotation = kOther.m_rotation;
	m_viewMatrix = kOther.m_viewMatrix;
}

Camera::~Camera()
{
}

void Camera::SetPosition(float x, float y, float z)
{
	m_position.x = x;
	m_position.y = y;
	m_position.z = z;
}

void Camera::SetRotation(float x, float y, float z)
{
	m_rotation.x = x;
	m_rotation.y = y;
	m_rotation.z = z;
}

XMFLOAT3 Camera::GetPosition()
{
	return m_position;
}

XMFLOAT3 Camera::GetRotation()
{
	return m_rotation;
}

// Uses the position and rotation of the camera to build and update the view matrix.
void Camera::Render()
{
	// Setup the vector that points upwards
	XMFLOAT3 up;
	up.x = 0.0f;
	up.y = 1.0f;
	up.z = 0.0f;

	// Load it into XMVECTOR structure
	XMVECTOR upVector = XMLoadFloat3(&up);

	// Setup the position of the camera in the world.
	XMFLOAT3 position;
	position.x = m_position.x;
	position.y = m_position.y;
	position.z = m_position.z;

	XMVECTOR positionVector;
	positionVector = XMLoadFloat3(&position);

	// Setup where the camera is looking by default.
	XMFLOAT3 lookAt;
	lookAt.x = 0.0f;
	lookAt.y = 0.0f;
	lookAt.z = 1.f;

	XMVECTOR lookAtVector = XMLoadFloat3(&lookAt);

	//Set the yaw (Y axis), pitch (X axis), and roll (Z axis) rotations in radians.
	constexpr float kRadian = 0.0174532925f;
	float pitch = m_rotation.x * kRadian;
	float yaw = m_rotation.y * kRadian;
	float roll = m_rotation.z * kRadian;

	// Create the rotation matrix from the yawk, pitch, and roll values.
	XMMATRIX rotationMatrix;
	rotationMatrix = XMMatrixRotationRollPitchYaw(pitch, yaw, roll);

	// Transform the lootAt and up vector by the roation matrix so the view is corretly rotated at the origin.
	lookAtVector = XMVector3TransformCoord(lookAtVector, rotationMatrix);
	upVector = XMVector3TransformCoord(upVector, rotationMatrix);

	// Translate the rotated camera position to the location of the viewer.
	lookAtVector = XMVectorAdd(positionVector, lookAtVector);

	// Finally create the view matrix from the three updated vectors.
	m_viewMatrix = XMMatrixLookAtLH(positionVector, lookAtVector, upVector);
}

void Camera::GetViewMatrix(XMMATRIX& viewMatrix)
{
	viewMatrix = m_viewMatrix;
}
