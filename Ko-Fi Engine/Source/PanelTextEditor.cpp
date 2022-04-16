#include "PanelTextEditor.h"
// Modules
#include "Engine.h"
#include "M_Editor.h"
#include "M_FileSystem.h"
#include "M_SceneManager.h"

// GameObject
#include "GameObject.h"
#include "C_Material.h"

#include "R_Material.h"
#include "PanelChooser.h"
#include "glew.h"
#include <gl/GL.h>
#include <gl/GLU.h>

PanelTextEditor::PanelTextEditor(M_Editor* editor)
{
	this->editor = editor;
	panelName = "Text M_Editor";
	//M_Editor configuration
	textEditor.SetShowWhitespaces(false);
	textEditor.SetReadOnly(false);
	textEditor.SetPalette(TextEditor::GetDarkPalette());
	textEditor.SetLanguageDefinition(TextEditor::LanguageDefinition::GLSL());
}

PanelTextEditor::PanelTextEditor(M_Editor* editor,const char* path)
{
	this->editor = editor;
	panelName = "Text M_Editor";
	editor->engine->GetFileSystem()->OpenFile(path);
	//M_Editor configuration
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

bool PanelTextEditor::Update()
{
	OPTICK_EVENT();

	if (editor->toggleTextEditor) RenderWindow(&editor->toggleTextEditor);
	
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
		if (go->GetComponent<C_Material>() != nullptr)
		{
			R_Material* material = go->GetComponent<C_Material>()->GetMaterial();
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
