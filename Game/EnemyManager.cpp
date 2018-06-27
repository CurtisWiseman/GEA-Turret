#include "EnemyManager.h"
#include "Enemy.h"
#include <time.h>

EnemyManager::EnemyManager(int _numEnemies, string _enemyFileName, ID3D11Device* _pd3dDevice, IEffectFactory* _EF)
{
	//Set up all the enemies, and initialise the random seed.
	for (int i = 0; i < _numEnemies; i++)
	{
		m_Enemies.push_back(new Enemy(_enemyFileName, _pd3dDevice, _EF));
	}
	srand(static_cast<unsigned int>(time(NULL)));
}


EnemyManager::~EnemyManager()
{

}


void EnemyManager::Tick(GameData* _GD)
{
	//Randomly spawn in enemies within the bounds we have set.
	const float terrainBounds = 200.0f;

	float r = (float)rand() / (float)RAND_MAX;
	if (r < 0.2 *_GD->m_dt)
	{
		for (auto&& en : m_Enemies)
		{
			if (!(en->isAlive()))
			{
				float startX = -terrainBounds + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (terrainBounds - -terrainBounds)));
				float startY = 0.0f;
				float startZ = -terrainBounds + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (terrainBounds - -terrainBounds)));
				Vector3 newPos = Vector3(startX, startY, startZ);
				en->Spawn(newPos);
				break;
			}
		}
	}

	//For each living enemy, tick their behaviour.
	for (auto&& en : m_Enemies)
	{
		if (en->isAlive())
		{
			en->Tick(_GD);
		}
	}
}


void EnemyManager::Draw(ID3D11DeviceContext* _pd3dImmediateContext, CommonStates* _states, Camera* _cam)
{
	//For each living enemy, render them.
	for (auto&& en : m_Enemies)
	{
		if (en->isAlive())
		{
			en->Draw(_pd3dImmediateContext, _states, _cam);
		}
	}
}