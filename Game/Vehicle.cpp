#include "Vehicle.h"
#include "Gun.h"
#include <dinput.h>
#include <math.h>

Vehicle::Vehicle(string _fileName, ID3D11Device* _pd3dDevice, IEffectFactory* _EF) : CMOGO(_fileName,_pd3dDevice,_EF)
{
	//Any special set up for Vehicle goes here.
	currentDirection = Vector3(0, 0, 1);
	SetFudge(Vector3(0, 0, 0), 0.0f, 0.0f, 0.0f*XM_PI, Vector3(1, 1, 1));
	SetRadius(20);

	//Set up the gun as well.
	gun = new Gun("turret.cmo", _pd3dDevice, _EF, this, "bullet.cmo", 8);
	
	Spawn();
}


Vehicle::~Vehicle()
{
	//Tidy up anything I've created.
	if (gun)
	{
		delete gun;
	}
}


void Vehicle::Spawn()
{
	m_alive = true;
}


void Vehicle::Tick(GameData* _gData)
{
	if (m_alive == true)
	{
		//Keyboard control of the vehicle.
		if (_gData->currentState == _gData->GS_PLAY_TPS_CAM)
		{
			//Go forward.
			if (_gData->m_keyboardState[DIK_W] & 0x80 || _gData->m_keyboardState[DIK_UP] & 0x80)
			{
				m_pos -= movespeed*currentDirection*_gData->m_dt;
			}
			//Go backward.
			if (_gData->m_keyboardState[DIK_S] & 0x80 || _gData->m_keyboardState[DIK_DOWN] & 0x80)
			{
				m_pos += movespeed*currentDirection*_gData->m_dt;
			}
			//Turn left.
			if (_gData->m_keyboardState[DIK_A] & 0x80 || _gData->m_keyboardState[DIK_LEFT] & 0x80)
			{
				m_yaw += turnspeed*_gData->m_dt;
				UpdateCurrentDirection('y', m_yaw);
			}
			//Turn right.
			if (_gData->m_keyboardState[DIK_D] & 0x80 || _gData->m_keyboardState[DIK_RIGHT] & 0x80)
			{
				m_yaw -= turnspeed*_gData->m_dt;
				UpdateCurrentDirection('y', m_yaw);
			}
		}
		else
		{
			//Move based on mouse movement.
			m_pos.x += movespeed*_gData->m_mouseState->lX*_gData->m_dt;
			m_pos.z += movespeed*_gData->m_mouseState->lY*_gData->m_dt;
		}

		//Limit the boundaries of movement.
		const int terrainBounds = 200;
		if (m_pos.z > terrainBounds)
			m_pos.z = terrainBounds;
		if (m_pos.z < -terrainBounds)
			m_pos.z = -terrainBounds;
		if (m_pos.x > terrainBounds)
			m_pos.x = terrainBounds;
		if (m_pos.x < -terrainBounds)
			m_pos.x = -terrainBounds;

		//Apply my base behaviour.
		CMOGO::Tick(_gData);

		//Apply fudge transform.
		m_worldMat = m_fudge * m_worldMat;

		//Make the gun tick.
		gun->Gun::Tick(_gData);
	}
	else
	{
		//Respawn after a small, random amount of time.
		float r = (float)rand() / (float)RAND_MAX;
		if (r < 1.0f *_gData->m_dt)
		{
			Spawn();
		}
	}
}


void Vehicle::Draw(ID3D11DeviceContext* _pd3dImmediateContext, CommonStates* _states, Camera* _cam)
{
	if (m_alive == true)
	{
		//I'm still a CMOGO so that's what draws me.
		CMOGO::Draw(_pd3dImmediateContext, _states, _cam);

		//The gun needs to be drawn too.
		gun->Gun::Draw(_pd3dImmediateContext, _states, _cam);
	}
}


void Vehicle::UpdateCurrentDirection(char axis, float rotation)
{
	/*
	* Updates the distance-based directional vector.
	* This is different to the angle-based rotation matrix,
	* because it works more like a co-ordinate from the origin.
	*/
	switch (axis)
	{
	case 'x':
		currentDirection.z = -sinf(rotation);
		currentDirection.y = cosf(rotation);
		break;
	case 'y':
		currentDirection.z = cosf(rotation);
		currentDirection.x = sinf(rotation);
		break;
	case 'z':
		currentDirection.y = sinf(rotation);
		currentDirection.x = cosf(rotation);
		break;
	}
}


void Vehicle::Destroy()
{
	m_alive = false;
}