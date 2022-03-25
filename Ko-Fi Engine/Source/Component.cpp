#include "Component.h"
#include "GameObject.h"

Component::Component(GameObject* parent) : owner(parent)
{
	type = ComponentType::NONE;

	if (parent)
	{
		parent->AddComponent(this);
	}
}