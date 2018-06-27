#include "Game.h"
#include "GameObject.h"
#include "Camera.h"
#include "TPSCam.h"
#include "SimpleMath.h"
#include "CMOGO.h"
#include "Terrain.h"
#include "GameData.h"
#include "Gun.h"
#include "Bullet.h"
#include "Enemy.h"

using namespace DirectX;
using namespace DirectX::SimpleMath;

Game::Game(ID3D11Device* _pd3dDevice, HWND _hWnd, HINSTANCE _hInstance) 
{
	//Set up audio for windows desktop - BROKEN
	/*
	CoInitializeEx(nullptr, COINIT_MULTITHREADED);
	AUDIO_ENGINE_FLAGS eflags = AudioEngine_Default;
#ifdef _DEBUG
	eflags = eflags | AudioEngine_Debug;
#endif
	m_audioEngine.reset(new AudioEngine(eflags));
	*/


	//Initialise gamedata.
	m_gData = new GameData;
	m_gData->m_keyboardState = m_keyboardState;


	//Set starting camera.
	m_gData->currentState = GameData::GameState::GS_PLAY_MAIN_CAM;


	//Set up DirectXTK Effects system.
	m_fxFactory = new EffectFactory(_pd3dDevice);


	//Nulling of inputs just to be safe.
	m_pKeyboard = nullptr;
	m_pDirectInput = nullptr;
	m_pMouse = nullptr;

	//Tell the fxFactory to look to the correct build directory to pull data in from.
#ifdef DEBUG
	((EffectFactory*)m_fxFactory)->SetDirectory(L"../Debug");
#else
	((EffectFactory*)m_fxFactory)->SetDirectory(L"../Release");
#endif

	// Create other render resources here.
	m_states = new CommonStates(_pd3dDevice);

	//Find how big my window is to correctly calculate my aspect ratio.
	RECT rc;
	GetClientRect(_hWnd, &rc);
	UINT width = rc.right - rc.left;
	UINT height = rc.bottom - rc.top;
	float AR = (float)width / (float)height;

	//Store window properties.
	m_hWnd = _hWnd;

	//Actually creating the inputs.
	HRESULT hr = DirectInput8Create(_hInstance, DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&m_pDirectInput, NULL);
	hr = m_pDirectInput->CreateDevice(GUID_SysKeyboard, &m_pKeyboard, NULL);
	hr = m_pKeyboard->SetDataFormat(&c_dfDIKeyboard);
	hr = m_pKeyboard->SetCooperativeLevel(_hWnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);

	HRESULT mr = DirectInput8Create(_hInstance, DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&m_pDirectInput, NULL);
	mr = m_pDirectInput->CreateDevice(GUID_SysMouse, &m_pMouse, NULL);
	mr = m_pMouse->SetDataFormat(&c_dfDIMouse);
	mr = m_pMouse->SetCooperativeLevel(_hWnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);

	//Loading sound files - BROKEN
	//std::unique_ptr<SoundEffect> soundEffect(new SoundEffect(m_audioEngine.get(), L"..\\Assets\\Gun\\gunshot.wav"));

	//Creating the terain object.
	Terrain* pTerrain = new Terrain("hexatiles.cmo", _pd3dDevice, m_fxFactory, Vector3(0,0,0), 0,0,0, Vector3(2,2,2), m_gData);
	m_GameObjects.push_back(pTerrain);

	//Creating the player.
	Vehicle* pJeep = new Vehicle("jeep.cmo", _pd3dDevice, m_fxFactory);
	m_GameObjects.push_back(pJeep);
	m_jeep = pJeep;

	//Creating the enemymanager.
	EnemyManager* pEnemyMan = new EnemyManager(num_enemies, "jeep.cmo", _pd3dDevice, m_fxFactory);
	m_GameObjects.push_back(pEnemyMan);
	m_enemy_man = pEnemyMan;

	//Create a base camera.
	m_cam = new Camera(0.25f * XM_PI, AR, 1.0f, 10000.0f, Vector3::Zero, Vector3::Up);
	m_cam->SetPos(Vector3(0.0f, 400.0f, 350.0f));
	m_GameObjects.push_back(m_cam);

	//Create a third person camera.
	m_tpsCam = new TPSCam(0.25f *XM_PI, AR, 1.0f, 10000.0f, pJeep, Vector3::Up, Vector3(0, 0, 0));
	m_GameObjects.push_back(m_tpsCam);

	//Hide mouse cursor.
	ShowCursor(false);

	//Device info.
	ID3D11DeviceContext* pd3dImmediateContext;
	_pd3dDevice->GetImmediateContext(&pd3dImmediateContext);

	//Creating objects for the SpriteBatch to use.
	m_Sprites.reset(new SpriteBatch(pd3dImmediateContext));
	m_Font.reset(new SpriteFont(_pd3dDevice, L"italic.spritefont"));
};


Game::~Game() 
{
	//Clearing out the DirectInput objects.
	if (m_pKeyboard)
	{
		m_pKeyboard->Unacquire();
		m_pKeyboard->Release();
	}

	if (m_pMouse)
	{
		m_pMouse->Unacquire();
		m_pMouse->Release();
	}

	if (m_pDirectInput)
	{
		m_pDirectInput->Release();
	}

	//Get rid of the game objects here.
	for (list<GameObject *>::iterator it = m_GameObjects.begin(); it != m_GameObjects.end(); it++)
	{
		delete (*it);
	}
	m_GameObjects.clear();

	//GameData, too, must be deleted.
	delete m_gData;
};


bool Game::Tick() 
{ 
	//tick audioengine - BROKEN
	/*
	if (m_audioEngine->Update())
	{
		if (m_audioEngine->IsCriticalError())
		{
			return false;
		}
	}
	*/

	//Check for bullet collisions.
	CheckCollisions();

	//Get the keyboard input.
	ReadKeyboard();

	//Escape key = SHUTDOWN.
	if (m_keyboardState[DIK_ESCAPE] & 0x80)
	{
		return false;
	}
	
	//Spacebar = Switch camera.
	if ((m_keyboardState[DIK_SPACE] & 0x80) && !(m_keyboardPrevState[DIK_SPACE] & 0x80))
	{
		if (m_gData->currentState == m_gData->GS_PLAY_MAIN_CAM)
		{
			m_gData->currentState = m_gData->GS_PLAY_TPS_CAM;
		}
		else if (m_gData->currentState == m_gData->GS_PLAY_TPS_CAM)
		{
			m_gData->currentState = m_gData->GS_PLAY_MAIN_CAM;
		}
	}

	//Lock cursor to window.
	RECT window;
	GetWindowRect(m_hWnd, &window);
	int cursX = static_cast<int>(0.5f*(window.left + window.right));
	int cursY = static_cast<int>(0.5f*(window.bottom + window.top));
	SetCursorPos(cursX, cursY);


	//Get the mouse input.
	ReadMouse();

	//Calculate frame time-step dt for passing down to game objects.S
	DWORD currentTime = GetTickCount();

	//Update GameData.
	m_gData->m_dt = min((float)(currentTime - m_playTime) / 1000.0f, 0.1f);
	m_gData->m_keyboardState = m_keyboardState;
	m_gData->m_prevKeyboardState = m_keyboardPrevState;
	m_gData->m_mouseState = &m_mouseState;
	m_gData->m_prevMouseState = &m_mousePrevState;

	m_playTime = currentTime;
	
	//Update all objects.
	for (list<GameObject *>::iterator it = m_GameObjects.begin(); it != m_GameObjects.end(); it++)
	{
		(*it)->Tick(m_gData);
	}

	return true; 
};


void Game::Draw(ID3D11DeviceContext* _pd3dImmediateContext) 
{
	switch (m_gData->currentState)
	{
	case GameData::GameState::GS_PLAY_MAIN_CAM:

		//Draw all objects with main camera.
		for (list<GameObject *>::iterator it = m_GameObjects.begin(); it != m_GameObjects.end(); it++)
		{
			(*it)->Draw(_pd3dImmediateContext, m_states, m_cam);
		}
		break;

	case GameData::GameState::GS_PLAY_TPS_CAM:
		
	default:
		//Draw all objects with third-person camera.
		for (list<GameObject *>::iterator it = m_GameObjects.begin(); it != m_GameObjects.end(); it++)
		{
			(*it)->Draw(_pd3dImmediateContext, m_states, m_tpsCam);
		}
		break;
	}

	//Render the score to the window.
	std::string score_str = std::to_string(m_gData->score);
	const char* score_char = score_str.c_str();
	//const char* full_char = "Score: " + *score_char;
	const char* full_char = score_char;

	m_Sprites->Begin();
	m_Font->DrawString(m_Sprites.get(), CMOGO::charToWChar(full_char), XMFLOAT2(100, 10), Colors::Yellow);
	m_Sprites->End();
	
};


bool Game::ReadKeyboard()
{
	//Update the "Previous" key press.
	for (int i = 0; i < 256; i++)
	{
		m_keyboardPrevState[i] = m_keyboardState[i];
	}

	//Clear the previous state.
	ZeroMemory(&m_keyboardState, sizeof(m_keyboardState));

	//Read in the new state.
	HRESULT hr = m_pKeyboard->GetDeviceState(sizeof(m_keyboardState), (LPVOID)&m_keyboardState);

	if (FAILED(hr))
	{
		//If the keyboard failed to read this will try to fix it.
		if ((hr == DIERR_INPUTLOST) || (hr == DIERR_NOTACQUIRED))
		{
			m_pKeyboard->Acquire();
		}
		else
		{
			return false;
		}
	}

	return true;
}


bool Game::ReadMouse()
{
	//Update the 'previous' state.
	m_mousePrevState = m_mouseState;

	//Clear the previous state.
	ZeroMemory(&m_mouseState, sizeof(m_mouseState));

	//Read in the new state.
	HRESULT hr = m_pMouse->GetDeviceState(sizeof(m_mouseState), (LPVOID)&m_mouseState);

	if (FAILED(hr))
	{
		//If the mouse failed to read this will try to fix it.
		if ((hr == DIERR_INPUTLOST) || (hr == DIERR_NOTACQUIRED))
		{
			m_pMouse->Acquire();
		}
		else
		{
			return false;
		}
	}

	return true;
}


void Game::CheckCollisions()
{
	//Loop through enemies.
	for (int i = 0; i < m_enemy_man->getEnemies().size(); i++)
	{
		//Loop through playerbullets.
		for (int j = 0; j < m_jeep->getGun()->getBullets().size(); j++)
		{
			//If enemy collides with playerbullet...
			if (m_enemy_man->getEnemies()[i]->DoRadiiCollide(m_jeep->getGun()->getBullets()[j]) && m_enemy_man->getEnemies()[i]->isAlive() && m_jeep->getGun()->getBullets()[j]->isAlive())
			{
				//...destroy playerbullet and add points to score...
				m_jeep->getGun()->getBullets()[j]->Destroy();
				m_gData->score++;
				//...also destroy enemy.
				m_enemy_man->getEnemies()[i]->Destroy();
			}
			//EndIf
		}
		//EndLoop
		//Loop through enemybullets.
		for (int k = 0; k < m_enemy_man->getEnemies()[i]->getGun()->getBullets().size(); k++)
		{
			//If enemybullet collides with player...
			if (m_jeep->DoRadiiCollide(m_enemy_man->getEnemies()[i]->getGun()->getBullets()[k]) && m_jeep->isAlive() && m_enemy_man->getEnemies()[i]->getGun()->getBullets()[k]->isAlive())
			{
				//...destroy enemybullet and take points off score...
				m_enemy_man->getEnemies()[i]->getGun()->getBullets()[k]->Destroy();
				m_gData->score--;
				//...also destroy player.
				m_jeep->Destroy();
			}
			//EndIf
		}
		//EndLoop
	}
	//EndLoop
}