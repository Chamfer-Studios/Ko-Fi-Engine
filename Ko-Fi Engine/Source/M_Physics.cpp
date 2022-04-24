#include "M_Physics.h"
#include "Engine.h"
#include "M_SceneManager.h"
#include "glew.h"

M_Physics::M_Physics(KoFiEngine* engine)
{
	this->engine = engine;
	
	//World Settings
	reactphysics3d::PhysicsWorld::WorldSettings worldSettings;
	worldSettings.gravity = reactphysics3d::Vector3(0, -1.0f, 0);

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
	//this->world->Step(dt);
	world->update(dt);
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
	return true;
}

void M_Physics::OnNotify(const Event& event)
{
}
