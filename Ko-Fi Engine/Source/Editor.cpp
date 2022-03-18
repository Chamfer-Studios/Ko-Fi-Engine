#include <ImNodes.h>
#include <imgui_impl_sdl.h>
#include <imgui_impl_opengl3.h>
#include "Engine.h"
#include "Editor.h"
#include "Window.h"
#include "Renderer3D.h"
#include "FileSystem.h"
#include "Input.h"
#include "SceneIntro.h"
#include "EngineConfig.h"
#include "ImGuiAppLog.h"
#include "MainBar.h"
#include "PanelHierarchy.h"
#include "PanelConfiguration.h"
#include "PanelLog.h"
#include "PanelAbout.h"
#include "PanelChooser.h"
#include "PanelInspector.h"
#include "PanelViewport.h"
#include "PanelCameraViewport.h"
#include "PanelGame.h"
#include "PanelRuntimeState.h"
#include "PanelAssets.h"
#include "PanelTextEditor.h"
#include "PanelNodeEditor.h"
#include "ImGuizmo.h"

void LoadFontsEditor(float fontSize_ = 12.0f);

Editor::Editor(KoFiEngine* engine)
{
	this->engine = engine;
	name = "Editor";

	// We need to check whether or not each panel is activated.
	// We have to set a bool for each of them in order to be able to close and open again a panel.
	// Steps:
	// 1.- We have to check if the panel is activated.
	// 2.- If it is, we'll then check if an instance of the panel already exists.
	// 3.- If it doesn't exist, we'll create one. But if it exists we don't need to do anything (the editor already updates each panel).
	// 4.- If the bool is false and the panel exists, we will RELEASE the panel. (In the cases when we close a panel).
	// NOTES:
	// We can do it in the constructor for the initial values of the bool, but it is also essential to do it in the update.
	// In order to have it updated when we close or open a panel.
	// For organization, we'll create a struct to have all the bools stacked there.

	// We should do it here
	mainMenuBar = new MainBar(this);
	panelHierarchy = new PanelHierarchy(this);
	panelConfig = new PanelConfiguration(this, engine->GetEngineConfig());
	panelLog = new PanelLog();
	panelAbout = new PanelAbout(this);
	panelChooser = new PanelChooser(this);
	panelGameObject = new PanelInspector(this);
	panelAssets = new PanelAssets(this);
	//panelNodeEditor = new PanelNodeEditor(this);
	panelTextEditor = new PanelTextEditor(this);

	// Panel instances with its own bool
	/*if (panelsState.showGameWindow)
	{*/
		//panelGame = new PanelGame(this);
		//AddPanel(panelGame);
	/*}*/
	if (panelsState.showViewportWindow)
	{
		panelViewport = new PanelViewport(this, engine);
		AddPanel(panelViewport);
	}
	if (panelsState.showCameraViewportWindow)
	{
		panelCameraViewport = new PanelCameraViewport(this, engine);
		AddPanel(panelCameraViewport);
	}
	//------------------------------------
	
	// We want to have it always displayed.
	panelRuntimeState = new PanelRuntimeState(this, engine);
	AddPanel(panelRuntimeState);

	AddPanel(mainMenuBar);
	AddPanel(panelHierarchy);
	AddPanel(panelConfig);
	AddPanel(panelLog);
	AddPanel(panelAbout);
	AddPanel(panelChooser);
	AddPanel(panelGameObject);
	AddPanel(panelAssets);
	//AddPanel(panelNodeEditor);
	AddPanel(panelTextEditor);
}

Editor::~Editor()
{
	for (std::list<Panel*>::reverse_iterator item = panels.rbegin(); item != panels.rend(); ++item)
	{
		RELEASE(*item);
	}
	panels.clear();
}

void Editor::AddPanel(Panel* panel)
{
	panel->Init();
	panels.push_back(panel);
}

void Editor::RemovePanel(Panel* panel)
{
	panel->CleanUp();
	panels.remove(panel);
	RELEASE(panel);
}

PanelChooser* Editor::GetPanelChooser()
{
	return this->panelChooser;
}

bool Editor::Awake(Json configModule)
{
	bool ret = true;

	// Panels Awake
	if (ret == true)
	{
		std::list<Panel*>::iterator item = panels.begin();;

		while (item != panels.end() && ret)
		{
			ret = (*item)->Awake();
			item++;
		}
	}

	// FIXME: The list of meshes should be in scene intro.
	//input->gameObjects = &gameObjects;
	ImGuizmo::Enable(true);
	return ret;
}

bool Editor::Start()
{
	appLog->AddLog("Starting panel editor\n");
	bool ret = true;

	// Initializing ImGui
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImNodes::CreateContext();
	//ImGuizmo::SetImGuiContext(ImGui::GetCurrentContext());
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard | ImGuiConfigFlags_NavEnableSetMousePos | ImGuiConfigFlags_DockingEnable;

	styleHandler.SetKoFiStyle();
	ImGui_ImplSDL2_InitForOpenGL(engine->GetWindow()->window, engine->GetRenderer()->context);
	ImGui_ImplOpenGL3_Init();

	// Panels Start
	if (ret == true)
	{
		std::list<Panel*>::iterator item = panels.begin();;

		while (item != panels.end() && ret)
		{
			ret = (*item)->Start();
			item++;
		}
	}

	LoadFontsEditor(16);

	return ret;
}

bool Editor::PreUpdate(float dt)
{
	bool ret = true;
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplSDL2_NewFrame(engine->GetWindow()->window);
	ImGui::NewFrame();
	ImGuizmo::BeginFrame();

	// Panels PreUpdate
	if (ret == true)
	{
		std::list<Panel*>::iterator item = panels.begin();;

		while (item != panels.end() && ret)
		{
			ret = (*item)->PreUpdate();
			item++;
		}
	}

	return true;
}

bool Editor::Update(float dt)
{
	bool ret = true;

	ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoDocking;
	ImGuiViewport* viewport = ImGui::GetMainViewport();
	ImGui::SetNextWindowPos(viewport->WorkPos);
	ImGui::SetNextWindowSize(viewport->WorkSize);
	ImGui::SetNextWindowViewport(viewport->ID);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
	windowFlags |= ImGuiWindowFlags_NoBackground;
	windowFlags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
	windowFlags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
	if (ImGui::Begin("Dockspace", 0, windowFlags))
	{
		// DockSpace
		ImGuiIO& io = ImGui::GetIO();
		if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
		{
			ImGuiID dockspaceId = ImGui::GetID("DefaultDockspace");
			ImGui::DockSpace(dockspaceId,ImVec2(0.0f,0.0f),ImGuiDockNodeFlags_PassthruCentralNode);
			//ImGui::DockSpaceOverViewport(viewport);
		}

	}
	ImGui::End();
	ImGui::PopStyleVar();

	// Update panels state
	UpdatePanelsState();

	// Panels Update
	if (ret == true)
	{
		std::list<Panel*>::iterator item = panels.begin();;

		while (item != panels.end() && ret)
		{
			ret = (*item)->Update();
			item++;
		}
	}

	return ret;
}

bool Editor::PostUpdate(float dt)
{
	bool ret = true;

	ImGuiIO& io = ImGui::GetIO(); (void)io;

	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	SDL_GL_MakeCurrent(engine->GetWindow()->window, engine->GetRenderer()->context);

	// Panels PostUpdate
	if (ret == true)
	{
		std::list<Panel*>::iterator item = panels.begin();;

		while (item != panels.end() && ret)
		{
			ret = (*item)->PostUpdate();
			item++;
		}
	}

	// Update and Render additional Platform Windows
	// (Platform functions may change the current OpenGL context, so we save/restore it to make it easier to paste this code elsewhere.
	// For this specific demo app we could also call SDL_GL_MakeCurrent(window, gl_context) directly)
	if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		SDL_Window* backup_current_window = SDL_GL_GetCurrentWindow();
		SDL_GLContext backup_current_context = SDL_GL_GetCurrentContext();
		ImGui::UpdatePlatformWindows();
		ImGui::RenderPlatformWindowsDefault();
		SDL_GL_MakeCurrent(backup_current_window, backup_current_context);
	}

	ImGui::EndFrame();

	return ret;
}

bool Editor::CleanUp()
{
	appLog->AddLog("Cleaning panel editor\n");
	bool ret = true;

	// Cleaning panels
	for (std::list<Panel*>::reverse_iterator item = panels.rbegin(); item != panels.rend() && ret == true; ++item)
	{
		ret = (*item)->CleanUp();
	}

	RELEASE(mainMenuBar);
	RELEASE(panelHierarchy);
	RELEASE(panelConfig);
	RELEASE(panelLog);
	RELEASE(panelAbout);
	RELEASE(panelChooser);
	RELEASE(panelGameObject);
	RELEASE(panelViewport);
	//RELEASE(panelGame);
	RELEASE(panelRuntimeState);

	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplSDL2_Shutdown();
	ImNodes::DestroyContext();
	ImGui::DestroyContext();

	return true;
}

// Method to receive and manage events
void Editor::OnNotify(const Event& event)
{
	// Manage events
}

#include "ImGui.h"                // https://github.com/ocornut/imgui
#include "imgui_markdown.h"       // https://github.com/juliettef/imgui_markdown
#include "IconsFontAwesome5.h"    // https://github.com/juliettef/IconFontCppHeaders

// Following includes for Windows LinkCallback
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include "Shellapi.h"
#include <string>

void LinkCallback(ImGui::MarkdownLinkCallbackData data_);
inline ImGui::MarkdownImageData ImageCallback(ImGui::MarkdownLinkCallbackData data_);

static ImFont* H1 = NULL;
static ImFont* H2 = NULL;
static ImFont* H3 = NULL;

static ImGui::MarkdownConfig mdConfig;

void LinkCallback(ImGui::MarkdownLinkCallbackData data_)
{
	std::string url(data_.link, data_.linkLength);
	if (!data_.isImage)
	{
		ShellExecuteA(NULL, "open", url.c_str(), NULL, NULL, SW_SHOWNORMAL);
	}
}

inline ImGui::MarkdownImageData ImageCallback(ImGui::MarkdownLinkCallbackData data_)
{
	// In your application you would load an image based on data_ input. Here we just use the imgui font texture.
	ImTextureID image = ImGui::GetIO().Fonts->TexID;
	// > C++14 can use ImGui::MarkdownImageData imageData{ true, false, image, ImVec2( 40.0f, 20.0f ) };
	ImGui::MarkdownImageData imageData;
	imageData.isValid = true;
	imageData.useLinkCallback = false;
	imageData.user_texture_id = image;
	imageData.size = ImVec2(40.0f, 20.0f);

	// For image resize when available size.x > image width, add
	ImVec2 const contentSize = ImGui::GetContentRegionAvail();
	if (imageData.size.x > contentSize.x)
	{
		float const ratio = imageData.size.y / imageData.size.x;
		imageData.size.x = contentSize.x;
		imageData.size.y = contentSize.x * ratio;
	}

	return imageData;
}

void LoadFontsEditor(float fontSize_/* = 12.0f*/)
{
	ImGuiIO& io = ImGui::GetIO();
	io.Fonts->Clear();
	// Base font
	io.Fonts->AddFontFromFileTTF("Assets/Fonts/Lato/Lato-Regular.ttf", fontSize_);
	// Bold headings H2 and H3
	H2 = io.Fonts->AddFontFromFileTTF("Assets/Fonts/Lato/Lato-Bold.ttf", fontSize_);
	H3 = mdConfig.headingFormats[1].font;
	// bold heading H1
	float fontSizeH1 = fontSize_ * 1.1f;
	H1 = io.Fonts->AddFontFromFileTTF("Assets/Fonts/Lato/Lato-Bold.ttf", fontSizeH1);
}

void ExampleMarkdownFormatCallback(const ImGui::MarkdownFormatInfo& markdownFormatInfo_, bool start_)
{
	// Call the default first so any settings can be overwritten by our implementation.
	// Alternatively could be called or not called in a switch statement on a case by case basis.
	// See defaultMarkdownFormatCallback definition for furhter examples of how to use it.
	ImGui::defaultMarkdownFormatCallback(markdownFormatInfo_, start_);

	switch (markdownFormatInfo_.type)
	{
		// example: change the colour of heading level 2
	case ImGui::MarkdownFormatType::HEADING:
	{
		if (markdownFormatInfo_.level == 2)
		{
			if (start_)
			{
				ImGui::PushStyleColor(ImGuiCol_Text, ImGui::GetStyle().Colors[ImGuiCol_TextDisabled]);
			}
			else
			{
				ImGui::PopStyleColor();
			}
		}
		break;
	}
	default:
	{
		break;
	}
	}
}

void Editor::Markdown(const std::string& markdown_)
{
	// You can make your own Markdown function with your prefered string container and markdown config.
	// > C++14 can use ImGui::MarkdownConfig mdConfig{ LinkCallback, NULL, ImageCallback, ICON_FA_LINK, { { H1, true }, { H2, true }, { H3, false } }, NULL };
	mdConfig.linkCallback = LinkCallback;
	mdConfig.tooltipCallback = NULL;
	mdConfig.imageCallback = ImageCallback;
	mdConfig.linkIcon = ICON_FA_LINK;
	mdConfig.headingFormats[0] = { H1, true };
	mdConfig.headingFormats[1] = { H2, true };
	mdConfig.headingFormats[2] = { H3, false };
	mdConfig.userData = NULL;
	mdConfig.formatCallback = ExampleMarkdownFormatCallback;
	ImGui::Markdown(markdown_.c_str(), markdown_.length(), mdConfig);
}

void Editor::MarkdownExample()
{
	const std::string markdownText = u8R"(
# H1 Header: Text and Links
You can add [links like this one to enkisoftware](https://www.enkisoftware.com/) and lines will wrap well.
You can also insert images ![image alt text](image identifier e.g. filename)
Horizontal rules:
***
___
*Emphasis* and **strong emphasis** change the appearance of the text.
## H2 Header: indented text.
  This text has an indent (two leading spaces).
    This one has two.
### H3 Header: Lists
  * Unordered lists
    * Lists can be indented with two extra spaces.
  * Lists can have [links like this one to Avoyd](https://www.avoyd.com/) and *emphasized text*
)";
	Markdown(markdownText);
}

// Refactor this function (it's not done the right way right now...).
void Editor::UpdatePanelsState()
{
	if (panelsState.showViewportWindow == true)
	{
		if (panelViewport == nullptr)
		{
			panelViewport = new PanelViewport(this, engine);
			AddPanel(panelViewport);
		}
	}
	else
	{
		if (panelViewport != nullptr)
		{
			RemovePanel(panelViewport);
			panelViewport = nullptr;
		}
	}

	if (panelsState.showGameWindow == true)
	{
		/*if (panelGame == nullptr)
		{
			panelGame = new PanelGame(this);
			AddPanel(panelGame);
		}*/
	}
	else
	{
		if (panelGame != nullptr)
		{
			RemovePanel(panelGame);
			panelGame = nullptr;
		}
	}
}

void Editor::OpenTextEditor(std::string path)
{
	toggleTextEditor = true;
	panelTextEditor->LoadFile(path);
}

bool Editor::MouseOnScene()
{
	return mouseScenePosition.x > 0 && mouseScenePosition.x < viewportSize.x
		&& mouseScenePosition.y > 0 && mouseScenePosition.y < viewportSize.y;
}


