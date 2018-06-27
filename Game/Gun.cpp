#include "Gun.h"
#include "Bullet.h"
#include <list>

Gun::Gun(string _fileName, ID3D11Device* _pd3dDevice, IEffectFactory* _EF, GameObject* _parent, string _bulletFileName, int _numBullets) : CMOGO(_fileName, _pd3dDevice, _EF)
{
	//Set parent.
	m_parent = _parent;

	//Other setup.
	SetFudge(Vector3(0, 0, 0), 0.0f, 0.0f, 0.0f, Vector3(1, 1, 1));
	SetRadius(10);
	m_pos.y = 30.0f;
	
	//Set up the bullets.
	for (int i = 0; i < _numBullets; i++)
	{
		m_Bullets.push_back(new Bullet(_bulletFileName, _pd3dDevice, _EF));
	}

	//Angle down if it isn't controlled by the player.
	if (!playerControlled)
	{
		m_pitch -= 0.25 * XM_PI;
	}
}


Gun::~Gun()
{
	//Delete any bullets.
	for (auto&& bull : m_Bullets)
	{
		delete bull;
	}
	m_Bullets.clear();
}


void Gun::Tick(GameData* _GD)
{
	if (playerControlled)
	{
		//Keyboard control of the Vehicle, followed by mouse control
		if (_GD->currentState == _GD->GS_PLAY_MAIN_CAM)
		{
			//Angle down
			if (_GD->m_keyboardState[DIK_W] & 0x80 || _GD->m_keyboardState[DIK_UP] & 0x80)
			{
				m_pitch -= turnspeed*_GD->m_dt;
			}
			//Angle up
			if (_GD->m_keyboardState[DIK_S] & 0x80 || _GD->m_keyboardState[DIK_DOWN] & 0x80)
			{
				m_pitch += turnspeed*_GD->m_dt;
			}
			//Angle right
			if (_GD->m_keyboardState[DIK_D] & 0x80 || _GD->m_keyboardState[DIK_RIGHT] & 0x80)
			{
				m_yaw += turnspeed*_GD->m_dt;
			}
			//Angle left
			if (_GD->m_keyboardState[DIK_A] & 0x80 || _GD->m_keyboardState[DIK_LEFT] & 0x80)
			{
				m_yaw = turnspeed*_GD->m_dt;
			}
		}
		else if (_GD->currentState == _GD->GS_PLAY_TPS_CAM)
		{
			//Yaw
			m_yaw -= turnspeed*_GD->m_mouseState->lX*_GD->m_dt;

			//Pitch
			m_pitch -= turnspeed*_GD->m_mouseState->lY*_GD->m_dt;
		}
	}
	else
	{
		//If not player controlled, keep that low angle in spite of the parent object's movement.
		m_pitch = -0.125 * XM_PI;
	}

	//Limit the amount of rotation.
	if (m_pitch > 0.2f*XM_PI)
		m_pitch = 0.2f*XM_PI;
	if (m_pitch < -0.2f*XM_PI)
		m_pitch = -0.2f*XM_PI;
	UpdateCurrentDirection('x', m_pitch);

	if (m_yaw > 0.3f*XM_PI)
		m_yaw = 0.3f*XM_PI;
	if (m_yaw < -0.3f*XM_PI)
		m_yaw = -0.3f*XM_PI;
	UpdateCurrentDirection('y', m_yaw);

	CMOGO::Tick(_GD);

	//Multiply our matrix by the parent's.
	if (m_parent)
	{
		m_worldMat = m_worldMat * m_parent->GetFudge().Invert() * m_parent->GetWorldMat();
	}

	if (playerControlled)
	{
		//Fire on button press.
		if ((_GD->currentState == _GD->GS_PLAY_MAIN_CAM && _GD->m_keyboardState[DIK_Z] & 0x80) && !(_GD->m_prevKeyboardState[DIK_Z] & 0x80) 
			|| _GD->currentState == _GD->GS_PLAY_TPS_CAM && _GD->m_mouseState->rgbButtons[0] && !(_GD->m_prevMouseState->rgbButtons[0]))
		{
			Fire();
		}
	}
	else
	{
		//Fire at random.
		float r = (float)rand() / (float)RAND_MAX;
		if (r < 0.2 *_GD->m_dt)
		{
			Fire();
		}
	}
	
	//Tick the bullets.
	for (auto&& bull : m_Bullets)
	{
		bull->Bullet::Tick(_GD);
	}
}


void Gun::UpdateCurrentDirection(char axis, float rotation)
{
	switch (axis)
	{
	case 'x':
		//Pitch.
		currentDirection.z = cosf(rotation);
		currentDirection.y = sinf(rotation);
		break;
	case 'y':
		//Yaw.
		currentDirection.z = -cosf(rotation);
		currentDirection.x = -sinf(rotation);
		break;
	case 'z':
		//Roll.
		currentDirection.y = cosf(rotation);
		currentDirection.x = sinf(rotation);
		break;
	}
}


void Gun::Draw(ID3D11DeviceContext* _pd3dImmediateContext, CommonStates* _states, Camera* _cam)
{
	//Draw all the living bullets.
	for (auto&& bull : m_Bullets)
	{
		if (bull->isAlive())
		{
			bull->Bullet::Draw(_pd3dImmediateContext, _states, _cam);
		}
	}
	CMOGO::Draw(_pd3dImmediateContext, _states, _cam);
}


void Gun::Fire()
{
	//Get the first free bullet and do the fire logic on it.
	for (vector<Bullet*>::iterator it = m_Bullets.begin(); it != m_Bullets.end(); it++)
	{
		if (!(*it)->isAlive())
		{
			Vector3 bulletPos = Vector3::Transform(Vector3::Zero, m_fudge.Invert() * m_worldMat);
			Vector3 bulletDir = Vector3::Transform(Vector3::Forward, m_fudge.Invert() * Matrix::CreateFromYawPitchRoll(m_parent->GetYaw() + m_yaw, m_parent->GetPitch() + m_pitch, m_parent->GetRoll() + m_roll));
			Vector3 bulletUp = Vector3::Transform(Vector3::Up, m_fudge.Invert() * m_worldMat);

			//Here we calculate the direction and location of the fired bullet.
			bulletDir.Normalize();
			bulletUp.Normalize();
			(*it)->Spawn(3.0f, bulletPos, bulletDir, bulletUp);

			break;
		}
	}
}