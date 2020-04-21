#pragma once
#ifndef CAMERA_H
#define CAMERA_H

#include "GaiaComponent.h"
#include "Vector3.h"
#include "Quaternion.h"

namespace Ogre {
	class Camera;
}

class GAIA_API Camera : public GaiaComponent
{
private:
	Ogre::Camera* camera;
	bool isMainCamera;

public:
	Camera(GameObject* gameObject);
	~Camera();

	enum SpaceReference { LocalSpace, WorldSpace, ParentSpace };

	void lookAt(const Vector3& pos, SpaceReference space);

	Vector3 getDirection() const;
	void setDirection(const Vector3& dir);
	Quaternion getOrientation() const;
	void setOrientation(const Quaternion& q);

	Ogre::Camera* getCamera();

	Vector3 worldToScreen(const Vector3& worldPoint);

	Vector3 worldToScreenPixel(const Vector3& worldPoint);

	void setClipDistances(double near, double far);

	virtual void handleData(ComponentData* data);

};

#endif