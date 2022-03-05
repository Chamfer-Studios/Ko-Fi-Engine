#include "GameObject.h"
#include "Engine.h"

#include "Primitive.h"
#include "Globals.h"

#include "ComponentTransform.h"
#include "ComponentMesh.h"
#include "ComponentInfo.h"
#include "ComponentCamera.h"
#include "ComponentRigidBody.h"
#include "ComponentMaterial.h"

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
	//LoadModel(path);
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
	components.push_back(component);
}

void GameObject::AddComponentByType(ComponentType componentType)
{
	// Check if it is repeated
	for (Component* component : components)
	{
		if (component->GetType() == componentType)
		{
			LOG_BOTH("Components cannot be duplicated!");
			return;
		}
	}

	Component* component = nullptr;

	switch (componentType)
	{
		case ComponentType::TRANSFORM: 
		{ 
			component = new ComponentTransform(this); 
			break;
		}	
		case ComponentType::MESH: 
		{ 
			component = new ComponentMesh(this); 
			break;
		}		
		case ComponentType::MATERIAL: 
		{ 
			component = new ComponentMaterial(this); 
			break;
		}	
		case ComponentType::CAMERA: 
		{ 
			component = new ComponentCamera(this); 
			break;
		}		
		case ComponentType::INFO:
		{
			component = new ComponentInfo(this);
			break;
		}
		case ComponentType::RIGID_BODY: 
		{ 
			component = new ComponentRigidBody(this); 
			break;
		}	
	}
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

KoFiEngine* GameObject::GetEngine()
{
	return engine;
}