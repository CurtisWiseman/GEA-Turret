#include "Terrain.h"

Terrain::Terrain(string _fileName, ID3D11Device* _pd3dDevice, IEffectFactory* _EF, Vector3 _pos, float _pitch, float _yaw, float _roll, Vector3 _scale, GameData* _gData) :CMOGO(_fileName, _pd3dDevice, _EF)
{
	m_pos = _pos;
	m_pitch = _pitch;
	m_roll = _roll;
	m_yaw = _yaw;
	m_scale = _scale;

	GameObject::Tick(_gData); //Do the initial update on my world_transform.
}


Terrain::~Terrain()
{
	//Nothing additional here but add this just in case.
}