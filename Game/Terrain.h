#ifndef _TERRAIN_H_
#define _TERRAIN_H_
#include "CMOGO.h"

/*
* A non-moving terrain object.
*/
class Terrain : public CMOGO
{
public:
	Terrain(string _fileName, ID3D11Device* _pd3dDevice, IEffectFactory* _EF, Vector3 _pos, float _pitch, float _yaw, float _roll, Vector3 _scale, GameData* _gData);
	~Terrain();

protected:
};
#endif