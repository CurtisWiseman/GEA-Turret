#ifndef _GAME_OBJECT_H
#define _GAME_OBJECT_H
#include "CommonStates.h"
#include "SimpleMath.h"
#include "GameData.h"

using namespace DirectX;
using namespace DirectX::SimpleMath;

class Camera;
struct ID3D11DeviceContext;

/*
* The basic GameObject from which all others are derived.
*/
class GameObject
{
public:
	GameObject();
	virtual ~GameObject();

	//Getters.
	Vector3 GetPos()										{ return m_pos; }
	float GetPitch()										{ return m_pitch; }
	float GetYaw()											{ return m_yaw; }
	float GetRoll()											{ return m_roll; }
	float GetRadius()										{ return m_radius; }
	Matrix GetFudge()										{ return m_fudge; }
	Vector3 GetScale()										{ return m_scale; }
	Matrix GetWorldMat()									{ return m_worldMat; }
	Vector3 GetCurrentDirection()							{ return currentDirection; }
	Vector3 GetDirectionTowardsTarget(Vector3 _targetPos);

	//Setters.
	void SetPos(Vector3 _pos)								{ m_pos = _pos; }
	void SetPitch(float _pitch)								{ m_pitch = _pitch; }
	void SetYaw(float _yaw)									{ m_yaw = _yaw; }
	void SetRoll(float _roll)								{ m_roll = _roll; }
	void SetPitchYawRoll(float _p, float _y, float _r)		{ m_pitch = _p; m_yaw = _y; m_roll = _r; }
	void SetRadius(float _radius)							{ m_radius = _radius; }
	void SetFudge(Vector3 _pos, float _pitch, float _yaw, float _roll, Vector3 _scale);
	void SetWorldMat(Matrix _mat)							{ m_worldMat = _mat; }

	//Collisions and the ability to turn to face a point.
	bool DoRadiiCollide(GameObject* _other);
	Matrix GameObject::RotateToFace(Vector3 O, Vector3 P, Vector3 U);

	//Basic versions of logic update and render update functions.
	virtual void Tick(GameData* _gData);
	virtual void Draw( ID3D11DeviceContext* _pd3dImmediateContext,  CommonStates* _states, Camera* _cam) = 0;

protected:
	Vector3 m_pos;
	float m_pitch, m_yaw, m_roll;
	Vector3 m_scale;
	Matrix m_fudge;
	Matrix m_worldMat;

	//Collision sphere redius
	float m_radius;
	
	/*
	* A vector to determine how closely the object lines up with each axis.
	* 1 = Directly facing along the axis.
	* 0 = Facing perpendicular to the axis.
	* -1 = Directly facing backwards along the axis.
	*/
	Vector3 currentDirection;
};

#endif