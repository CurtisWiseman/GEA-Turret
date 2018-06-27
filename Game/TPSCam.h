#include "Camera.h"

/*
* A specialised camera that tracks the player's position and rotation,
* following their movements.
*/
class TPSCam : public Camera
{
public:
	TPSCam(float _fov, float _aspect, float _nearClip, float _farClip, GameObject* _target, Vector3 _up, Vector3 _dpos);
	virtual ~TPSCam();

	//This is the function that keeps it behind the player as they move.
	virtual void Tick(GameData* _gData);

protected:
	GameObject* m_targetObject;
	Vector3 m_dpos;
};