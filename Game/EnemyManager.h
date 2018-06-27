#ifndef _ENEMY_MANAGER_H_
#define _ENEMY_MANAGER_H_
#include "GameObject.h"
#include <list>
#include "Model.h"
#include <string>

using namespace std;
using namespace DirectX;

class Enemy;

/*
* The manager that handles enemy spawning.
*/
class EnemyManager : public GameObject
{
public:
	EnemyManager(int _numEnemies, string _enemyFileName, ID3D11Device* _pd3dDevice, IEffectFactory* _EF);
	~EnemyManager();

	//Getter for the enemies to pass into collision handler.
	std::vector<Enemy*> getEnemies() { return m_Enemies; }
	
	//Specialised versions of basic functions.
	virtual void Tick(GameData* _GD) override;
	virtual void Draw(ID3D11DeviceContext* _pd3dImmediateContext, CommonStates* _states, Camera* _cam) override;

protected:
	std::vector<Enemy*> m_Enemies;
};

#endif