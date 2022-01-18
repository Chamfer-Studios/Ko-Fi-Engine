#include "PanelTextEditor.h"
#include "Editor.h"
#include "Engine.h"
#include "FileSystem.h"
#include "PanelChooser.h"
#include "SceneManager.h"
#include "ComponentMaterial.h"

PanelTextEditor::PanelTextEditor(Editor* editor)
{
	this->editor = editor;
	panelName = "Text Editor";
	//Editor configuration
	textEditor.SetShowWhitespaces(false);
	textEditor.SetReadOnly(false);
	textEditor.SetPalette(TextEditor::GetDarkPalette());
	textEditor.SetLanguageDefinition(TextEditor::LanguageDefinition::GLSL());
}

PanelTextEditor::PanelTextEditor(Editor* editor,const char* path)
{
	this->editor = editor;
	panelName = "Text Editor";
	editor->engine->GetFileSystem()->OpenFile(path);
	//Editor configuration
	textEditor.SetShowWhitespaces(false);
	textEditor.SetReadOnly(false);
	textEditor.SetPalette(TextEditor::GetDarkPalette());
	textEditor.SetLanguageDefinition(TextEditor::LanguageDefinition::GLSL());
}

PanelTextEditor::~PanelTextEditor()
{
}

bool PanelTextEditor::Awake()
{
	return true;
}

bool PanelTextEditor::PreUpdate()
{
	return true;
}

bool PanelTextEditor::Update()
{
	ImGui::Begin(panelName.c_str(), 0);
	
	if (editor->GetPanelChooser()->IsReadyToClose("TextEditor")) {
		if (editor->GetPanelChooser()->OnChooserClosed() != nullptr) {
			const char* path = editor->GetPanelChooser()->OnChooserClosed();
			
			std::string text = editor->engine->GetFileSystem()->OpenFile(path);
			textEditor.SetText(text);
		}
	}
	if (ImGui::Button("Open File")) {
		editor->GetPanelChooser()->OpenPanel("TextEditor","glsl");

		
	}
	ImGui::SameLine();
	if (ImGui::Button("Save File")) {
		editor->engine->GetFileSystem()->SaveFile("Assets/Shaders/default_shader.glsl",textEditor.GetText());
		for (GameObject* go : editor->engine->GetSceneManager()->GetCurrentScene()->gameObjectList) {
			if(go->GetComponent<ComponentMaterial>() != nullptr)
			go->GetComponent<ComponentMaterial>()->Compile();
		}
	}
	textEditor.Render("##EditorWindow");
	ImGui::End();
	
	return true;
}

bool PanelTextEditor::PostUpdate()
{
	return true;
}
