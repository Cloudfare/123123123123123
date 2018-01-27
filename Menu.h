#pragma once
#include <string>
#include <Windows.h>
#include <vector>
#include "controls.h"
using namespace std;
/*Include menu elements*/
extern CRagebotElements RagebotElements;
extern CMiniWindows MiniWindowElements;
extern CVisualElements VisualElements;
extern CMiscElements MiscElements;
extern CColorElements ColorElements;
extern CLegitbotElements LegitbotElements;
//skinchanger not needed in bf1
/*class MSkinchanger : public MBaseElement
{
protected:
Skins::vecSkinInfo currentWep;
std::string currentWepName;
public:
MSkinchanger() {}
MCategory* parent;
MSkinchanger::MSkinchanger(MCategory* parent);
void DrawCheckbox(int x, int y, bool & booli, std::string Name);
virtual void Draw() override;
void DrawList1();
void DrawList2();
void DrawSlider(int posx, int posy, const char* Name, CVars* Var, int min, int max, bool bActive);

};*/

class CMenu
{
public:
	CMenu();
	//void InitConfig(HMODULE hModule);
	//void InitSkins();
	void SaveWindowState(std::string Filename);
	void LoadWindowState(std::string Filename);
	//void ReadOrWriteConfig(const char* szPath, bool bRead);
	float alpha = 0;
	MWindow* MainWindow = nullptr;
	bool initialized = false;

}; extern CMenu* m_pMenu;



