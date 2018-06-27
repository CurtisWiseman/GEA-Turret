#include "Bullet.h"
#include "GameData.h"

Bullet::Bullet(string _fileName, ID3D11Device* _pd3dDevice, IEffectFactory* _EF) :CMOGO( _fileName, _pd3dDevice, _EF)
{
	m_alive = false;
	m_lifeLeft = 0.0f;
	m_fudge = Matrix::Identity;
	SetRadius(5);
}


Bullet::~Bullet()
{

}


void Bullet::Tick(GameData* _GD)
{
	//If alive, tick down the life counter and move in the correct direction.
	if ( m_alive)
	{ 
		m_lifeLeft -= _GD->m_dt;
		if (m_lifeLeft <= 0.0f)
		{
			m_alive = false;
			return;
		}

		m_pos += speed * _GD->m_dt * m_dir;

		Matrix  scaleMat = Matrix::CreateScale(m_scale);
		Matrix  rotMat = Matrix::CreateFromYawPitchRoll(m_yaw, m_pitch, m_roll);
		Matrix  transMat = Matrix::CreateTranslation(m_pos);

		m_worldMat = m_fudge * scaleMat * Matrix::CreateWorld(m_pos, m_dir, m_up);
	}
}


void Bullet::Draw(ID3D11DeviceContext* _pd3dImmediateContext, CommonStates* _states, Camera* _cam)
{
	if (m_alive)
	{
		CMOGO::Draw(_pd3dImmediateContext, _states, _cam);
	}
}


void Bullet::Spawn(float _life, Vector3 _pos, Vector3 _dir,Vector3 _up)
{
	//Setup for 'alive' state.
	m_alive = true;
	m_lifeLeft = _life;
	m_pos = _pos;
	m_dir = _dir;
	m_up = _up;
}


void Bullet::Destroy()
{
	//Exiting 'alive' state, doesn't care about positioning since it'll be invisible.
	m_alive = false;
	m_lifeLeft = 0;
}