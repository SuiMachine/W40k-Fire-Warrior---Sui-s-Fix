#pragma once
#include <Windows.h>

class WidescreenHack
{
public: 
	WidescreenHack();
	void SetAspect(float aspect);
	void OverrideFOV(float fov);
private:
	float aspect;
};

