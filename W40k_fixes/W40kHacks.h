#pragma once
#include "WidescreenHack.h"

class W40kHacks
{
public:
	W40kHacks();
	void Update(int Width, int Height);
	bool isBorderless;
	int PosX, PosY;
private:
	WidescreenHack* wideScreenHack;
	int Width, Height;
	float DesiredFOV;
};

