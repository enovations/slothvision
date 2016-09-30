#pragma once

#include "../CameraStream.h"

class Render
{
public:
	CameraStream* _leftCameraStream;
	CameraStream* _rightCameraStream;

public: 
	void start(HINSTANCE hinst);

	void setLeftCameraStream(CameraStream* leftCameraStream);
	void setRightCameraStream(CameraStream* rightCameraStream);
};