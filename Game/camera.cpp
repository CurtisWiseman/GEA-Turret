#include "Camera.h"

Camera::Camera(float _fieldOfView, float _aspectRatio, float _nearPlaneDistance, float _farPlaneDistance, Vector3 _target, Vector3 _up)
{
	m_fieldOfView = _fieldOfView;
	m_aspectRatio = _aspectRatio;
	m_nearPlaneDistance = _nearPlaneDistance;
	m_farPlaneDistance = _farPlaneDistance;

	m_target = _target;
	m_up = _up;
}


Camera::~Camera()
{
}


void Camera::Tick(GameData* _gData)
{
	m_projMat = Matrix::CreatePerspectiveFieldOfView( m_fieldOfView, m_aspectRatio, m_nearPlaneDistance, m_farPlaneDistance );
	m_viewMat = Matrix::CreateLookAt( m_pos, m_target, m_up );
	GameObject::Tick(_gData);
}


void Camera::Draw( ID3D11DeviceContext* _pd3dImmediateContext,  CommonStates* _states, Camera* _cam)
{
	//standard camera doesn't draw ANYTHING
}
