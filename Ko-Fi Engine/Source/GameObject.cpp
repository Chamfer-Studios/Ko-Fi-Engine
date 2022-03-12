#include "GameObject.h"
#include "Engine.h"

#include "Primitive.h"
#include "Globals.h"

#include "ComponentMesh.h"
#include "ComponentMaterial.h"
#include "ComponentParticle.h"
#include "ComponentCamera.h"
#include "ComponentCollider.h"
#include "ComponentScript.h"
#include "ComponentRigidBody.h"
#include "ComponentTransform2D.h"
#include "ComponentCanvas.h"
#include "ComponentImage.h"
#include "ComponentButton.h"
#include "ComponentText.h"
#include "ComponentTransform.h"
#include "ComponentInfo.h"

// Used with a path for the .fbx load
GameObject::GameObject(int uid, KoFiEngine* engine, const char* name, bool _is3D)
{
	active = true;

	if (name == nullptr)
		this->name = "GameObject " + std::to_string(uid);
	else
		this->name = name;

	this->uid = uid;
	this->engine = engine;
	CreateComponent<ComponentInfo>();

	is3D = _is3D;
	if (is3D)
		transform = CreateComponent<ComponentTransform>();

	this->parent = nullptr;
}

GameObject::GameObject()
{
	active = true;

	this->name = "GameObject " + std::to_string(uid);

	this->uid = uid;
	this->engine = engine;

	CreateComponent<ComponentInfo>();
	transform = CreateComponent<ComponentTransform>();

	this->parent = nullptr;
}

GameObject::~GameObject()
{
	CleanUp();
}

bool GameObject::Start()
{
	bool ret = true;
	for (Component* component : components)
	{
		ret = component->Start();
	}
	return ret;
}

bool GameObject::PreUpdate()
{
	bool ret = true;
	for (Component* component : components)
	{
		ret = component->PreUpdate();
	}
	return ret;
}

bool GameObject::Update(float dt)
{
	bool ret = true;
	for (Component* component : components)
	{
		ret = component->Update(dt);
	}
	return ret;
}

bool GameObject::PostUpdate(float dt)
{
	bool ret = true;
	if (active)
	{
		for (Component* component : components)
		{
			ret = component->PostUpdate(dt);
		}
	}
	return ret;
}

bool GameObject::CleanUp()
{
	for (Component* component : components)
	{
		RELEASE(component);
	}
	components.clear();
	children.clear();
	parent = nullptr;

	return true;
}

void GameObject::Enable()
{
	active = true;
}

void GameObject::Disable()
{
	active = false;
}

void GameObject::DeleteComponent(Component* component)
{
	auto componentIt = std::find(components.begin(), components.end(), component);
	if (componentIt != components.end())
	{
		(*componentIt)->CleanUp();
		components.erase(componentIt);
		components.shrink_to_fit();
	}
}

void GameObject::AddComponent(Component* component)
{
	// Check if it is repeated
	for (Component* c : components)
	{
		if (c->GetType() == component->GetType())
		{
			LOG_BOTH("Components cannot be duplicated!");
			return;
		}
	}
	components.push_back(component);
}

Component* GameObject::AddComponentByType(ComponentType componentType)
{
	// Check if it is repeated
	for (Component* component : components)
	{
		if (component->GetType() == componentType)
		{
			LOG_BOTH("Components cannot be duplicated!");
			return component;
		}
	}

	Component* c = nullptr;
	switch (componentType)
	{
		case ComponentType::MESH:
		{
			c = this->CreateComponent<ComponentMesh>();
			break;
		}
		case ComponentType::MATERIAL:
		{
			c = this->CreateComponent<ComponentMaterial>();
			break;
		}
		case ComponentType::PARTICLE:
		{
			c = this->CreateComponent<ComponentParticle>();
			break;
		}
		case ComponentType::CAMERA:
		{
			c = this->CreateComponent<ComponentCamera>();
			break;
		}
		case ComponentType::COLLIDER:
		{
			c = this->CreateComponent<ComponentCollider>();
			break;
		}
		case ComponentType::SCRIPT:
		{
			c = this->CreateComponent<ComponentScript>();
			break;
		}
		case ComponentType::RIGID_BODY:
		{
			c = this->CreateComponent<ComponentRigidBody>();
			break;
		}
		case ComponentType::TRANSFORM2D:
		{
			c = this->CreateComponent<ComponentTransform2D>();
			break;
		}
		case ComponentType::CANVAS:
		{
			c = this->CreateComponent<ComponentCanvas>();
			break;
		}
		case ComponentType::IMAGE:
		{
			c = this->CreateComponent<ComponentImage>();
			break;
		}
		case ComponentType::BUTTON:
		{
			c = this->CreateComponent<ComponentButton>();
			break;
		}
		case ComponentType::TEXT:
		{
			c = this->CreateComponent<ComponentText>();
			break;
		}
		case ComponentType::TRANSFORM:
		{
			c = this->CreateComponent<ComponentTransform>();
			break;
		}
		case ComponentType::INFO:
		{
			c = this->CreateComponent<ComponentInfo>();
			break;
		}
	}
	c->Start();
	return c;
}

void GameObject::AttachChild(GameObject* child)
{
	if (child->parent != nullptr)
		child->parent->RemoveChild(child);

	child->parent = this;
	children.push_back(child);
	if (child->transform != nullptr) {
		child->transform->NewAttachment();
		child->PropagateTransform();
	}
}

void GameObject::RemoveChild(GameObject* child)
{
	auto it = std::find(children.begin(), children.end(), child);
	if (it != children.end())
	{
		children.erase(it);
	}
}

void GameObject::PropagateTransform()
{
	for (GameObject* go : children)
	{
		if (go->transform != nullptr)
			go->transform->OnParentMoved();
	}
}

void GameObject::SetName(const char* name)
{
	this->name = name;
}

const char* GameObject::GetName()
{
	return name.c_str();
}

std::vector<GameObject*> GameObject::GetChildren() const
{
	return children;
}

void GameObject::SetChild(GameObject* child)
{
	children.push_back(child);
}

GameObject* GameObject::GetParent()const
{
	return parent;
}

ComponentTransform* GameObject::GetTransform()
{
	return this->transform;
}

std::vector<Component*> GameObject::GetComponents() const
{
	return components;
}

void GameObject::SetUID(uint uid)
{
	this->uid = uid;
}

uint GameObject::GetUID() const
{
	return uid;
}

void GameObject::SetParentUID(uint uid)
{
	this->parentUid = uid;
}

uint GameObject::GetParentUID() const
{
	return parentUid;
}

bool GameObject::HasChildrenWithUID(uint uid)
{
	for (std::vector<GameObject*>::iterator child = children.begin(); child != children.end(); child++)
	{
		if ((*child)->uid == uid)
			return true;
	}
	return false;
}

bool GameObject::HasParentWithUID(uint uid)
{
	while (parent != engine->GetSceneManager()->GetCurrentScene()->rootGo)
	{
		if (parent->uid = uid)
			return true;
	}

	return false;
}

KoFiEngine* GameObject::GetEngine() const
{
	return engine;
}

void GameObject::SetEngine(KoFiEngine* engine)
{
	this->engine = engine;
}
AABB GameObject::BoundingAABB()
{
	return GetComponent<ComponentMesh>()->GetGlobalAABB();
}
