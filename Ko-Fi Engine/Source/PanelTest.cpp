#include "PanelTest.h"
#include <imgui.h>
#include "EditorStyleHandler.h"

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
	// Window with a button to create another window
	ImGui::Begin("Create window");
	ImGui::Text("Press the button to create another window.");
	if (ImGui::Button("Button"))
		styleHandler.SetKoFiStyle();
	ImGui::End();

	return true;
}

bool PanelTest::PostUpdate()
{
	return true;
}
