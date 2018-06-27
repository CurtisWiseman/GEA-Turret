#ifndef _VEHICLE_H_
#define _VEHICLE_H_
#include "CMOGO.h"
//#include "Audio.h"

class Gun;

/*
* The player 'character', has a choice of controls between 'tank' and 'mouse'.
*/
class Vehicle : public CMOGO
{
public:
	Vehicle(string _fileName, ID3D11Device* _pd3dDevice, IEffectFactory* _EF);
	~Vehicle();

	//Getters.
	Gun* getGun() { return gun;	}
	bool isAlive() { return m_alive; }

	//Functions to allow for asynchrous loading and creation.
	void Destroy();
	void Spawn();

	//Specialised versions of basic functions.
	virtual void Tick(GameData* _gData) override;
	virtual void Draw(ID3D11DeviceContext* _pd3dImmediateContext, CommonStates* _states, Camera* _cam) override;

protected:
	void UpdateCurrentDirection(char axis, float rotation);

	bool m_alive;
	float movespeed = 60.0f;
	float turnspeed = 0.7 * XM_PI;
	Gun* gun = nullptr;
private:

};
#endif