#include "Component.h"
#include "GameObject.h"

Component::Component(GameObject* parent) : owner(parent)
{
	type = ComponentType::NONE;

	if (parent)
	{
		//PERA UN SEC PLS JEJE
		parent->AddComponent(this);
	}
}