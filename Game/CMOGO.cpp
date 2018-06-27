#include "CMOGO.h"
#include "Model.h"
#include <windows.h>
#include "Effects.h"
#include "Camera.h"
#include "CommonStates.h"

extern ID3D11RasterizerState*  g_pRasterState;

using namespace DirectX;

//helper function to convert to odd char type used by Microsoft stuff
wchar_t* CMOGO::charToWChar(const char* _text)
{
	size_t size = strlen(_text) + 1;
	static wchar_t* wa = NULL;
	if (wa)
	{
		delete[] wa;
		wa = NULL;
	}
	wa = new wchar_t[size];
	mbstowcs_s(nullptr, wa, size, _text, size);
	return wa;
}


CMOGO::CMOGO(string _fileName, ID3D11Device* _pd3dDevice, IEffectFactory* _EF) :m_model(nullptr)
{
	string filePath =
#ifdef DEBUG
		"../Debug/"
#else
		"../Release/"
#endif
		+ _fileName;

	wchar_t* file = charToWChar(filePath.c_str());

	m_model = Model::CreateFromCMO(_pd3dDevice, file, *_EF);
	
	SetPos(Vector3(0.0f, 0.0f, 0.0f));
	SetPitch(0.0*XM_PI);
	SetYaw(0.0*XM_PI);
	SetRoll(0.0*XM_PI);

}


CMOGO::~CMOGO()
{
	//model shouldn't need deleting as it's linked to by a unique_ptr
}


void CMOGO::Tick(GameData* _gData)
{
	//SetYaw(m_yaw+_dt);
	GameObject::Tick(_gData);
}


void CMOGO::Draw(ID3D11DeviceContext* _pd3dImmediateContext, CommonStates* _states, Camera* _cam)
{
	m_model->Draw(_pd3dImmediateContext, *_states, //graphics device and CommonStates reuqired by model system
		m_worldMat, //world transform to poisiton this model in the world
		_cam->GetView(), _cam->GetProj(), //veiw and projection matrix of the camera
		false, //NO! I don't want wireframe
		[&](){_pd3dImmediateContext->RSSetState(g_pRasterState);} //this VERY weird construction creates a function on the fly to set up the render states correctly else the model system overrides them BADLY
		);

}