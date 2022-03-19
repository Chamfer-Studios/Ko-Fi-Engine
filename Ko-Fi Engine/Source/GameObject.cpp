#include "GameObject.h"
#include "Engine.h"

#include "Primitive.h"
#include "Globals.h"

#include "ComponentTransform.h"
#include "ComponentMesh.h"
#include "ComponentInfo.h"
#include "ComponentCamera.h"
#include "ComponentRigidBody.h"
#include "ComponentCollider.h"
#include "ComponentMaterial.h"
#include "ComponentScript.h"
#include "C_Collider.h"

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

		for (Component* component : components)
		{
			ret = component->PostUpdate(dt);
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

	

	switch (componentType)
	{
		case ComponentType::TRANSFORM: 
		{ 
			this->CreateComponent<ComponentTransform>();
			break;
		}	
		case ComponentType::MESH: 
		{ 
			this->CreateComponent<ComponentMesh>();
			break;
		}
		case ComponentType::SCRIPT:
		{
			this->CreateComponent<ComponentScript>();
			break;
		}
		case ComponentType::COLLIDER:
		{
			this->CreateComponent<ComponentCollider>();
			break;
		}
		case ComponentType::MATERIAL: 
		{ 
			this->CreateComponent<ComponentMaterial>();
			break;
		}	
		case ComponentType::CAMERA: 
		{ 
			this->CreateComponent<ComponentCamera>();
			break;
		}		
		case ComponentType::INFO:
		{
			this->CreateComponent<ComponentInfo>();
			break;
		}
		case ComponentType::RIGID_BODY: 
		{ 
			this->CreateComponent<ComponentRigidBody>();
			break;
		}	
		case ComponentType::COLLIDER2:
		{
			/*this->CreateComponent<ComponentCollider2>();*/
			if (!this->GetComponent<ComponentRigidBody>())
				this->CreateComponent<ComponentRigidBody>();
			ComponentCollider2* cmpColl2 = new ComponentCollider2(this, ColliderShape::NONE);
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
