#include "C_SoundTriggerCreator.h"

C_SoundTriggerCreator::C_SoundTriggerCreator(GameObject* gameObject) : Component(gameObject)
{
}

bool C_SoundTriggerCreator::Start()
{
	return true;
}

bool C_SoundTriggerCreator::Update(float dt)
{
	return true;
}

bool C_SoundTriggerCreator::CleanUp()
{
	return true;
}

bool C_SoundTriggerCreator::InspectorDraw(PanelChooser* chooser)
{
	if (ImGui::CollapsingHeader("Sound Trigger Creator", ImGuiTreeNodeFlags_AllowItemOverlap))
	{

	}

	return true;
}

void C_SoundTriggerCreator::Save(Json& json) const
{
}

void C_SoundTriggerCreator::Load(Json& json)
{
}
