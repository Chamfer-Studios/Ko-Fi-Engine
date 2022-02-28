#include "Component.h"
#include "GameObject.h"

Component::Component(GameObject* parent) : owner(parent)
{
	if (parent)
	{
		parent->AddComponent(this);
	}
}