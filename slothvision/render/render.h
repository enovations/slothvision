#pragma once

#include "../network/CameraStream.h"
#include "../sensors/Sensors.h"

class Render
{
public:
	CameraStream* leftCameraStream;
	CameraStream* rightCameraStream;
	Sensors* sensors;

public:
	Render();

	void start(HINSTANCE hinst);
};