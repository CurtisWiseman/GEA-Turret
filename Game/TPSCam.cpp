#include "TPSCam.h"


TPSCam::TPSCam(float _fov, float _aspect, float _nearClip, float _farClip, GameObject* _target, Vector3 _up, Vector3 _dpos) : Camera(_fov, _aspect, _nearClip, _farClip, _target->GetPos(), _up)
{
	m_targetObject = _target;
	m_dpos = _dpos;
}


TPSCam::~TPSCam()
{

}


void TPSCam::Tick(GameData* _gData)
{
	//Tracking the player.
	m_target = m_targetObject->GetPos();

	Vector3 tpr = Vector3(0, 50, 100);
	Matrix rMatrix = Matrix::CreateRotationY(m_targetObject->GetYaw());
	Vector3 transformedReference = Vector3::Transform(tpr, rMatrix);
	m_pos = transformedReference + m_target;

	//Setting up matrices.
	Camera::Tick(_gData);
}