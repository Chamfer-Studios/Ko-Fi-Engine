#include "C_Animator.h"

#include "GameObject.h"

#include "ComponentMesh.h"

C_Animator::C_Animator(GameObject* parent) : Component(parent)
{
}

C_Animator::~C_Animator()
{
}

bool C_Animator::Start()
{
	return true;
}

bool C_Animator::Update(float dt)
{
	return true;
}

bool C_Animator::CleanUp()
{
	return true;
}
