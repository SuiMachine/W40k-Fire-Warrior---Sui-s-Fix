#include "WidescreenHack.h"
# define M_PI           3.14159265358979323846  /* pi */
#include <math.h>


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
	double dHorizontalRadians = M_PI * fov / 180;
	double dVerticalRadians = 2 * atan(tan(dHorizontalRadians / 2) * (3.0 / 4.0));
	//printf("Initial FOV calculation (4:3): %.3d (hor) %.3d (vert)\n", dHorizontalRadians, dVerticalRadians);
	dHorizontalRadians = 2 * atan(tan(dVerticalRadians / 2) * this->aspect);

	*(float*)((intptr_t)mod + 0x29F938) = dHorizontalRadians;
}
