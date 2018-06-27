#pragma once
#ifndef _GUN_H_
#define _GUN_H_
#include "CMOGO.h"
//#include "Audio.h"
#include <list>
class Bullet;

/*
* The gun to be mounted upon a player vehicle or enemy.
* The gun knows whether it is player controlled or not,
* and acts accordingly.
*/
class Gun : public CMOGO
{
public:
	Gun(string _fileName, ID3D11Device* _pd3dDevice, IEffectFactory* _EF, GameObject* _parent, string _bulletFileName, int _numBullets);
	virtual ~Gun();

	//Getters.
	std::vector<Bullet*> getBullets() { return m_Bullets; }
	bool getPlayerControlled() { return playerControlled; }

	//Setters.
	void setPlayerControlled(bool _pc) { playerControlled = _pc; }
	
	//Specialised versions of basic functions.
	virtual void Tick(GameData* _GD);
	virtual void Draw(ID3D11DeviceContext* _pd3dImmediateContext, CommonStates* _states, Camera* _cam);

protected:
	void Fire();

	GameObject* m_parent;
	Vector3 currentDirection = Vector3(0, 0, 1);
	Vector3 gunTarget = Vector3::Forward;
	float turnspeed = 0.3 * XM_PI;
	void UpdateCurrentDirection(char axis, float rotation);
	std::vector<Bullet*> m_Bullets;
	bool playerControlled = true;
private:

};
#endif