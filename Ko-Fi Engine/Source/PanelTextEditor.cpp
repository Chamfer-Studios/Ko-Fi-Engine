#include "PanelTextEditor.h"
#include "Editor.h"
#include "Engine.h"
#include "FileSystem.h"
#include "PanelChooser.h"
#include "SceneManager.h"
#include "ComponentMaterial.h"
#include "Material.h"

#include "glew.h"
#include <gl/GL.h>
#include <gl/GLU.h>

PanelTextEditor::PanelTextEditor(Editor* editor)
{
	this->editor = editor;
	panelName = "Text Editor";
	//Editor configuration
	textEditor = TextEditor();
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
	CleanUp();
}

bool PanelTextEditor::CleanUp()
{
	editor = nullptr;
	//TODO: CHECK TEXT EDITOR CLEANUP -- MAMAMIA SCARY
	filePath.clear();
	filePath.shrink_to_fit();
	return true;
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
	if (editor->toggleTextEditor) RenderWindow(&editor->toggleTextEditor);
	
	
	return true;
}

bool PanelTextEditor::PostUpdate()
{
	return true;
}

void PanelTextEditor::RenderWindow(bool* toggleEditText)
{
	if (ImGui::Begin(panelName.c_str(), toggleEditText)) {
		
		ChooserListener();
		if (ImGui::Button("Open File")) {
			LoadFile();
		}
		ImGui::SameLine();
		if (ImGui::Button("Save File")) {
			SaveFile(filePath);
		}
		textEditor.Render("##EditorWindow");

	}
	ImGui::End();


}

void PanelTextEditor::LoadFile(std::string path,std::string ext)
{
	if (path.empty()) {
		editor->GetPanelChooser()->OpenPanel("TextEditor", "glsl", {"glsl"});
	}
	else {
		this->filePath = path;
		std::string text = editor->engine->GetFileSystem()->OpenFile(filePath.c_str());
		textEditor.SetText(text);
		if (ext == ".lua")
		{
			textEditor.SetLanguageDefinition(TextEditor::LanguageDefinition::Lua());
		}
		else
		{
			textEditor.SetLanguageDefinition(TextEditor::LanguageDefinition::GLSL());
		}


	}

}

void PanelTextEditor::SaveFile(std::string path)
{
	editor->engine->GetFileSystem()->SaveFile(path.c_str(), textEditor.GetText());
	for (GameObject* go : editor->engine->GetSceneManager()->GetCurrentScene()->gameObjectList)
	{
		if (go->GetComponent<ComponentMaterial>() != nullptr)
		{
			Material* material = go->GetComponent<ComponentMaterial>()->GetMaterial();
			if (material->shaderProgramID != 0)
				glDeleteProgram(material->shaderProgramID);

			Importer::GetInstance()->materialImporter->LoadAndCreateShader(material->GetShaderPath(), material);
		}
	}
}

void PanelTextEditor::Focus()
{
	ImGui::SetWindowFocus(panelName.c_str());
}

void PanelTextEditor::ChooserListener()
{
	if (editor->GetPanelChooser()->IsReadyToClose("TextEditor")) {
		if (editor->GetPanelChooser()->OnChooserClosed() != nullptr) {
			const char* path = editor->GetPanelChooser()->OnChooserClosed();
			this->filePath = path;
			std::string text = editor->engine->GetFileSystem()->OpenFile(path);
			textEditor.SetText(text);
		}
	}
}
