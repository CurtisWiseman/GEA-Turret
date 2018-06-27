#ifndef _ENEMY_H_
#define _ENEMY_H_
#include "CMOGO.h"

class Gun;


/*
* A non-player-controlled vehicle that moves and shoots randomly.
*/
class Enemy : public CMOGO
{
public:
	Enemy(string _fileName, ID3D11Device* _pd3dDevice, IEffectFactory* _EF);
	~Enemy();

	//Functions so we can load the object and create it separately.
	void Spawn(Vector3 _pos);
	void Destroy();

	//Specialised versions of basic functions.
	virtual void Tick(GameData* _GD) override;
	virtual void Draw(ID3D11DeviceContext* _pd3dImmediateContext, CommonStates* _states, Camera* _cam) override;

	//Getters for 'alive' state and the gun.
	bool isAlive(){ return m_alive; }
	Gun* getGun() { return gun;	}

protected:
	bool m_alive;
	Vector3 m_target;
	float turnSpeed = XM_PI;
	float moveSpeed = 60.0f;
	Gun* gun = nullptr;

	void setRandomTarget();
	bool isCloseToTarget(char dir);
	void targetSettingAndMovement(GameData* _GD);
	void fixRotation(GameData* _GD);
};

#endif