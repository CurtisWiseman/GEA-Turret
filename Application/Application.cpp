//--------------------------------------------------------------------------------------
// This class is a direct re-build of the following Microsoft example Tutorial.
//--------------------------------------------------------------------------------------
// File: Tutorial01.cpp
//
// This application demonstrates creating a Direct3D 11 device
//
// http://msdn.microsoft.com/en-us/library/windows/apps/ff729718.aspx
//
// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
// PARTICULAR PURPOSE.
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//--------------------------------------------------------------------------------------

#include "Application.h"
#include "../Game/Game.h"

#include <WinSDKVer.h>
#define _WIN32_WINNT 0x0602
#include <SDKDDKVer.h>

ID3D11Texture2D*        g_pDepthStencil = NULL;
ID3D11DepthStencilView* g_pDepthStencilView = nullptr;
ID3D11RasterizerState*  g_pRasterState = NULL;


#define DESTROY( x ) if( x ){ x->Release(); x = nullptr;}

//--------------------------------------------------------------------------------------
// Forward Deceleration of Window Procedure
//--------------------------------------------------------------------------------------
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);

//--------------------------------------------------------------------------------------
// Register class and create window
//--------------------------------------------------------------------------------------
HRESULT Application::InitWindow( HINSTANCE hInstance, int nCmdShow )
{
    // Register class
    WNDCLASSEX wcex;
    wcex.cbSize = sizeof( WNDCLASSEX );
    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon( hInstance, ( LPCTSTR )IDI_TUTORIAL1 );
    wcex.hCursor = LoadCursor( nullptr, IDC_ARROW );
    wcex.hbrBackground = ( HBRUSH )( COLOR_WINDOW + 1 );
    wcex.lpszMenuName = nullptr;
    wcex.lpszClassName = L"TutorialWindowClass";
    wcex.hIconSm = LoadIcon( wcex.hInstance, ( LPCTSTR )IDI_TUTORIAL1 );
    if( !RegisterClassEx( &wcex ) )
        return E_FAIL;

    // Create window
    m_hInst = hInstance;

	bool isFullScreen = 0;					//Fullscreen on startup true/false

	if (isFullScreen)
	{
		int SCREEN_WIDTH = GetSystemMetrics(SM_CXSCREEN);
		int SCREEN_HEIGHT = GetSystemMetrics(SM_CYSCREEN);
		m_hWnd = CreateWindowEx(NULL,
			L"TutorialWindowClass",
			L"GEA Tutorial",
			WS_EX_TOPMOST | WS_POPUP,		//Fullscreen values?
			0, 0,							//Starting values for x and y at top left
			SCREEN_WIDTH, SCREEN_HEIGHT,
			NULL,
			NULL,
			hInstance,
			NULL);
	}
	else
	{
		RECT rc = { 0, 0, 800, 600 };
		AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, FALSE);
		m_hWnd = CreateWindow(L"TutorialWindowClass", L"GEA Tutorial",
			WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
			CW_USEDEFAULT, CW_USEDEFAULT, rc.right - rc.left,
			rc.bottom - rc.top, nullptr, nullptr, hInstance,
			nullptr);
	}

    if( !m_hWnd )
        return E_FAIL;

    ShowWindow( m_hWnd, nCmdShow );
	
    return S_OK;
}

//--------------------------------------------------------------------------------------
// Create Direct3D device and swap chain
//--------------------------------------------------------------------------------------
HRESULT Application::InitDevice()
{
    HRESULT hr = S_OK;

    RECT rc;
    GetClientRect( m_hWnd, &rc );
    UINT width = rc.right - rc.left;
    UINT height = rc.bottom - rc.top;

    UINT createDeviceFlags = 0;
#ifdef _DEBUG
    createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

    D3D_DRIVER_TYPE driverTypes[] =
    {
        D3D_DRIVER_TYPE_HARDWARE,
        D3D_DRIVER_TYPE_WARP,
        D3D_DRIVER_TYPE_REFERENCE,
    };
    UINT numDriverTypes = ARRAYSIZE( driverTypes );

    D3D_FEATURE_LEVEL featureLevels[] =
    {
        D3D_FEATURE_LEVEL_11_1,
        D3D_FEATURE_LEVEL_11_0,
        D3D_FEATURE_LEVEL_10_1,
        D3D_FEATURE_LEVEL_10_0,
    };
	UINT numFeatureLevels = ARRAYSIZE( featureLevels );

    for( UINT driverTypeIndex = 0; driverTypeIndex < numDriverTypes; driverTypeIndex++ )
    {
        m_driverType = driverTypes[driverTypeIndex];
        hr = D3D11CreateDevice( nullptr, m_driverType, nullptr, createDeviceFlags, featureLevels, numFeatureLevels,
                                D3D11_SDK_VERSION, &m_pd3dDevice, &m_featureLevel, &m_pImmediateContext );

        if ( hr == E_INVALIDARG )
        {
            // DirectX 11.0 platforms will not recognize D3D_FEATURE_LEVEL_11_1 so we need to retry without it
            hr = D3D11CreateDevice( nullptr, m_driverType, nullptr, createDeviceFlags, &featureLevels[1], numFeatureLevels - 1,
                                    D3D11_SDK_VERSION, &m_pd3dDevice, &m_featureLevel, &m_pImmediateContext );
        }

        if( SUCCEEDED( hr ) )
            break;
    }
    if( FAILED( hr ) )
        return hr;

    // Obtain DXGI factory from device (since we used nullptr for pAdapter above)
    IDXGIFactory1* dxgiFactory = nullptr;
    {
        IDXGIDevice* dxgiDevice = nullptr;
        hr = m_pd3dDevice->QueryInterface( __uuidof(IDXGIDevice), reinterpret_cast<void**>(&dxgiDevice) );
        if (SUCCEEDED(hr))
        {
            IDXGIAdapter* adapter = nullptr;
            hr = dxgiDevice->GetAdapter(&adapter);
            if (SUCCEEDED(hr))
            {
                hr = adapter->GetParent( __uuidof(IDXGIFactory1), reinterpret_cast<void**>(&dxgiFactory) );
                adapter->Release();
            }
            dxgiDevice->Release();
        }
    }
    if (FAILED(hr))
        return hr;

    // Create swap chain
    IDXGIFactory2* dxgiFactory2 = nullptr;
    hr = dxgiFactory->QueryInterface( __uuidof(IDXGIFactory2), reinterpret_cast<void**>(&dxgiFactory2) );
    if ( dxgiFactory2 )
    {
        // DirectX 11.1 or later
        hr = m_pd3dDevice->QueryInterface( __uuidof(ID3D11Device1), reinterpret_cast<void**>(&m_pd3dDevice1) );
        if (SUCCEEDED(hr))
        {
            (void) m_pImmediateContext->QueryInterface( __uuidof(ID3D11DeviceContext1), reinterpret_cast<void**>(&m_pImmediateContext1) );
        }

        DXGI_SWAP_CHAIN_DESC1 sd;
        ZeroMemory(&sd, sizeof(sd));
        sd.Width = width;
        sd.Height = height;
        sd.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        sd.SampleDesc.Count = 1;
        sd.SampleDesc.Quality = 0;
        sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        sd.BufferCount = 1;

        hr = dxgiFactory2->CreateSwapChainForHwnd( m_pd3dDevice, m_hWnd, &sd, nullptr, nullptr, &m_pSwapChain1 );
        if (SUCCEEDED(hr))
        {
            hr = m_pSwapChain1->QueryInterface( __uuidof(IDXGISwapChain), reinterpret_cast<void**>(&m_pSwapChain) );
        }

        dxgiFactory2->Release();
    }
    else
    {
        // DirectX 11.0 systems
        DXGI_SWAP_CHAIN_DESC sd;
        ZeroMemory(&sd, sizeof(sd));
        sd.BufferCount = 1;
        sd.BufferDesc.Width = width;
        sd.BufferDesc.Height = height;
        sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        sd.BufferDesc.RefreshRate.Numerator = 60;
        sd.BufferDesc.RefreshRate.Denominator = 1;
        sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        sd.OutputWindow = m_hWnd;
        sd.SampleDesc.Count = 1;
        sd.SampleDesc.Quality = 0;
        sd.Windowed = TRUE;

        hr = dxgiFactory->CreateSwapChain( m_pd3dDevice, &sd, &m_pSwapChain );
    }

    // Note this tutorial doesn't handle full-screen swapchains so we block the ALT+ENTER shortcut
    //dxgiFactory->MakeWindowAssociation( m_hWnd, DXGI_MWA_NO_ALT_ENTER );

    dxgiFactory->Release();

    if (FAILED(hr))
        return hr;

    // Create a render target view
    ID3D11Texture2D* pBackBuffer = nullptr;
    hr = m_pSwapChain->GetBuffer( 0, __uuidof( ID3D11Texture2D ), reinterpret_cast<void**>( &pBackBuffer ) );
    if( FAILED( hr ) )
        return hr;

    hr = m_pd3dDevice->CreateRenderTargetView( pBackBuffer, nullptr, &m_pRenderTargetView );
    pBackBuffer->Release();
    if( FAILED( hr ) )
        return hr;

    m_pImmediateContext->OMSetRenderTargets( 1, &m_pRenderTargetView, nullptr );

    // Setup the viewport
    D3D11_VIEWPORT vp;
    vp.Width = (FLOAT)width;
    vp.Height = (FLOAT)height;
    vp.MinDepth = 0.0f;
    vp.MaxDepth = 1.0f;
    vp.TopLeftX = 0;
    vp.TopLeftY = 0;
    m_pImmediateContext->RSSetViewports( 1, &vp );

	//SS Create depth stencil texture
	D3D11_TEXTURE2D_DESC descDepth;
	ZeroMemory(&descDepth, sizeof(descDepth));
	descDepth.Width = width;
	descDepth.Height = height;
	descDepth.MipLevels = 1;
	descDepth.ArraySize = 1;
	descDepth.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	descDepth.SampleDesc.Count = 1;
	descDepth.SampleDesc.Quality = 0;
	descDepth.Usage = D3D11_USAGE_DEFAULT;
	descDepth.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	descDepth.CPUAccessFlags = 0;
	descDepth.MiscFlags = 0;
	hr = m_pd3dDevice->CreateTexture2D(&descDepth, NULL, &g_pDepthStencil);
	if (FAILED(hr))
		return hr;

	//SS Create the depth stencil view
	D3D11_DEPTH_STENCIL_VIEW_DESC descDSV;
	ZeroMemory(&descDSV, sizeof(descDSV));
	descDSV.Format = descDepth.Format;
	descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	descDSV.Texture2D.MipSlice = 0;
	hr = m_pd3dDevice->CreateDepthStencilView(g_pDepthStencil, &descDSV, &g_pDepthStencilView);
	if (FAILED(hr))
		return hr;

	m_pImmediateContext->OMSetRenderTargets(1, &m_pRenderTargetView, g_pDepthStencilView);

	//Setup Raster State
	D3D11_RASTERIZER_DESC rasterDesc;
	rasterDesc.AntialiasedLineEnable = false;
	rasterDesc.CullMode = D3D11_CULL_NONE;
	rasterDesc.DepthBias = 0;
	rasterDesc.DepthBiasClamp = 0.0f;
	rasterDesc.DepthClipEnable = true;
	rasterDesc.FillMode = D3D11_FILL_SOLID;
	rasterDesc.FrontCounterClockwise = false;
	rasterDesc.MultisampleEnable = false;
	rasterDesc.ScissorEnable = false;
	rasterDesc.SlopeScaledDepthBias = 0.0f;

	// Create the rasterizer state from the description we just filled out.
	hr = m_pd3dDevice->CreateRasterizerState(&rasterDesc, &g_pRasterState);
	if (FAILED(hr))
		return hr;

	m_pImmediateContext->RSSetState(g_pRasterState);

	//actually create the game
	m_Game = new Game(m_pd3dDevice,m_hWnd,m_hInst);

    return S_OK;
}


//--------------------------------------------------------------------------------------
// Render the frame
//--------------------------------------------------------------------------------------
void Application::Render()
{
    // Just clear the backbuffer
   // m_pImmediateContext->ClearRenderTargetView( m_pRenderTargetView, Colors::MidnightBlue );
    //m_pSwapChain->Present( 0, 0 );

	//m_Game->Draw(m_pImmediateContext);


	// Just clear the backbuffer
	m_pImmediateContext->ClearRenderTargetView(m_pRenderTargetView, Colors::MidnightBlue);
	// Clear the depth stencil
	m_pImmediateContext->ClearDepthStencilView(g_pDepthStencilView, D3D11_CLEAR_DEPTH, 1.0, 0);

	if (m_Game) m_Game->Draw(m_pImmediateContext); //SS Render the next frame from the game

	m_pSwapChain->Present(0, 0);
}


//--------------------------------------------------------------------------------------
// Update the Frame
//--------------------------------------------------------------------------------------
bool Application::Update()
{
	if (m_Game)
	{
		return m_Game->Tick();
	}

	return false;
}


//--------------------------------------------------------------------------------------
// Clean up the objects we've created
//--------------------------------------------------------------------------------------
void Application::CleanupDevice()
{
	//Tidy up all DirectX stuff
    if( m_pImmediateContext ) m_pImmediateContext->ClearState();

	if (g_pRasterState)		g_pRasterState->Release();//SS
	if (g_pDepthStencil) g_pDepthStencil->Release();//SS to get rid of depth stuff
	if (g_pDepthStencilView) g_pDepthStencilView->Release();//SS

	DESTROY(m_pRenderTargetView);
	DESTROY(m_pSwapChain1);
	DESTROY(m_pSwapChain);
	DESTROY(m_pImmediateContext1);
	DESTROY(m_pImmediateContext);
	DESTROY(m_pd3dDevice1);
	DESTROY(m_pd3dDevice);
}
