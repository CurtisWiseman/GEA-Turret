#ifndef _CMOGO_H_
#define _CMOGO_H_
#include "GameObject.h"
#include "Model.h"
#include <string>

using namespace std;
using namespace DirectX;


/*
* A GameObject with a model attached.
*/
class CMOGO : public GameObject
{
public:
	CMOGO(string _fileName, ID3D11Device* _pd3dDevice, IEffectFactory* _EF);
	virtual ~CMOGO();

	//Specialised versions of basic functions.
	virtual void Tick(GameData* _gData) override;
	virtual void Draw( ID3D11DeviceContext* _pd3dImmediateContext,  CommonStates* _states, Camera* _cam) override;

	//Necessary for converting normal text into the kind DirectX can read.
	static wchar_t* charToWChar(const char* _text);


protected:
	unique_ptr<Model>  m_model;
};

#endif
