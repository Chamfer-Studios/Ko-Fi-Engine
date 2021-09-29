#include "PanelTest.h"
#include <imgui.h>
PanelTest::PanelTest()
{
}

PanelTest::~PanelTest()
{
}

bool PanelTest::Awake()
{
	return true;
}

bool PanelTest::PreUpdate()
{
	return true;
}

bool PanelTest::Update()
{
	ImGui::Begin("End the program");
	ImGui::Text("Press the button to end the program.");
	if (ImGui::Button("Button") == true) exit(0);
	if (ImGui::Button("Button2") == true) exit(0);
	ImGui::End();
	return true;
}

bool PanelTest::PostUpdate()
{
	return true;
}
