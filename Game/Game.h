#ifndef _GAME_H_
#define _GAME_H_
//=================================================================
//Basic Game Manager Class
//=================================================================

#include <windows.h>
//#include <Audio.h>
#include <list>
#include "Effects.h"
#include "CommonStates.h"
#include <dinput.h>
#include "GameData.h"
#include "SpriteBatch.h"
#include "SpriteFont.h"
#include "Vehicle.h"
#include "EnemyManager.h"

using std::list;
using std::unique_ptr;
using namespace DirectX;

// Forward declarations
class GameObject;
class Camera;

class Game
{
public:

	Game(ID3D11Device* _pd3dDevice, HWND _hWnd, HINSTANCE _hInstance);
	virtual ~Game();

	bool Tick();
	void Draw(ID3D11DeviceContext* _pd3dImmediateContext);

protected:
	//input variables
	IDirectInput8* m_pDirectInput;
	IDirectInputDevice8* m_pKeyboard;
	unsigned char m_keyboardState[256];
	unsigned char m_keyboardPrevState[256];
	IDirectInputDevice8* m_pMouse;
	DIMOUSESTATE m_mouseState;
	DIMOUSESTATE m_mousePrevState;

	DWORD m_playTime;  //Amount of time since the game started.
	GameData* m_gData; //All the useful tick variables to pass down the chain.
	Camera* m_cam;     //Principle camera.
	Camera* m_tpsCam;  //Third-Person camera.
	HWND m_hWnd;       //Window properties.
	
	//Data structure storing all GameObjects of this Game.
	list<GameObject *> m_GameObjects;
	Vehicle* m_jeep = nullptr;
	EnemyManager* m_enemy_man = nullptr;
	const int num_enemies = 10;

	//Required for the CMO model rendering system.
	CommonStates* m_states;
	IEffectFactory* m_fxFactory;

	bool ReadKeyboard();
	bool ReadMouse();
	void CheckCollisions();

	//Required for the SpriteBatch system.
	unique_ptr<SpriteBatch> m_Sprites;
	unique_ptr<SpriteFont> m_Font;

	//Audio engine. Currently broken.
	//std::unique_ptr<AudioEngine> m_audioEngine;

};


#endif