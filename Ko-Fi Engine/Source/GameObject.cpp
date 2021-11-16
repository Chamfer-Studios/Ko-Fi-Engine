#include "GameObject.h"
#include "Primitive.h"
#include "Defs.h"

// Used without a path because we use a primitive
GameObject::GameObject(int id, const char* name)
{
	active = true;
	this->directory = ""; // As we use this constructor for primitives, we don't need a path...
	if (name == nullptr) {
		this->name = "GameObject " + std::to_string(id);

	}
	else {
		this->name = name;
	}
	this->id = id;
	CreateComponent(COMPONENT_TYPE::COMPONENT_INFO);
	CreateComponent(COMPONENT_TYPE::COMPONENT_TRANSFORM);
	this->parent = nullptr;
}

// Used with a path for the .fbx load
GameObject::GameObject(const char* path, int id, const char* name)
{
	active = true;
	//LoadModel(path);
	this->directory = path;
	if (name == nullptr) {
		this->name = "GameObject " + std::to_string(id);
	}
	else {
		this->name = name;
	}
	this->id = id;
	CreateComponent(COMPONENT_TYPE::COMPONENT_INFO);
	this->parent = nullptr;
}

GameObject::~GameObject()
{
	for (Component* component : components)
	{
		RELEASE(component);
	}

	components.clear();
	children.clear();
	parent = nullptr;
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

bool GameObject::PreUpdate() {
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

Component* GameObject::CreateComponent(COMPONENT_TYPE type)
{
	Component* ret = nullptr;
	switch (type)
	{
	case COMPONENT_TYPE::COMPONENT_TRANSFORM:
		ret = new ComponentTransform(this);
		break;
	case COMPONENT_TYPE::COMPONENT_MESH:
		ret = new ComponentMesh(this,directory);
		break;
	case COMPONENT_TYPE::COMPONENT_MATERIAL:
		ret = new ComponentMaterial(this);
		break;
	case COMPONENT_TYPE::COMPONENT_INFO:
		ret = new ComponentInfo(this);
		break;
	default:
		break;
	}
	components.push_back(ret);
	return ret;
}

Component* GameObject::CreateComponent(COMPONENT_TYPE type, COMPONENT_SUBTYPE subtype)
{
	Component* ret = nullptr;
	switch (type)
	{
	case COMPONENT_TYPE::COMPONENT_TRANSFORM:
		ret = new ComponentTransform(this);
		break;
	case COMPONENT_TYPE::COMPONENT_MESH:
		ret = new ComponentMesh(this,subtype);
		break;
	case COMPONENT_TYPE::COMPONENT_MATERIAL:
		ret = new ComponentMaterial(this);
		break;
	case COMPONENT_TYPE::COMPONENT_INFO:
		ret = new ComponentInfo(this);
		break;
	default:
		break;
	}
	components.push_back(ret);
	return ret;
}

void GameObject::SetChild(GameObject* go)
{
	if (go->parent != this) {
		if (go->parent != nullptr) {
			go->parent->RemoveChild(go);
		}

		this->children.push_back(go);
		go->parent = this;
	}
}

void GameObject::RemoveChild(GameObject* go)
{
	std::vector<GameObject*>::iterator it = children.begin();
	for (int i = 0; children.size(); ++i, ++it) {
		if (children.at(i) == go) {
			children.erase(it);
			break;
		}
	}
}

void GameObject::SetName(std::string name)
{
	this->name = name;
}

std::vector<GameObject*> GameObject::GetChildren()
{
	return children;
}

std::string GameObject::GetName()
{
	return name;
}

GameObject* GameObject::GetParent()
{
	return parent;
}

std::vector<Component*> GameObject::GetComponents()
{
	return components;
}

uint GameObject::GetId()
{
	return id;
}

Component* GameObject::GetComponent(COMPONENT_TYPE type)
{
	for (Component* component : components)
	{
		if (component->type == type) return component;
	}
	return nullptr;
}

ComponentTransform* GameObject::GetTransform()
{
	return (ComponentTransform*)GetComponent(COMPONENT_TYPE::COMPONENT_TRANSFORM);
}
