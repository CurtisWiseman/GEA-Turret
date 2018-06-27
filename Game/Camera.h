//base camera class
#ifndef _CAMERA_H_
#define _CAMERA_H_
#include "GameObject.h"

/*
* Basic camera with no in-built movement potential.
*/
class Camera : public GameObject
{
public:
	Camera(float _fieldOfView, float _aspectRatio, float _nearPlaneDistance, float _farPlaneDistance, Vector3 _target, Vector3 _up);
	~Camera();

	//Getters for matrices.
	Matrix const GetProj() { return m_projMat; };
	Matrix const GetView() { return m_viewMat; };

	//Specialised versions of basic functions.
	virtual void Tick(GameData* _gData) override;
	virtual void Draw( ID3D11DeviceContext* _pd3dImmediateContext,  CommonStates* _states, Camera* _cam) override;

protected:

	//DirectX matrices and other basic parameters.
	Matrix m_projMat;
	Matrix m_viewMat;
	float m_fieldOfView;
	float m_aspectRatio;
	float m_nearPlaneDistance;
	float m_farPlaneDistance;

	//Parameters for setting the initial target.
	Vector3 m_target;
	Vector3 m_up;

};
#endif