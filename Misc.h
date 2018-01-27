#pragma once
#include "FixMove.h"
class CMisc
{
public:
	void RankReveal();
	void Namechanger();
	void Bunnyhop();
	void AutoStrafe();
	void FixCmd();
	void FixMovement();
	bool FakeLag();
	void Walkbot();
	void CStrafe_Manual();
	bool IsCircling;
	void CStrafe_New();
	std::vector<Vector> path;
	bool b_CStrafeActive = false;
	void HandleClantag();
	float m_circle_yaw = 0;
}; extern CMisc* g_Misc;