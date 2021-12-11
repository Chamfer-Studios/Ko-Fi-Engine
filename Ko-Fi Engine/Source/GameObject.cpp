#include "GameObject.h"
#include "Engine.h"

#include "Primitive.h"
#include "Defs.h"

#include "ComponentTransform.h"
#include "ComponentMesh.h"
#include "ComponentInfo.h"

// Used with a path for the .fbx load
GameObject::GameObject(int id, KoFiEngine* engine, const char* name)
{
	active = true;
	//LoadModel(path);
	if (name == nullptr)
		this->name = "GameObject " + std::to_string(id);
	else
		this->name = name;

	this->id = id;
	this->engine = engine;

	transform = CreateComponent<ComponentTransform>();
	CreateComponent<ComponentInfo>();
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

bool GameObject::Update()
{
	bool ret = true;
	for (Component* component : components)
	{
		ret = component->Update();
	}
	return ret;
}

bool GameObject::PostUpdate()
{
	bool ret = true;
	if (active)
	{
		for (Component* component : components)
		{
			ret = component->PostUpdate();
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
		components.erase(componentIt);
		components.shrink_to_fit();
	}
}

void GameObject::AddComponent(Component* component)
{
	components.push_back(component);
}

void GameObject::AttachChild(GameObject* child)
{
	if (child->parent != nullptr)
		child->parent->RemoveChild(child);

	child->parent = this;
	children.push_back(child);
	child->transform->NewAttachment();
	child->PropagateTransform();
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
		go->transform->OnParentMoved();
	}
}

ComponentTransform* GameObject::GetTransform()
{
	return this->transform;
}

void GameObject::SetName(std::string name)
{
	this->name = name;
}

std::vector<GameObject*> GameObject::GetChildren() const
{
	return children;
}

void GameObject::SetChild(GameObject* child)
{
	children.push_back(child);
}

std::string GameObject::GetName()
{
	return name;
}

GameObject* GameObject::GetParent()const
{
	return parent;
}

std::vector<Component*> GameObject::GetComponents() const
{
	return components;
}

void GameObject::SetId(int id)
{
	this->id = id;
}

uint GameObject::GetId() const
{
	return id;
}

bool GameObject::HasChildrenWithId(int id)
{
	for (std::vector<GameObject*>::iterator child = children.begin(); child != children.end(); child++)
	{
		if ((*child)->id == id)
			return true;
	}
	return false;
}

KoFiEngine* GameObject::GetEngine()
{
	return engine;
}
