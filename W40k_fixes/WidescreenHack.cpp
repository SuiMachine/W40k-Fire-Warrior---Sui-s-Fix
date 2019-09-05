#include "WidescreenHack.h"

WidescreenHack::WidescreenHack()
{
}

void WidescreenHack::SetAspect(float aspect)
{
	this->aspect = aspect;
	HMODULE mod = GetModuleHandle(NULL);
	*(float*)((intptr_t)mod + 0x263914) = aspect;
}

void WidescreenHack::OverrideFOV(float fov)
{
	HMODULE mod = GetModuleHandle(NULL);
	*(float*)((intptr_t)mod + 0x29F938) = fov * 0.01745329300562541;
}
