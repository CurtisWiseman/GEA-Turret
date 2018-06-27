#include "Enemy.h"
#include "Gun.h"

Enemy::Enemy(string _fileName, ID3D11Device* _pd3dDevice, IEffectFactory* _EF) : CMOGO(_fileName, _pd3dDevice, _EF)
{
	//Setup and loading for enemy.
	m_alive = false;
	SetRadius(20);

	SetFudge(Vector3(0, 0, 0), 0.0f*XM_PI, 0.0f, 0.0f, Vector3(1, 1, 1));
	setRandomTarget();

	//Setup and loading for child object: gun.
	gun = new Gun("turret.cmo", _pd3dDevice, _EF, this, "bullet.cmo", 8);
	gun->SetFudge(Vector3(0, 0, 0), 0.0f*XM_PI, 0.0f, 0.0f, Vector3(1, 1, 1));
	gun->setPlayerControlled(false);
}


Enemy::~Enemy()
{
	//Cleaning up the memory for the gun.
	if (gun)
	{
		delete gun;
	}
}


void Enemy::Spawn(Vector3 _pos)
{
	//Create the Enemy in the world.
	m_alive = true;
	m_pos = _pos;
}


void Enemy::Destroy()
{
	//Remove the enemy from the world.
	m_alive = false;
}


void Enemy::Tick(GameData* _GD)
{
	//If alive, handle the targets and face towards target.
	if (m_alive)
	{
		targetSettingAndMovement(_GD);

		fixRotation(_GD);

		//Make the gun tick.
		gun->Gun::Tick(_GD);
	}
}


void Enemy::Draw(ID3D11DeviceContext* _pd3dImmediateContext, CommonStates* _states, Camera* _cam)
{
	//Draw both self and gun.
	if (m_alive)
	{
		CMOGO::Draw(_pd3dImmediateContext, _states, _cam);
		gun->Gun::Draw(_pd3dImmediateContext, _states, _cam);
	}
}


void Enemy::setRandomTarget()
{
	//Find a new random target within the bounds we have set.
	const int terrainBounds = 200;
	int intX = rand() % (terrainBounds*2) - terrainBounds;
	float floatX = intX;

	float floatY = 0.0f;

	int intZ = rand() % (terrainBounds*2) - terrainBounds;
	float floatZ = intZ;

	m_target = Vector3(floatX, floatY, floatZ);
}


bool Enemy::isCloseToTarget(char dir)
{
	//Are we within one tick's distance of the target?
	bool close = false;
	switch (dir)
	{
	case 'x':
		if ((m_pos.x < (m_target.x + moveSpeed*0.2)) &&
			(m_pos.x >(m_target.x - moveSpeed*0.2)))
		{
			close = true;
		}
		break;
	case 'y':
		if ((m_pos.y < (m_target.y + moveSpeed*0.2)) &&
			(m_pos.y >(m_target.y - moveSpeed*0.2)))
		{
			close = true;
		}
		break;
	case 'z':
		if ((m_pos.z < (m_target.z + moveSpeed*0.2)) &&
			(m_pos.z >(m_target.z - moveSpeed*0.2)))
		{
			close = true;
		}
		break;
	default:
		break;
	}
	return close;
}


void Enemy::targetSettingAndMovement(GameData* _GD)
{
	//Check to see if we are close, and act accordingly.

	if (_GD->MovementType == 1)
	{
		//If we ARE close, set a new target
		if (isCloseToTarget('x') && isCloseToTarget('y') && isCloseToTarget('z'))
		{
			setRandomTarget();
		}
		//If we aren't close, move towards the target.
		else
		{
			if (!isCloseToTarget('x'))
			{
				if (m_pos.x > m_target.x)
				{
					m_pos.x -= moveSpeed*_GD->m_dt;
				}
				else
				{
					m_pos.x += moveSpeed*_GD->m_dt;
				}
			}
			if (!isCloseToTarget('y'))
			{
				if (m_pos.y > m_target.y)
				{
					m_pos.y -= moveSpeed*_GD->m_dt;
				}
				else
				{
					m_pos.y += moveSpeed*_GD->m_dt;
				}
			}
			if (!isCloseToTarget('z'))
			{
				if (m_pos.z > m_target.z)
				{
					m_pos.z -= moveSpeed*_GD->m_dt;
				}
				else
				{
					m_pos.z += moveSpeed*_GD->m_dt;
				}
			}
		}
	}
	
	if (_GD->MovementType == 2)
	{
		//If we ARE close, set a new target
		if (isCloseToTarget('x') && isCloseToTarget('z'))
		{
			setRandomTarget();
		}
		else
		{
			//If we aren't close, move towards the target.
			if (!isCloseToTarget('x'))
			{
				m_pos.x += moveSpeed*_GD->m_dt*GetDirectionTowardsTarget(m_target).x;
			}
			if (!isCloseToTarget('z'))
			{
				m_pos.z += moveSpeed*_GD->m_dt*GetDirectionTowardsTarget(m_target).z;
			}
		}
	}

	//Put a hard limit on how far the enemy can move, just in case something else fails.
	const int terrainBounds = 200;

	if (m_pos.x > terrainBounds)
	{
		m_pos.x = terrainBounds;
	}
	else if (m_pos.x < -terrainBounds)
	{
		m_pos.x = -terrainBounds;
	}

	if (m_pos.z > terrainBounds)
	{
		m_pos.z = terrainBounds;
	}
	else if (m_pos.z < -terrainBounds)
	{
		m_pos.z = -terrainBounds;
	}
}


void Enemy::fixRotation(GameData* _GD)
{
	//Get some data to work with.
	Matrix  scaleMat = Matrix::CreateScale(m_scale);
	Matrix  rotMat = Matrix::CreateFromYawPitchRoll(m_yaw, m_pitch, m_roll);
	Matrix  transMat = Matrix::CreateTranslation(m_pos);

	Matrix lookAt = Matrix::CreateLookAt(m_pos, GetDirectionTowardsTarget(m_target), Vector3::Up);


	Vector3 enemDir = Vector3::Transform(Vector3::Forward, m_fudge.Invert() *
										 Matrix::CreateFromYawPitchRoll(
										 GetDirectionTowardsTarget(m_target).y + m_yaw,
										 GetDirectionTowardsTarget(m_target).x + m_pitch,
										 GetDirectionTowardsTarget(m_target).z + m_roll));
	Vector3 enemUp = Vector3::Transform(Vector3::Up, m_fudge.Invert() * m_worldMat);
	enemDir.Normalize();
	enemUp.Normalize();

	//Depending on what version of the rotation we want to use, try to rotate to face.

	if (_GD->RotationType == 1)
	{
		m_worldMat = m_fudge * scaleMat * Matrix::CreateWorld(m_pos, GetDirectionTowardsTarget(m_target), Vector3::Up);
		m_yaw = GetDirectionTowardsTarget(m_target).x;
		
	}

	if (_GD->RotationType == 2)
	{
		m_worldMat = m_fudge * scaleMat * Matrix::CreateWorld(m_pos, GetDirectionTowardsTarget(m_target), Vector3::Up);
		m_yaw = asin(GetDirectionTowardsTarget(m_target).x);
	}

	if (_GD->RotationType == -1)
	{
		//Some other methods that may or may not work. Try at your own risk.
		m_worldMat = m_fudge * m_worldMat;
		m_worldMat = m_fudge * scaleMat * RotateToFace(m_pos, m_target, Vector3::Up);
		m_worldMat = m_fudge * scaleMat * RotateToFace(m_pos, enemDir, Vector3::Up);
		
		Vector3 vecA = m_pos;
		Vector3 vecB;
		vecB = XMVectorSubtract(m_target, m_pos);

		vecA.Normalize();
		vecB.Normalize();
		Vector3 crossProd = vecA;
		crossProd.Cross(vecB);
		Vector3 dotProd = vecA;

		float fDotProd = dotProd.Dot(vecB);
		float theta = acos(fDotProd);
		m_yaw = theta;
	}
}