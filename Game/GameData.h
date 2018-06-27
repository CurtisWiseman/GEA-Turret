#pragma once
#include <dinput.h>

/*
* A data-only class to hold all the variables we want to pass around between objects.
*/
struct GameData
{
	//Options for enemy movement
	const int MovementType  = 1;
	const int RotationType = 1;

	//The amount of time that has passed since the last tick.
	float m_dt;
	
	//The player's score
	int score = 0;

	//The different input states, as well as previous states for comparison.
	unsigned char* m_keyboardState = nullptr;
	unsigned char* m_prevKeyboardState = nullptr;
	DIMOUSESTATE* m_mouseState = nullptr;
	DIMOUSESTATE* m_prevMouseState = nullptr;

	//The states for the camera system.
	enum GameState
	{
		GS_PLAY_MAIN_CAM,
		GS_PLAY_TPS_CAM
	};
	GameState currentState;
};