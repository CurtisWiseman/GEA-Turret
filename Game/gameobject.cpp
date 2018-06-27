#include "GameObject.h"

GameObject::GameObject()
{
	//Set the Gameobject to the origin with no rotation and unit scaling.
	m_pos = Vector3::Zero;
	m_pitch = 0.0f;
	m_yaw = 0.0f;
	m_roll = 0.0f;
	m_scale = Vector3::One;

	m_worldMat = Matrix::Identity;
	m_fudge = Matrix::Identity;
}

GameObject::~GameObject()
{

}

void GameObject::Tick(GameData* _gData)
{
	//Build up the world matrix depending on the new position of the GameObject,
	//the assumption is that this class will be inherited by the class that ACTUALLY changes this.
	Matrix  scaleMat	= Matrix::CreateScale(m_scale);
	Matrix  rotMat		= Matrix::CreateFromYawPitchRoll(m_yaw, m_pitch, m_roll); 
	Matrix  transMat	= Matrix::CreateTranslation(m_pos);

	m_worldMat = m_fudge * scaleMat * rotMat * transMat;
}

void GameObject::SetFudge(Vector3 _pos, float _yaw, float _roll, float _pitch, Vector3 _scale)
{
	//Scale.
	Matrix m_fudgeS = Matrix({
		_scale.x,	0.0f,		0.0f,		0.0f,


		0.0f,		_scale.y,	0.0f,		0.0f,


		0.0f,		0.0f,		_scale.z,	0.0f,


		_pos.x,		_pos.y,		_pos.z,		1.0f,
	});

	//Rotate yaw.
	float phi = _yaw;
	float sineP = sin(phi);
	float cosineP = cos(phi);

	Matrix m_fudgeP = Matrix({
		cosineP,	0.0f,		-sineP,		0.0f,


		0.0f,		1.0f,		0.0f,		0.0f,


		sineP,		0.0f,		cosineP,	0.0f,


		0.0f,		0.0f,		0.0f,		1.0f,
	});

	//Rotate roll.
	float theta = _roll;
	float sineY = sin(theta);
	float cosineY = cos(theta);

	Matrix m_fudgeY = Matrix({
		cosineY,	sineY,		0.0f,		0.0f,


		-sineY,		cosineY,	0.0f,		0.0f,


		0.0f,		0.0f,		1.0f,		0.0f,


		0.0f,		0.0f,		0.0f,		1.0f,
	});

	//Rotate pitch.
	float sigma = _pitch;
	float sineR = sin(sigma);
	float cosineR = cos(sigma);

	Matrix m_fudgeR = Matrix({
		1.0f,		0.0f,		0.0f,		0.0f,


		0.0f,		cosineR,	sineR,		0.0f,


		0.0f,		-sineR,		cosineR,	0.0f,


		0.0f,		0.0f,		0.0f,		1.0f,
	});

	//Combine into fudge.
	//m_fudge = m_fudgeS * m_fudgeP * m_fudgeR * m_fudgeY;
	m_fudge = m_fudgeS * m_fudgeP * m_fudgeR * m_fudgeY;
}

Vector3 GameObject::GetDirectionTowardsTarget(Vector3 _targetPos)
{
	const int calculationType = 1;
	
	if (calculationType == 1)
	{
		//It's quite possible that this is far too complicated for its own good but it works so it's staying in.
		Vector3 A = Vector3(m_pos.x, m_pos.y, m_pos.z);
		Vector3 B = Vector3(_targetPos.x, m_pos.y, m_pos.z);
		Vector3 C = Vector3(_targetPos.x, m_pos.y, _targetPos.z);
		Vector3 D = Vector3(m_pos.x, m_pos.y, _targetPos.z);
		Vector3 E = Vector3(m_pos.x, _targetPos.y, _targetPos.z);
		Vector3 F = Vector3(_targetPos.x, _targetPos.y, _targetPos.z);
		Vector3 G = Vector3(_targetPos.x, _targetPos.y, m_pos.z);
		Vector3 H = Vector3(m_pos.x, _targetPos.y, m_pos.z);

		float AB = B.x - A.x;
		float BC = C.z - B.z;
		float AC = sqrt((AB*AB) + (BC*BC));
		float BG = G.y - B.y;
		float AG = sqrt((BG*BG) + (AB*AB));
		float CF = F.y - C.y;
		float AF = sqrt((AC*AC) + (CF*CF));
		float AD = D.z - A.z;
		float DE = E.y - D.y;
		float AE = sqrt((AD*AD) + (DE*DE));
		float DC = C.z - D.z;

		float angleYaw = asin(BC / AC);
		float anglePitch = asin(CF / AF);
		float angleRoll = asin(AD / AE);

		Vector3 direction = Vector3(angleYaw, anglePitch, angleRoll);
		direction.Normalize();

		return direction;
	}
	
	if (calculationType == 2)
	{
		//2D-centric design
		float ax = m_pos.x;
		float az = m_pos.z;
		float bx = _targetPos.x;
		float bz = _targetPos.z;

		float dot = (ax*bx) + (az*bz);
		float det = (ax*bz) + (bx*az);

		float phi = atan2(det, dot);

		float dirx = sin(phi);
		float dirz = cos(phi);

		return Vector3(dirx, phi, dirz);
	}
}

bool GameObject::DoRadiiCollide(GameObject* _other)
{
	//Check to see if the collision spheres intersect
	float length = Vector3::Distance(_other->GetPos(), m_pos);
	length -= _other->GetRadius();
	length -= m_radius;
	if (length > 0)
	{
		return false;
	}
	else
	{
		return true;
	}
}



Matrix GameObject::RotateToFace(Vector3 O, Vector3 P, Vector3 U)
{
	//Attempts to rotate the object to face a certain direction.
	//O is the object's position.
	//P is the position of the object to face.
	//U is the "up" vector.
	//Important: this does not work when O is straight below or straight above P.
	Vector3 D = (O - P);
	Vector3 Right;
	Right.Cross(U, D);
	Right.Normalize();
	Vector3 Backwards;
	Backwards.Cross(Right, U);
	Backwards.Normalize();
	Vector3 Up;
	Up.Cross(Backwards, Right);
	Matrix rot = Matrix(Right.x, Right.y, Right.z, 0, Up.x, Up.y, Up.z, 0, Backwards.x, Backwards.y, Backwards.z, 0, 0, 0, 0, 1);
	return rot;
}
