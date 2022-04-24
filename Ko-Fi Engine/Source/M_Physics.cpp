#include "M_Physics.h"
#include "Engine.h"
#include "M_SceneManager.h"
#include "glew.h"
#include <imgui_stdlib.h>
M_Physics::M_Physics(KoFiEngine* engine)
{
	this->engine = engine;
	
	//World Settings
	reactphysics3d::PhysicsWorld::WorldSettings worldSettings;
	worldSettings.gravity = reactphysics3d::Vector3(0, -9.81f, 0);

	// Create the physics world 
	world = physicsCommon.createPhysicsWorld(worldSettings);
	world->setIsDebugRenderingEnabled(true);
	reactphysics3d::DebugRenderer& debugRenderer = world->getDebugRenderer();

	// Select the contact points and contact normals to be displayed 
	debugRenderer.setIsDebugItemDisplayed(reactphysics3d::DebugRenderer::DebugItem::COLLISION_SHAPE, true);
	debugRenderer.setIsDebugItemDisplayed(reactphysics3d::DebugRenderer::DebugItem::CONTACT_POINT, true);
	debugRenderer.setIsDebugItemDisplayed(reactphysics3d::DebugRenderer::DebugItem::CONTACT_NORMAL, true);
}

M_Physics::~M_Physics()
{
}

bool M_Physics::Awake(Json configModule)
{
	

	return true;
}

bool M_Physics::Start()
{

	return true;
}

bool M_Physics::Update(float dt)
{
	if (engine->GetSceneManager()->GetGameState() == GameState::PLAYING)
	{
		world->update(dt);
	}
	else {
		world->update(0.000000001f);
	}
	return true;
}

bool M_Physics::RenderPhysics()
{
	reactphysics3d::DebugRenderer& debugRenderer = world->getDebugRenderer();
	int numLines = debugRenderer.getNbLines();
	int numTriangles = debugRenderer.getNbTriangles();

	const reactphysics3d::Array<reactphysics3d::DebugRenderer::DebugLine>& lines = debugRenderer.getLines();
	const reactphysics3d::Array<reactphysics3d::DebugRenderer::DebugTriangle>& triangles = debugRenderer.getTriangles();

	for (int i = 0; i < numLines; ++i)
	{
		glBegin(GL_LINES);
		glVertex2f(lines[i].point1.x, lines[i].point1.y);
		glVertex2f(lines[i].point2.x, lines[i].point2.y);
		glEnd();
	}
	for (int i = 0; i < numTriangles; ++i)
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

		glBegin(GL_TRIANGLES);
		glColor3f(triangles[i].color1, triangles[i].color2, triangles[i].color3);
		glVertex3f(triangles[i].point1.x, triangles[i].point1.y, triangles[i].point1.z);
		glVertex3f(triangles[i].point2.x, triangles[i].point2.y, triangles[i].point2.z);
		glVertex3f(triangles[i].point3.x, triangles[i].point3.y, triangles[i].point3.z);
		glEnd();
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	}
	return true;
}

bool M_Physics::CleanUp()
{
	// Destroy a physics world 
	physicsCommon.destroyPhysicsWorld(world);

	return true;
}

bool M_Physics::SaveConfiguration(Json& configModule) const
{
	return true;
}

bool M_Physics::LoadConfiguration(Json& configModule)
{
	return true;
}

bool M_Physics::InspectorDraw()
{
	if (ImGui::CollapsingHeader("Physics##"))
	{
		if (ImGui::TreeNodeEx("Filters"))
		{
			auto iter = filters.begin();
			while (iter != filters.end())
			{
				ImGui::Text(iter->second.c_str());
				ImGui::SameLine();
				std::string label = "delete##";
				label += iter->first;
				if (ImGui::Button(label.c_str()))
				{
					RemoveFilter(iter->second);
					engine->SaveConfiguration();
					break;
				}

				++iter;
			}
		

			ImGui::Text("Create Filter: ");
			ImGui::InputText("##addFilter", &imguiNewFilterText);
			ImGui::SameLine();
			if (ImGui::Button("Add Filter"))
			{
				AddFilter(imguiNewFilterText);
				engine->SaveConfiguration();
			}
			ImGui::TreePop();
		}
		ImGui::Separator();
		if (ImGui::TreeNodeEx("Filter Matrix"))
		{
			size_t filSize = filters.size();
			
			for (auto iterI = filters.begin(); iterI != filters.end(); ++iterI)
			{
				int i = std::distance(filters.begin(), iterI);
				for (auto iterJ = filters.begin(); iterJ != filters.end(); ++iterJ)
				{
					int j = std::distance(filters.begin(), iterJ);
					if (j != 0) ImGui::SameLine();
					std::string label("##" + iterI->second + std::to_string(i) + std::to_string(j));
					bool filterMat = filterMatrix[i][j];
					if (ImGui::Checkbox(label.c_str(), &filterMat))
					{
						filterMatrix[i][j] = filterMat;
						engine->SaveConfiguration();
					}

				}
				ImGui::SameLine();
				ImGui::Text(std::string(iterI->second + " (" + std::to_string(i) + ")").c_str());

			}
			int count = filSize - 1;
			for (int i = 0; i < filSize; ++i)
			{
				if (i != 0)
				{
					ImGui::SameLine();
				}
				ImGui::Text(std::string("  " + std::to_string(i) + "      ").c_str());
			}

			ImGui::TreePop();
		}
		ImGui::Separator();


	}
	
	return true;
}

void M_Physics::OnNotify(const Event& event)
{
}

void M_Physics::AddFilter(std::string newFilter)
{
	filters.emplace(filters.size() + 1,newFilter);
	// TEST: Traverse the 2D array and assign values from old filter matrix and add the new filter
	// First, we want to declare a new filter matrix
	size_t filSize = filters.size();
	bool** newFilterMatrix = new bool* [filSize];
	for (int i = 0; i < filSize; ++i) { newFilterMatrix[i] = new bool[filSize]; }
	// Then we want to copy the old filter matrix to the new one, and set to true the incoming filter change
	for (int i = 0; i < filSize; ++i)
	{
		for (int j = 0; j < filSize; ++j)
		{
			// If i or j gets to the end of filterMatrix (new incoming filter), then set to true, else copy from old filter matrix
			if (i == filSize - 1 || j == filSize - 1)
				newFilterMatrix[i][j] = true;
			else
				newFilterMatrix[i][j] = filterMatrix[i][j];
		}
	}
	DeclareFilterMatrix();

	// TEST: Delete current filter matrix
	DeleteFilterMatrix();

	// TEST: Set the new filter matrix
	SetFilterMatrix(newFilterMatrix);
}

void M_Physics::RemoveFilter(std::string filterToRemove)
{
	for (auto it = filters.begin(); it != filters.end(); ++it)
	{
		if (it->second == filterToRemove)
		{
			filters.erase(it);
		}
	}

	// TEST: Traverse the 2D array and assign values from old filter matrix and add the new filter
// First, we want to declare a new filter matrix
	size_t filSize = filters.size();
	bool** newFilterMatrix = new bool* [filSize];
	for (int i = 0; i < filSize; ++i) { newFilterMatrix[i] = new bool[filSize]; }
	// Then we want to copy the old filter matrix to the new one
	// TODO: We need to re-do the delete logic, as it is not done correctly
	for (int i = 0; i < filSize; ++i)
	{
		if (i != 0) // What I guess this make is that the default filter is kept as true
		{
			for (int j = 0; j < filSize; ++j)
			{
				newFilterMatrix[i][j] = false;
			}
		}
		else
		{
			for (int j = 0; j < filSize; ++j)
				newFilterMatrix[0][j] = true;
		}

	}

	// TEST: Delete current filter matrix
	DeleteFilterMatrix();

	// TEST: Set the new filter matrix
	SetFilterMatrix(newFilterMatrix);
}

unsigned int M_Physics::GetFilter(std::string filter)
{
	for (auto it = filters.begin(); it != filters.end(); ++it)
	{
		if (it->second == filter)
		{
			return it->first;
		}
	}
}
