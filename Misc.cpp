#include "sdk.h"
#include "Misc.h"
#include "global.h"
#include <chrono>
#include "GameUtils.h"
#include "Menu.h"
#include "Math.h"
#include "xor.h"
#include <chrono>
CMisc* g_Misc = new CMisc;
//using RevealAllFn = void(*)(int); Double V fix: Can't use this syntax with my VS version!
typedef void(*RevealAllFn)(int);
RevealAllFn fnReveal;
void CMisc::RankReveal()
{
	if (!MiscElements.Misc_General_RevealRanks->Checked)
		return;

	if(!fnReveal)
		fnReveal = (RevealAllFn)Utilities::Memory::FindPattern(XorStr("client.dll"), (PBYTE)XorStr("\x55\x8B\xEC\x8B\x0D\x00\x00\x00\x00\x68\x00\x00\x00\x00"), XorStr("xxxxx????x????"));

	int iBuffer[1];

	if (G::UserCmd->buttons & IN_SCORE)
		fnReveal(iBuffer[1]);
}


void CMisc::Namechanger()
{

	if (MiscElements.Misc_Chat_NameChanger->SelectedIndex == 0)
		return;


	static int changes;

	auto GetEpochTime = []()
	{
		auto duration = std::chrono::system_clock::now().time_since_epoch();

		return std::chrono::duration_cast<std::chrono::milliseconds>(duration).count();
	};

	long currentTime_ms = GetEpochTime();
	static long timeStamp = currentTime_ms;

	if (currentTime_ms - timeStamp < 150)
		return;

	timeStamp = currentTime_ms;
	changes++;


	string name_to_set = "";


	if (MiscElements.Misc_Chat_NameChanger->SelectedIndex == 1)
	{
		static const char* OldName = "";
		if (!G::LocalPlayer)
			return;
		std::vector<std::string> NameList;
		for (int i = 1; i < g_pEngine->GetMaxClients(); i++)
		{
			CBaseEntity* pBaseEntity = g_pEntitylist->GetClientEntity(i);
			if (!pBaseEntity)
				continue;
			if (pBaseEntity->GetTeamNum() != G::LocalPlayer->GetTeamNum())
				continue;
			player_info_t info;
			if (g_pEngine->GetPlayerInfo(i, &info))
				NameList.push_back(info.m_szPlayerName);
		}
		if (!(std::find(NameList.begin(), NameList.end(), OldName) != NameList.end()))
		{
			random_shuffle(NameList.begin(), NameList.end());
			NameList[0].append("  ");
			const char* Name = NameList[0].c_str();
			name_to_set = Name;
			OldName = Name;
		}


	}
	else if (MiscElements.Misc_Chat_NameChanger->SelectedIndex == 2)
	{
		static bool ySwitch = false;

		if (ySwitch)
			name_to_set = XorStr("getcoe.us");
		else
			name_to_set = XorStr("--getcoe.us");

		ySwitch = !ySwitch;
	}
	else if (MiscElements.Misc_Chat_NameChanger->SelectedIndex == 3)
		name_to_set = XorStr("\r");

	else if (MiscElements.Misc_Chat_NameChanger->SelectedIndex == 4)
	{
		static bool ySwitch = false;
		if (ySwitch)
			name_to_set = XorStr("-INTERWEBZ");
		else
			name_to_set = XorStr("INTERWEBZ-");

		ySwitch = !ySwitch;
	}
	else
	{
		static bool ySwitch = false;
		if (ySwitch)
			name_to_set = XorStr(" AIMWARE.net ");
		else
			name_to_set = XorStr(" AIMWARE.net");

		ySwitch = !ySwitch;
	}



	if (changes >= 5)
	{
		string backup = XorStr("\230");
		backup.append(name_to_set);
		name_to_set = backup;
		changes = 0;
	}
	for (int i = 0; i <= changes; i++)
	{
		name_to_set.append(XorStr(" "));
	}
	g_pEngine->SetName(name_to_set);
}

void CMisc::Bunnyhop()
{
	if (MiscElements.Misc_Movement_Bunnyhop->Checked/* && Cvar->FindVar("sv_enablebunnyhopping")->GetValue() != 1*/)
	{
		static auto bJumped = false;
		static auto bFake = false;
		if (!bJumped && bFake)
		{
			bFake = false;
			G::UserCmd->buttons |= IN_JUMP;
		}
		else if (G::UserCmd->buttons & IN_JUMP)
		{
			if (G::LocalPlayer->GetFlags() & FL_ONGROUND)
			{
				bJumped = true;
				bFake = true;
			}
			else
			{
				G::UserCmd->buttons &= ~IN_JUMP;
				bJumped = false;
			}
		}
		else
		{
			bJumped = false;
			bFake = false;
		}
	}
}
template<class T, class U>
inline T clamp(T in, U low, U high)
{
	if (in <= low)
		return low;
	else if (in >= high)
		return high;
	else
		return in;
}
#define CheckIfNonValidNumber(x) (fpclassify(x) == FP_INFINITE || fpclassify(x) == FP_NAN || fpclassify(x) == FP_SUBNORMAL)
void CMisc::AutoStrafe()
{
	if (!MiscElements.Misc_Movement_Autostrafe->Checked)
		return;

	if (!GetAsyncKeyState(VK_SPACE))
		return;

	/* AW*/
	static int old_yaw;

	auto get_velocity_degree = [](float length_2d)
	{
	auto tmp = RAD2DEG(atan(30.f / length_2d));

	if (CheckIfNonValidNumber(tmp) || tmp > 90.f)
	return 90.f;

	else if (tmp < 0.f)
	return 0.f;

	else
	return tmp;
	};

	//if (pLocal->GetMoveType() != MOVETYPE_WALK)
	//return;

	auto velocity = G::LocalPlayer->GetVelocity();
	velocity.z = 0;

	static auto flip = false;
	auto turn_direction_modifier = (flip) ? 1.f : -1.f;
	flip = !flip;

	if (G::LocalPlayer->GetFlags() & FL_ONGROUND || G::LocalPlayer->GetMoveType() == MOVETYPE_LADDER)
		return;

	if (G::UserCmd->forwardmove > 0.f)
		G::UserCmd->forwardmove = 0.f;

	auto velocity_length_2d = velocity.Length2D();

	auto strafe_angle = RAD2DEG(atan(15.f / velocity_length_2d));

	if (strafe_angle > 90.f)
	strafe_angle = 90.f;

	else if (strafe_angle < 0.f)
	strafe_angle = 0.f;

	Vector Buffer(0, G::StrafeAngle.y - old_yaw, 0);
	Buffer.y = Math::NormalizeYaw(Buffer.y);

	int yaw_delta = Buffer.y;
	old_yaw = G::StrafeAngle.y;

	if (yaw_delta > 0.f)
		G::UserCmd->sidemove = -450.f;

	else if (yaw_delta < 0.f)
		G::UserCmd->sidemove = 450.f;

	auto abs_yaw_delta = abs(yaw_delta);

	if (abs_yaw_delta <= strafe_angle || abs_yaw_delta >= 30.f)
	{
	Vector velocity_angles;
	Math::VectorAngles(velocity, velocity_angles);

	Buffer = Vector(0, G::StrafeAngle.y - velocity_angles.y, 0);
	Buffer.y = Math::NormalizeYaw(Buffer.y);
	int velocityangle_yawdelta = Buffer.y;

	auto velocity_degree = get_velocity_degree(velocity_length_2d) * MiscElements.Misc_Movement_Strafe_Retrack->value; // retrack value, for teleporters

	if (velocityangle_yawdelta <= velocity_degree || velocity_length_2d <= 15.f)
	{
	if (-(velocity_degree) <= velocityangle_yawdelta || velocity_length_2d <= 15.f)
	{
		G::StrafeAngle.y += (strafe_angle * turn_direction_modifier);
	G::UserCmd->sidemove = 450.f * turn_direction_modifier;
	}

	else
	{
		G::StrafeAngle.y = velocity_angles.y - velocity_degree;
		G::UserCmd->sidemove = 450.f;
	}
	}

	else
	{
		G::StrafeAngle.y = velocity_angles.y + velocity_degree;
		G::UserCmd->sidemove = -450.f;
	}


	}




	G::UserCmd->buttons &= ~(IN_MOVELEFT | IN_MOVERIGHT | IN_FORWARD | IN_BACK);

	if (G::UserCmd->sidemove <= 0.0)
		G::UserCmd->buttons |= IN_MOVELEFT;
	else
		G::UserCmd->buttons |= IN_MOVERIGHT;

	if (G::UserCmd->forwardmove <= 0.0)
		G::UserCmd->buttons |= IN_BACK;
	else
		G::UserCmd->buttons |= IN_FORWARD;

/*SKEET & CSTRAFE*/
	/*auto get_angle_from_speed = [](float speed)
	{
		auto ideal_angle = RAD2DEG(std::atan2(30.f, speed));
		ideal_angle = clamp<float>(ideal_angle, 0.f, 90.f);
		return ideal_angle;
	};

	auto get_velocity_step = [](Vector velocity, float speed, float circle_yaw)
	{
		auto velocity_degree = RAD2DEG(std::atan2(velocity.x, velocity.y));
		auto step = 1.5f;

		Vector start = G::LocalPlayer->GetOrigin(), end = G::LocalPlayer->GetOrigin();

		Ray_t ray;
		CGameTrace trace;
		CTraceFilter filter;

		while (true)
		{
			end.x += (std::cos(DEG2RAD(velocity_degree + circle_yaw)) * speed);
			end.y += (std::sin(DEG2RAD(velocity_degree + circle_yaw)) * speed);
			end *= g_pGlobals->frametime;

			ray.Init(start, end, Vector(-20.f, -20.f, 0.f), Vector(20.f, 20.f, 32.f));
			g_pEngineTrace->TraceRay(ray, CONTENTS_SOLID, &filter, &trace);

			if (trace.fraction < 1.f || trace.allsolid || trace.startsolid)
				break;

			step -= g_pGlobals->frametime;

			if (step == 0.f)
				break;

			start = end;
			velocity_degree += (velocity_degree + circle_yaw);
		}

		return step;
	};

	auto set_button_state = [](CUserCmd* cmd)
	{
		cmd->buttons &= ~(IN_MOVELEFT | IN_MOVERIGHT | IN_FORWARD | IN_BACK);

		if (cmd->sidemove <= 0.0)
			cmd->buttons |= IN_MOVELEFT;
		else
			cmd->buttons |= IN_MOVERIGHT;

		if (cmd->forwardmove <= 0.0)
			cmd->buttons |= IN_BACK;
		else
			cmd->buttons |= IN_FORWARD;
	};

	//if ( !menu->main.misc.air_strafer.checked( ) || opulence->me->m_MoveType( ) != MOVETYPE_WALK || opulence->me->m_fFlags( ) & FL_ONGROUND )
	//return;
	if (G::LocalPlayer->GetFlags() & FL_ONGROUND || G::LocalPlayer->GetMoveType() == MOVETYPE_LADDER)
		return;

	auto velocity = G::LocalPlayer->GetVelocity();
	velocity.z = 0;

	static bool flip = false;
	auto turn_direction_modifier = flip ? 1.f : -1.f;
	flip = !flip;

	if (G::UserCmd->forwardmove > 0.f)
		G::UserCmd->forwardmove = 0.f;

	auto speed = velocity.Length2D();

	static float m_circle_yaw;
	static float m_previous_yaw;
	// circle strafe
	/*if (MiscElements.Misc_Movement_Circlestrafe->Checked && GetAsyncKeyState(MiscElements.Misc_Movement_Circlestrafe_Key->Key))
	{
		auto ideal_speed_angle = get_angle_from_speed(speed);

		m_circle_yaw = Math::NormalizeYaw(m_circle_yaw + ideal_speed_angle);
		auto step = get_velocity_step(velocity, speed, ideal_speed_angle);

		if (step != 0.f)
			m_circle_yaw += (((g_pGlobals->frametime * 128.f) * step) * step);

		G::StrafeAngle.y = Math::NormalizeYaw(m_circle_yaw);
		G::UserCmd->sidemove = -450.f;
		set_button_state(G::UserCmd);
		return;
	}*/
/*

	auto ideal_move_angle = RAD2DEG(std::atan2(15.f, speed));
	ideal_move_angle = clamp<float>(ideal_move_angle, 0.f, 90.f);

	auto yaw_delta = Math::NormalizeYaw(G::StrafeAngle.y - m_previous_yaw);
	auto abs_yaw_delta = abs(yaw_delta);
	m_circle_yaw = m_previous_yaw = G::StrafeAngle.y;

	if (yaw_delta > 0.f)
		G::UserCmd->sidemove = -450.f;

	else if (yaw_delta < 0.f)
		G::UserCmd->sidemove = 450.f;

	if (abs_yaw_delta <= ideal_move_angle || abs_yaw_delta >= 30.f)
	{
		Vector velocity_angles;
		Math::VectorAngles(velocity, velocity_angles);

		auto velocity_angle_yaw_delta = Math::NormalizeYaw(G::StrafeAngle.y - velocity_angles.y);
		auto velocity_degree = get_angle_from_speed(speed) * MiscElements.Misc_Movement_Strafe_Retrack->value;

		if (velocity_angle_yaw_delta <= velocity_degree || speed <= 15.f)
		{
			if (-(velocity_degree) <= velocity_angle_yaw_delta || speed <= 15.f)
			{
				G::StrafeAngle.y += (ideal_move_angle * turn_direction_modifier);
				G::UserCmd->sidemove = 450.f * turn_direction_modifier;
			}

			else
			{
				G::StrafeAngle.y = velocity_angles.y - velocity_degree;
				G::UserCmd->sidemove = 450.f;
			}
		}

		else
		{
			G::StrafeAngle.y = velocity_angles.y + velocity_degree;
			G::UserCmd->sidemove = -450.f;
		}
	}

	set_button_state(G::UserCmd);

	*/
}

void CMisc::CStrafe_Manual()
{
	

	static bool JustStarted = true;
	static bool CircleDone = true;


	static float RightMovement;
	static bool IsActive = false;
	static float StrafeAngle = 0;

	static bool OldBool;
	bool CircleStrafe = (MiscElements.Misc_Movement_Circlestrafe->Checked && GetAsyncKeyState(MiscElements.Misc_Movement_Circlestrafe_Key->Key) & 0x8000) || !CircleDone;

	if (OldBool != CircleStrafe)
		JustStarted = true;

	if (!(G::UserCmd->buttons & IN_JUMP)) 
	{
		CircleStrafe = false;
		CircleDone = true;
		JustStarted = true;
		return;
	}

	OldBool = CircleStrafe;





	auto GetTraceFraction =[](Vector start, Vector end)
	{
		Ray_t ray;
		trace_t tr;
		CTraceFilter filter;
		filter.pSkip = G::LocalPlayer;

		ray.Init(start, end);
		g_pEngineTrace->TraceRay(ray, 0x4600400B, &filter, &tr);

		return tr.fraction;
	};

	auto AngleVectors = [](const Vector &angles, Vector *forward, Vector *right, Vector *up)
	{
		auto SinCos = [](float flRadians, float* pflSine, float* pflCosine)
		{
			__asm
			{
				fld	DWORD PTR[flRadians]
				fsincos
				mov edx, DWORD PTR[pflCosine]
				mov eax, DWORD PTR[pflSine]
				fstp DWORD PTR[edx]
				fstp DWORD PTR[eax]
			}
		};

		float sr, sp, sy, cr, cp, cy;

		SinCos(DEG2RAD(angles[1]), &sy, &cy);
		SinCos(DEG2RAD(angles[0]), &sp, &cp);
		SinCos(DEG2RAD(angles[2]), &sr, &cr);

		if (forward)
		{
			forward->x = cp*cy;
			forward->y = cp*sy;
			forward->z = -sp;
		}

		if (right)
		{
			right->x = (-1 * sr*sp*cy + -1 * cr*-sy);
			right->y = (-1 * sr*sp*sy + -1 * cr*cy);
			right->z = -1 * sr*cp;
		}

		if (up)
		{
			up->x = (cr*sp*cy + -sr*-sy);
			up->y = (cr*sp*sy + -sr*cy);
			up->z = cr*cp;
		}
	};

	if (!(G::LocalPlayer->GetFlags() & FL_ONGROUND) || (G::LocalPlayer->GetFlags() & FL_INWATER) && G::UserCmd->buttons & IN_JUMP)
	{
		printf("Working\n");

		static float angle = 0.f;
		static int inv = -1;

		CircleDone = false;

		if (JustStarted)
		{
			JustStarted = false;
			angle = G::UserCmd->viewangles.y;
		}

		Ray_t left_trace;
		Ray_t right_trace;
		Ray_t normal_trace;
		CTraceFilter Filter;
		Filter.pSkip = G::LocalPlayer;
		Vector EyePos = G::LocalPlayer->GetAbsAngles() + G::LocalPlayer->GetOrigin();

		Vector vleft = Vector(10.f, angle - 50.f, 0.f), vright = Vector(10.f, angle + 50.f, 0.f), vnormal = Vector(10.f, angle, 0.f), left, right, normal;
		AngleVectors(vright, &right, 0, 0);
		AngleVectors(vleft, &left, 0, 0);
		AngleVectors(vnormal, &normal, 0, 0);

		left_trace.Init(EyePos, EyePos + left * 50.f);
		right_trace.Init(EyePos, EyePos + right * 50.f);
		normal_trace.Init(EyePos, EyePos + normal * 50.f);

		trace_t result_left, result_right, result;

		g_pEngineTrace->TraceRay(left_trace, MASK_SOLID, &Filter, &result_left);
		g_pEngineTrace->TraceRay(right_trace, MASK_SOLID, &Filter, &result_right);
		g_pEngineTrace->TraceRay(normal_trace, MASK_SOLID, &Filter, &result);
		static int amount;
		amount = 10 + G::LocalPlayer->GetVelocity().Length2D() / 50;
		if ((result.fraction != 1.f && !result.m_pEnt) || (result_right.fraction != 1.f && !result_right.m_pEnt))
		{
			inv = -1;
			angle -= 30.f;
			G::UserCmd->sidemove = -450.f;
		}
		else if (result_left.fraction != 1.f && !result_left.m_pEnt)
		{
			inv = 1;
			angle += 30.f;
			G::UserCmd->sidemove = 450.f;
		}
		else
		{
			float flAdd = (200 / G::LocalPlayer->GetVelocity().Length2D()) * amount;
			if (flAdd > 5.f) flAdd = 5.f;
			angle += flAdd * (float)inv;
			G::UserCmd->sidemove = 450.f * (float)inv;
		}



		Vector noob(0.f, angle - G::UserCmd->viewangles.y, 0.f);
		noob.y = Math::NormalizeYaw(noob.y);

		if (noob.y > 170.f && noob.y < 180.f)
			CircleDone = true;

		G::UserCmd->viewangles.y = angle;
	}
}
void CMisc::CStrafe_New()
{
	auto get_angle_from_speed = [](float speed)
	{
		auto ideal_angle = RAD2DEG(std::atan2(30.f, speed));
		ideal_angle = clamp<float>(ideal_angle, 0.f, 90.f);
		return ideal_angle;
	};

	auto get_velocity_step = [](Vector velocity, float speed, float circle_yaw)
	{
		auto velocity_degree = RAD2DEG(std::atan2(velocity.x, velocity.y));
		auto step = 1.5f;

		Vector start = G::LocalPlayer->GetOrigin(), end = G::LocalPlayer->GetOrigin();

		Ray_t ray;
		CGameTrace trace;
		CTraceFilterWorldAndPropsOnly filter;

		while (true)
		{
			end.x += (std::cos(DEG2RAD(velocity_degree + circle_yaw)) * speed);
			end.y += (std::sin(DEG2RAD(velocity_degree + circle_yaw)) * speed);
			end *= g_pGlobals->frametime;

			ray.Init(start, end, Vector(-20.f, -20.f, 0.f), Vector(20.f, 20.f, 32.f));
			g_pEngineTrace->TraceRay_NEW(ray, CONTENTS_SOLID, &filter, &trace);

			if (trace.fraction < 1.f || trace.allsolid || trace.startsolid)
				break;

			step -= g_pGlobals->frametime;

			if (step == 0.f)
				break;

			start = end;
			velocity_degree += (velocity_degree + circle_yaw);
		}

		return step;
	};

	auto set_button_state = [](CUserCmd* cmd)
	{
		cmd->buttons &= ~(IN_MOVELEFT | IN_MOVERIGHT | IN_FORWARD | IN_BACK);

		if (cmd->sidemove <= 0.0)
			cmd->buttons |= IN_MOVELEFT;
		else
			cmd->buttons |= IN_MOVERIGHT;

		if (cmd->forwardmove <= 0.0)
			cmd->buttons |= IN_BACK;
		else
			cmd->buttons |= IN_FORWARD;
	};

	auto GetTraceFractionWorldProps = [](Vector start, Vector end)
	{
		Ray_t ray;
		trace_t tr;
		CTraceFilterWorldAndPropsOnly filter;

		ray.Init(start, end, G::LocalPlayer->GetCollision()->VecMins(), G::LocalPlayer->GetCollision()->VecMaxs());
		g_pEngineTrace->TraceRay_NEW(ray, MASK_SOLID, &filter, &tr);

		return tr.fraction;
	};

	auto GetLengthBetween = [](Vector start, Vector end)
	{
		Ray_t ray;
		trace_t tr;
		CTraceFilterWorldAndPropsOnly filter;

		ray.Init(start, end, G::LocalPlayer->GetCollision()->VecMins(), G::LocalPlayer->GetCollision()->VecMaxs());
		g_pEngineTrace->TraceRay_NEW(ray, MASK_SOLID, &filter, &tr);

		Vector between = tr.endpos - tr.startpos;

		return between.Length2D();
	};

	if (G::LocalPlayer->GetFlags() & FL_ONGROUND)
		return;

	Vector velocity = G::LocalPlayer->GetVelocity();

	auto speed = velocity.Length2D();

	auto speed_per_tick = G::LocalPlayer->GetVelocity().Normalized().Length2D();
	
	static float m_previous_yaw;


	static bool in_circle = false;

	if (GetAsyncKeyState(MiscElements.Misc_Movement_Circlestrafe_Key->Key))
		in_circle = true;

	//normal old aw
	/*if (GetAsyncKeyState(MiscElements.Misc_Movement_Circlestrafe_Key->Key))
	{
		if (MiscElements.Misc_Movement_Circlestrafe->Checked)
		{
			if (m_circle_yaw == 1000)
				m_circle_yaw = G::UserCmd->viewangles.y;

			auto ideal_speed_angle = get_angle_from_speed(speed);

			m_circle_yaw = Math::NormalizeYaw(m_circle_yaw + ideal_speed_angle);
			auto step = get_velocity_step(velocity, speed, ideal_speed_angle);

			if (step != 0.f)
				m_circle_yaw += (((g_pGlobals->frametime * 128.f) * step) * step);

			G::StrafeAngle.y = Math::NormalizeYaw(m_circle_yaw);
			if (step == 0)
			{
				G::UserCmd->forwardmove = 450;
				G::UserCmd->sidemove = 0;
			}
			else
				G::UserCmd->sidemove = -450.f;

			set_button_state(G::UserCmd);
		}
	}*/
	// circle strafe

	
	
		
		if (MiscElements.Misc_Movement_Circlestrafe->Checked && in_circle)
		{
			QAngle CurrentAngles; g_pEngine->GetViewAngles(CurrentAngles);
			if (m_circle_yaw == 1000)
			{
				
				m_circle_yaw = CurrentAngles.y;
			}

			auto ideal_speed_angle = get_angle_from_speed(speed);

			Vector circle_angle = Vector(0, m_circle_yaw + ideal_speed_angle);

			Vector Forward = circle_angle.Direction();

			Vector Right = Forward.Cross(Vector(0, 0, 1));

			Vector Left = Vector(Right.x * -1.0f, Right.y * -1.0f, Right.z);

			float CenterPath = GetTraceFractionWorldProps(G::LocalPlayer->GetOrigin(), G::LocalPlayer->GetOrigin() + Forward * speed_per_tick);

		
				
				if(circle_angle.y / CenterPath < 70)//if (CenterPath > 0)
				{
					m_circle_yaw = m_circle_yaw + ideal_speed_angle/*/CenterPath*/;

					G::UserCmd->sidemove = -450.f;


				
				}
				else
				{
					circle_angle = Vector(0, m_circle_yaw);

					Forward = circle_angle.Direction();

					Right = Forward.Cross(Vector(0, 0, 1));

					Left = Vector(Right.x * -1.0f, Right.y * -1.0f, Right.z);

					CenterPath = GetTraceFractionWorldProps(G::LocalPlayer->GetOrigin(), G::LocalPlayer->GetOrigin() + Forward * speed_per_tick);

				
					if (circle_angle.y / CenterPath < 70)//if (CenterPath > 0)
					{
						//m_circle_yaw = m_circle_yaw + ideal_speed_angle/CenterPath;

						G::UserCmd->sidemove = -450.f;
					}
					else
					{
						for (int i = 0; i < 180; i += 10)
						{
							Vector circle_angle_ = Vector(0, m_circle_yaw + i);

							Vector on_course = circle_angle_.Direction();

							float on_course_path = GetTraceFractionWorldProps(G::LocalPlayer->GetOrigin(), G::LocalPlayer->GetOrigin() + on_course * speed_per_tick);

							//if ((ideal_speed_angle / on_course_path) <= 15)
							if (on_course_path >= 1.f)
							{
								//go straight forward

								m_circle_yaw = circle_angle_.y;

								//G::UserCmd->sidemove = 450.f;

								G::StrafeAngle.y = Math::NormalizeYaw(m_circle_yaw);

								set_button_state(G::UserCmd);
								if (fabs(m_circle_yaw) >= 360)
								{
									in_circle = false;
									m_circle_yaw = 1000;

								}
								return;
							}
							/*else
							{
								m_circle_yaw = m_circle_yaw - ideal_speed_angle;
								G::UserCmd->forwardmove = 450;
								//G::UserCmd->sidemove = 450.f;
							}*/
						}
					}
				}
				if (fabs(m_circle_yaw) >= 360)
				{
					in_circle = false;
					m_circle_yaw = 1000;
					return;
				}

			
			
				G::StrafeAngle.y = Math::NormalizeYaw(m_circle_yaw);
			
			

			set_button_state(G::UserCmd);
		}
		else
		{
			m_circle_yaw = 1000;
		}
}
void CMisc::FixCmd()
{
	if (MiscElements.Misc_General_Restriction->SelectedIndex > 0)
	{

		G::UserCmd->viewangles.y = Math::NormalizeYaw(G::UserCmd->viewangles.y);
		Math::ClampAngles(G::UserCmd->viewangles);

		if (G::UserCmd->forwardmove > 450)
			G::UserCmd->forwardmove = 450;
		if (G::UserCmd->forwardmove < -450)
			G::UserCmd->forwardmove = -450;

		if (G::UserCmd->sidemove > 450)
			G::UserCmd->sidemove = 450;
		if (G::UserCmd->sidemove < -450)
			G::UserCmd->sidemove = -450;
	}
}
bool CMisc::FakeLag()
{
	if (G::LocalPlayer->GetFlags() & FL_ONGROUND)
	{
		if (!MiscElements.Misc_Movement_Fakelag_OnGround->Checked)
		{
			G::SendPacket = true;
			return false;
		}
	}
	
	//hardcore
	int Type = MiscElements.Misc_Movement_FakelagType->SelectedIndex;

	static int ticks = 0;
	const int ticksMax = 14; //16

	static bool new_factor = false;

	static int packetsToChoke = 0;

	if (Type == 1)
	{
		packetsToChoke = MiscElements.Misc_Movement_Fakelag_Choke->value;
	}
	else if (Type == 2)
	{
		float z_velocity = G::LocalPlayer->GetVelocity().z * 3; //5.5 is the peak
		if (z_velocity < 0)
			z_velocity *= -1;

		

		packetsToChoke = z_velocity;
		if (packetsToChoke == 0)
			packetsToChoke += 3;
	}
	else if (Type == 3)
	{
		packetsToChoke = max(1, min((int)(fabs(G::LocalPlayer->GetVelocity().Length() / 80.f)), 5));
	}
	else if (Type == 4 && new_factor)
	{
		packetsToChoke = (rand() % 8) + 6;
		new_factor = false;
	}
	else if (Type == 5 && new_factor)
	{
		static int FakelagFactor = 10;
		static int m_iOldHealth = 0;
		static int iChoked;
		if (m_iOldHealth != G::LocalPlayer->GetHealth())
		{
			m_iOldHealth = G::LocalPlayer->GetHealth();
			packetsToChoke = rand() % 7 + 6 + 1;
			new_factor = false;
		}
	}
	else if (Type == 6 && new_factor)
	{
		int min_packets = 0;
		Vector vel_p_t = Vector(G::LocalPlayer->GetVelocity() * g_pGlobals->interval_per_tick);
		for (int i = 1; i <= 14; i++)
		{
			if (Vector(vel_p_t * i).Length() > 64)
			{
				min_packets = i;
				break;
			}
		}

		packetsToChoke = int(Math::RandomFloat(min_packets, ticksMax));
	}

	if (packetsToChoke > ticksMax)
		packetsToChoke = ticksMax;

	if (ticks >= packetsToChoke)
	{
		ticks = 0;
		G::SendPacket = true;
		new_factor = true;
	}
	else
	{
		G::SendPacket = false;
	}


	ticks++;

	return false;
}
void CMisc::FixMovement()
{
	Vector vMove =Vector(G::UserCmd->forwardmove, G::UserCmd->sidemove, 0.0f);
	float flSpeed = vMove.Length();
	Vector qMove;
	Math::VectorAngles(vMove, qMove);
	float normalized = fmod(G::UserCmd->viewangles.y + 180.f, 360.f) - 180.f;
	float normalizedx = fmod(G::UserCmd->viewangles.x + 180.f, 360.f) - 180.f;
	float flYaw = DEG2RAD((normalized - G::StrafeAngle.y) + qMove.y);

	if (normalizedx >= 90.0f || normalizedx <= -90.0f || (G::UserCmd->viewangles.x >= 90.f && G::UserCmd->viewangles.x <= 200) || G::UserCmd->viewangles.x <= -90)
		G::UserCmd->forwardmove = -cos(flYaw) * flSpeed;
	else 
		G::UserCmd->forwardmove = cos(flYaw) * flSpeed;

	G::UserCmd->sidemove = sin(flYaw) * flSpeed;
}
typedef void(__fastcall* ClanTagFn)(const char*, const char*);
ClanTagFn dw_ClanTag;
void SetClanTag(const char* tag, const char* name)
{
	//static auto pSetClanTag = reinterpret_cast<void(__fastcall*)(const char*, const char*)>(Utilities::Memory::FindPattern("engine.dll", (PBYTE)"\x53\x56\x57\x8B\xDA\x8B\xF9\xFF\x15\x00\x00\x00\x00\x6A\x24\x8B\xC8\x8B\x30", "xxxxxxxxx????xxxxxx")); // 0x9AF10
	if (!dw_ClanTag)
		dw_ClanTag = reinterpret_cast<ClanTagFn>(FindPatternIDA("engine.dll", "53 56 57 8B DA 8B F9 FF 15"));
	
	if(dw_ClanTag)
		dw_ClanTag(tag, name);
}
auto clock_begin = std::chrono::high_resolution_clock::now();
float TickCounter = 0;
void SetScrollClanTag(std::string Tag)
{
	//int TagLength = Tag.length();
	//std::string Whitespace;
	//for (int i = 0; i < TagLength; i++)
	//	Whitespace += " ";
	//std::string WhitespaceTag = Whitespace + Tag + Whitespace;
	//if (TickCounter / 1000.0f > TagLength * 2) 
		//clock_begin = std::chrono::high_resolution_clock::now();
	//std::string CurrentSubStr = WhitespaceTag.substr((int)(TickCounter / 1000.0f), TagLength);

	//SetClanTag(CurrentSubStr.c_str(), CurrentSubStr.c_str());
	SetClanTag(Tag.c_str(), Tag.c_str());
	//TickCounter = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - clock_begin).count();
}

void CMisc::HandleClantag()
{
	if (MiscElements.Misc_Chat_ClanTagChanger->SelectedIndex == 0)
		return;
	static int iLastTime;

	float latency = g_pEngine->GetNetChannelInfo()->GetLatency(FLOW_INCOMING) + g_pEngine->GetNetChannelInfo()->GetLatency(FLOW_OUTGOING);

	if (int(g_pGlobals->curtime*2 + latency) != iLastTime)
	{
		std::string Name = "getcoe.us";
		SetScrollClanTag(Name.substr(0, int(g_pGlobals->curtime*2 + latency) % (Name.length()+1)));
		iLastTime = int(g_pGlobals->curtime*2 + latency);
	}
}


void LoadPathing()
{

	char path[MAX_PATH];
	GetModuleFileNameA(GetModuleHandle(NULL), path, 255);
	for (int i = strlen(path); i > 0; i--)
	{
		if (path[i] == '\\')
		{
			path[i + 1] = 0;
			break;
		}
	}

	char size[8];
	char vecstr[64];
	char itostr[8];
	CBaseEntity *pLocal = G::LocalPlayer;
	if (!pLocal)
		return;
	if (g_Misc->path.size() > 1)
	{

		g_Misc->path.erase(g_Misc->path.begin(), g_Misc->path.end());
		sprintf_s(path, "\\%s_%s_%d.cfg", "pathes", "test"/*g_pEngine->GetLevelName()*/, pLocal->GetTeamNum());
		GetPrivateProfileStringA("Points", "Size", "0", size, 8, path);
		int numPoints = atoi(size);
		for (int i = 0; i < numPoints - 1; i++)
		{
			char vecstr[64];
			char itostr[8];
			sprintf_s(itostr, "%d", i);
			GetPrivateProfileStringA("Pathing", itostr, "0.0 0.0 0.0", vecstr, 64, path);
			std::string PosStr = vecstr;
			string buffer;
			stringstream ss(PosStr);
			vector<string> floats;
			while (ss >> buffer)
				floats.push_back(buffer);

			g_Misc->path.push_back(Vector(stof(floats[0]), stof(floats[1]), stof(floats[2])));
		}
	}
}

void SavePathing()
{

	char path[MAX_PATH];
	GetModuleFileNameA(GetModuleHandle(NULL), path, 255);
	for (int i = strlen(path); i > 0; i--)
	{
		if (path[i] == '\\')
		{
			path[i + 1] = 0;
			break;
		}
	}
	char size[8];
	char vecstr[64];
	char itostr[8];

	if (g_Misc->path.size() > 1)
	{
		sprintf_s(path, "\\%s_%s_%d.cfg", "pathes", "test"/*g_pEngine->GetLevelName()*/, G::LocalPlayer->GetTeamNum());
		printf("Path %s\n", path);
		sprintf_s(size, "%d", g_Misc->path.size() + 1);
		WritePrivateProfileStringA("Points", "Size", size, path);
		for (int i = 0; i < g_Misc->path.size(); i++)
		{
			sprintf_s(itostr, "%d", i);
			sprintf_s(vecstr, "%f %f %f", g_Misc->path.at(i).x, g_Misc->path.at(i).y, g_Misc->path.at(i).z);
			WritePrivateProfileStringA("Pathing", itostr, vecstr, path);
		}
	}
}
bool MarksIsVisible(CBaseEntity* local, Vector& vTo) 
{
	Ray_t ray;
	trace_t trace;
	CTraceFilterNoPlayer filter;
	filter.pSkip = local;

	ray.Init(local->GetEyePosition(), vTo);
	g_pEngineTrace->TraceRay(ray, 0x4600400B, &filter, &trace);
	return (trace.fraction > 0.99f);
}
void CMisc::Walkbot()
{
	static QAngle lastangle;
	static int PathingID;
	auto AimStep = [](QAngle angSource, QAngle angDestination, QAngle &angOut)
	{
		QAngle angDelta = (angDestination - angSource); 
		angDelta.y = Math::NormalizeYaw(angDelta.y);
		int iStepAmount = 12.f; // above 15 gets me kicked.
		bool bXFinished = false;
		bool bYFinished = false;

		if (angDelta.x > iStepAmount)
			angSource.x += iStepAmount;
		else if (angDelta.x < -iStepAmount)
			angSource.x -= iStepAmount;
		else
		{
			bXFinished = true;
			angSource.x = angDestination.x;
		}

		if (angDelta.y > iStepAmount)
			angSource.y += iStepAmount;
		else if (angDelta.y < -iStepAmount)
			angSource.y -= iStepAmount;
		else
		{
			bYFinished = true;
			angSource.y = angDestination.y;
		}

		angSource.y = Math::NormalizeYaw(angSource.y);

		angOut = angSource;

		return bXFinished && bYFinished;
	};

	static bool enabled;
	if (m_pMenu->MainWindow->ToggleButton(2))
		enabled = !enabled;
	QAngle nextAngle;

	if (path.size() > 1)
	{
		if (!MarksIsVisible(G::LocalPlayer, path.at(PathingID)))
		{
			for (int i = 0; i < path.size(); i++)
			{
				float best = 9999.f;
				if (G::LocalPlayer->GetOrigin().Dist(path.at(i)) < best && MarksIsVisible(G::LocalPlayer, path.at(i)))
				{
					best = G::LocalPlayer->GetOrigin().Dist(path.at(i));
					PathingID = i;
				}
			}
		}
	}

	if (path.size() > 1 && enabled)
	{
		QAngle CurAngles;
		g_pEngine->GetViewAngles(CurAngles);
		QAngle newAngle, temp;
		temp = GameUtils::CalculateAngle(G::LocalPlayer->GetOrigin(), path.at(PathingID));
		temp.y = Math::NormalizeYaw(temp.y);
		//temp.x = CurAngles.x;

		AimStep(lastangle, temp, newAngle);

		newAngle.y = Math::NormalizeYaw(newAngle.y);
		G::StrafeAngle = newAngle;
		//g_pEngine->SetViewAngles(G::UserCmd->viewangles);
		lastangle = newAngle;
		G::UserCmd->buttons |= IN_JUMP;
		if (!(G::UserCmd->buttons & IN_DUCK))
			G::UserCmd->buttons |= IN_DUCK;

	}
	if (m_pMenu->MainWindow->ToggleButton(VK_F6))
	{

		path.push_back(G::LocalPlayer->GetOrigin());
	}

	if (m_pMenu->MainWindow->ToggleButton(VK_F7))
		LoadPathing();

	if (m_pMenu->MainWindow->ToggleButton(VK_F8))
	{
		printf("Called\n");
		SavePathing();
	}
	if (m_pMenu->MainWindow->ToggleButton(VK_DELETE))
	{
		path.clear();
		PathingID = 0;
	}

		if (path.size() > 1 && G::LocalPlayer->GetOrigin().Dist2D(path.at(PathingID)) < 64)
		{
			PathingID++;
			if (PathingID > path.size() - 1)
				PathingID = 0;
		}

}