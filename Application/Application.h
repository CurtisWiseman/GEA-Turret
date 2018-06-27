#ifndef  _APPLICATION_H_
#define  _APPLICATION_H_
#include <windows.h>
#include <d3d11_1.h>
#include <directxcolors.h>
#include "resource.h"

using namespace DirectX;

class Game;
class Application
{
public:

	HRESULT InitWindow(HINSTANCE hInstance, int nCmdShow);
	HRESULT InitDevice();
	void CleanupDevice();
	void Render();
	bool Update();

protected:

	HINSTANCE               m_hInst = nullptr;
	HWND                    m_hWnd = nullptr;
	D3D_DRIVER_TYPE         m_driverType = D3D_DRIVER_TYPE_NULL;
	D3D_FEATURE_LEVEL       m_featureLevel = D3D_FEATURE_LEVEL_11_0;
	ID3D11Device*           m_pd3dDevice = nullptr;
	ID3D11Device1*          m_pd3dDevice1 = nullptr;
	ID3D11DeviceContext*    m_pImmediateContext = nullptr;
	ID3D11DeviceContext1*   m_pImmediateContext1 = nullptr;
	IDXGISwapChain*         m_pSwapChain = nullptr;
	IDXGISwapChain1*        m_pSwapChain1 = nullptr;
	ID3D11RenderTargetView* m_pRenderTargetView = nullptr;

	Game* m_Game = nullptr;
};

#endif // ! _APPLICATION_H_

