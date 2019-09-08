#include "W40kHacks.h"
#include "HookFunctions.h"
#include "../externals/inireader/IniReader.h"

W40kHacks::W40kHacks()
{
	HMODULE mod = GetModuleHandle(NULL);
	UnprotectModule(mod);

	//IniReader
	CIniReader iniReader("");
	this->isBorderless = iniReader.ReadBoolean("MAIN", "Borderless", 0);
	this->PosX = iniReader.ReadInteger("MAIN", "BorderlessPositionX", 0);
	this->PosY = iniReader.ReadInteger("MAIN", "BorderlessPositionY", 0);
	float FOV = iniReader.ReadFloat("MAIN", "FOV", 90);
	if (FOV < 60)
		this->DesiredFOV = 60;
	else if (FOV > 170)
		this->DesiredFOV = 170;
	else
		this->DesiredFOV = FOV;


	wideScreenHack = new WidescreenHack();

}

void W40kHacks::Update(int Width, int Height)
{
	this->Width = Width;
	this->Height = Height;
	wideScreenHack->SetAspect(Width * 1.0f / Height);
	wideScreenHack->OverrideFOV(this->DesiredFOV);
}
