	#ifndef __SCRIPTING_H__
#define __SCRIPTING_H__

#include "Globals.h"
#include "Engine.h"
#include "M_Navigation.h"
#include "M_Renderer3D.h"
#include "M_Input.h" 
#include "M_SceneManager.h"
#include "M_Physics.h"
#include "SceneIntro.h"
#include "M_Camera3D.h"
#include "ImGuiAppLog.h"

#include <vector>

#include "Log.h"
#include "MathGeoLib/Math/float3.h"
#include "MathGeoLib/Math/float4.h"
#include "MathGeoLib/Math/Quat.h"
#include "GameObject.h"
#include "C_Transform.h"
#include "C_Mesh.h"
#include "C_Script.h"
#include "C_Text.h"
#include "C_Image.h"
#include "C_Button.h"
#include "C_Animator.h"
#include "C_Particle.h"
#include "C_Camera.h"
#include "C_AudioSource.h"
#include "C_AudioSwitch.h"
#include "C_Script.h"
#include "C_RigidBody.h"
#include "C_BoxCollider.h"

class Scripting
{
public:
	Scripting(C_Script* _script)
	{
		//script = _script;
	}

	~Scripting() {}

	void SetUpVariableTypes()
	{
		/*
		

		*/

	}

	/*bool CleanUp()
	{
		CONSOLE_LOG("Quitting scripting system");
		appLog->AddLog("Quitting scripting system\n");
		return true;
	}

	

	*/

public:
};

#endif // !__SCRIPTING_H__