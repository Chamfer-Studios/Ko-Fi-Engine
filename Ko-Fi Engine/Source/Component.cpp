#include "Component.h"
#include "GameObject.h"

Component::Component(GameObject* parent) : owner(parent)
{
	if (parent != nullptr)
	{
		//PERA UN SEC PLS JEJE
		parent->AddComponent(this);
	}
}