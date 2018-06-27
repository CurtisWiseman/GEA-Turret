#ifndef _BULLET_H_
#define _BULLET_H_
#include "CMOGO.h"

/*
* The bullet fired by a gun.
* It takes an initial position and rotation from the parent gun,
* then it moves autonomously.
*/
class Bullet : public CMOGO
{
public:
	Bullet(string _fileName, ID3D11Device* _pd3dDevice, IEffectFactory* _EF);
	~Bullet();

	//Functions so we can load the object and create it separately.
	void Spawn(float _life, Vector3 _pos, Vector3 _dir, Vector3 _up);
	void Destroy();

	//Specialised versions of basic functions.
	virtual void Tick(GameData* _GD) override;
	virtual void Draw(ID3D11DeviceContext* _pd3dImmediateContext, CommonStates* _states, Camera* _cam) override;

	bool isAlive() { return m_alive; }

protected:

	bool m_alive;
	float m_lifeLeft;
	Vector3 m_dir;
	Vector3 m_up;
	float speed = 80.0f;
};
#endif