#pragma once

#include "../network/CameraStream.h"

class Render
{
protected:
	CameraStream* _leftCameraStream;
	CameraStream* _rightCameraStream;

public: 
	void start(HINSTANCE hinst);

	void setLeftCameraStream(CameraStream* leftCameraStream);
	void setRightCameraStream(CameraStream* rightCameraStream);

	CameraStream* getLeftCameraStream() const;
	CameraStream* getRightCameraStream() const;
};