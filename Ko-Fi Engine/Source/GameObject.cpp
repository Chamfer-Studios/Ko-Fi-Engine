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
#include "ComponentCanvas.h"
#include "ComponentTransform2D.h"
#include "ComponentButton.h"
#include "ComponentImage.h"
#include "ComponentText.h"

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
		if (component->GetType() != ComponentType::MESH) // This is the dirty patch
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

bool GameObject::PrefabSaveJson()
{
	bool ret = false;

	JsonHandler jsonHandler;
	Json jsonFile;

	const char* name = this->name.c_str();

	this->PrefabSave(jsonFile);

	std::string path = "Assets/Prefabs/" + std::string(name) + "_prefab.json";

	ret = jsonHandler.SaveJson(jsonFile, path.c_str());

	return ret;
}

bool GameObject::PrefabSave(Json& jsonFile)
{
	jsonFile["name"] = this->name;
	jsonFile["active"] = this->active;
	jsonFile["isPrefab"] = this->isPrefab;

	std::vector<Component*> componentsList = this->GetComponents();
	jsonFile["components"] = Json::array();
	for (std::vector<Component*>::iterator cmpIt = componentsList.begin(); cmpIt != componentsList.end(); ++cmpIt)
	{
		Json jsonComponent;

		Component* component = (*cmpIt);

		jsonComponent["active"] = component->active;

		switch (component->GetType())
		{
		case ComponentType::NONE:
			jsonComponent["type"] = "NONE";
			break;
		case ComponentType::TRANSFORM:
		{
			ComponentTransform* transformCmp = (ComponentTransform*)component;
			transformCmp->Save(jsonComponent);
			break;
		}
		case ComponentType::MESH:
		{
			ComponentMesh* meshCmp = (ComponentMesh*)component;
			meshCmp->Save(jsonComponent);
			break;
		}
		case ComponentType::MATERIAL:
		{
			ComponentMaterial* materialCmp = (ComponentMaterial*)component;
			materialCmp->Save(jsonComponent);
			break;
		}
		case ComponentType::INFO:
		{
			ComponentInfo* infoCmp = (ComponentInfo*)component;
			infoCmp->Save(jsonComponent);
			break;
		}
		case ComponentType::CAMERA:
		{
			ComponentCamera* cameraCmp = (ComponentCamera*)component;
			cameraCmp->Save(jsonComponent);
			break;
		}
		case ComponentType::RIGID_BODY:
		{
			ComponentRigidBody* rigidBodyCmp = (ComponentRigidBody*)component;
			rigidBodyCmp->Save(jsonComponent);
			break;
		}
		case ComponentType::COLLIDER:
		{
			ComponentCollider* collisionCmp = (ComponentCollider*)component;
			collisionCmp->Save(jsonComponent);
			break;
		}
		case ComponentType::SCRIPT:
		{
			ComponentScript* scriptCmp = (ComponentScript*)component;
			scriptCmp->Save(jsonComponent);
			break;
		}
		case ComponentType::TRANSFORM2D:
		{
			ComponentTransform2D* transform2DCmp = (ComponentTransform2D*)component;
			transform2DCmp->Save(jsonComponent);
			break;
		}
		case ComponentType::CANVAS:
		{
			ComponentCanvas* canvasCmp = (ComponentCanvas*)component;
			canvasCmp->Save(jsonComponent);
			break;
		}
		case ComponentType::IMAGE:
		{
			ComponentImage* imageCmp = (ComponentImage*)component;
			imageCmp->Save(jsonComponent);
			break;
		}
		case ComponentType::BUTTON:
		{
			ComponentButton* buttonCmp = (ComponentButton*)component;
			buttonCmp->Save(jsonComponent);
			break;
		}
		case ComponentType::TEXT:
		{
			ComponentText* textCmp = (ComponentText*)component;
			textCmp->Save(jsonComponent);
			break;
		}
		default:
			break;
		}
		jsonFile["components"].push_back(jsonComponent);
	}

	std::vector<GameObject*> childrenList = this->GetChildren();
	jsonFile["children"] = Json::array();
	for (std::vector<GameObject*>::iterator chdIt = childrenList.begin(); chdIt != childrenList.end(); ++chdIt)
	{
		json jsonChildren;
		(*chdIt)->PrefabSave(jsonChildren);
		jsonFile["children"].push_back(jsonChildren);
	}

	return true;
}

bool GameObject::LoadPrefabJson(const char* path)
{
	bool ret = false;

	JsonHandler jsonHandler;
	Json jsonFile;

	ret = jsonHandler.LoadJson(jsonFile, path);

	if (!jsonFile.is_null())
	{
		ret = true;
		this->LoadPrefab(jsonFile);
	}
	else
		ret = false;
	return ret;
}

bool GameObject::LoadPrefab(Json& jsonFile)
{
	this->name = jsonFile.at("name");
	this->isPrefab = jsonFile.at("isPrefab");
	this->active = jsonFile.at("active");
	Json jsonCmp = jsonFile.at("components");
	for (const auto& cmpIt : jsonCmp.items())
	{
		Json jsonCmp = cmpIt.value();
		bool active = jsonCmp.at("active");
		std::string type = jsonCmp.at("type");

		if (type == "transform")
		{
			ComponentTransform* transformCmp = this->GetComponent<ComponentTransform>();
			transformCmp->active = true;
			transformCmp->Load(jsonCmp);
		}
		else if (type == "mesh")
		{
			ComponentMesh* meshCmp = this->GetComponent<ComponentMesh>();
			if (meshCmp == nullptr)
			{
				meshCmp = this->CreateComponent<ComponentMesh>();
			}
			meshCmp->active = true;
			meshCmp->Load(jsonCmp);
		}
		else if (type == "material")
		{
			ComponentMaterial* materialCmp = this->GetComponent<ComponentMaterial>();
			if (materialCmp == nullptr)
			{
				materialCmp = this->CreateComponent<ComponentMaterial>();
			}
			materialCmp->active = true;
			materialCmp->Load(jsonCmp);
		}
		else if (type == "info")
		{
			ComponentInfo* infoCmp = this->GetComponent<ComponentInfo>();
			infoCmp->active = true;
			infoCmp->Load(jsonCmp); //does nothing as of now
		}
		else if (type == "camera")
		{
			ComponentCamera* cameraCmp = this->GetComponent<ComponentCamera>();
			if (cameraCmp == nullptr)
			{
				cameraCmp = this->CreateComponent<ComponentCamera>();
			}
			cameraCmp->active = true;
			cameraCmp->Load(jsonCmp);
		}
		else if (type == "script")
		{
			ComponentScript* scriptCmp = this->GetComponent<ComponentScript>();
			if (scriptCmp == nullptr)
			{
				scriptCmp = this->CreateComponent<ComponentScript>();
			}
			scriptCmp->active = true;
			scriptCmp->Load(jsonCmp);
		}
		else if (type == "transform2D")
		{
			ComponentTransform2D* transform2DCmp = this->GetComponent<ComponentTransform2D>();
			if (transform2DCmp == nullptr)
			{
				transform2DCmp = this->CreateComponent<ComponentTransform2D>();
			}
			transform2DCmp->active = true;
			transform2DCmp->Load(jsonCmp);
		}
		else if (type == "canvas")
		{
			ComponentCanvas* canvasCmp = this->GetComponent<ComponentCanvas>();
			if (canvasCmp == nullptr)
			{
				canvasCmp = this->CreateComponent<ComponentCanvas>();
			}
			canvasCmp->active = true;
			canvasCmp->Load(jsonCmp);
		}
		else if (type == "image")
		{
			ComponentImage* imageCmp = this->GetComponent<ComponentImage>();
			if (imageCmp == nullptr)
			{
				imageCmp = this->CreateComponent<ComponentImage>();
			}
			imageCmp->active = true;
			imageCmp->Load(jsonCmp);
		}
		else if (type == "button")
		{
			ComponentButton* buttonCmp = this->GetComponent<ComponentButton>();
			if (buttonCmp == nullptr)
			{
				buttonCmp = this->CreateComponent<ComponentButton>();
			}
			buttonCmp->active = true;
			buttonCmp->Load(jsonCmp);
		}
		else if (type == "text")
		{
			ComponentText* textCmp = this->GetComponent<ComponentText>();
			if (textCmp == nullptr)
			{
				textCmp = this->CreateComponent<ComponentText>();
			}
			textCmp->active = true;
			textCmp->Load(jsonCmp);
		}
		else if (type == "rigidBody")
		{
			ComponentRigidBody* rbCmp = this->GetComponent<ComponentRigidBody>();
			if (rbCmp == nullptr)
			{
				rbCmp = this->CreateComponent<ComponentRigidBody>();
			}
			rbCmp->active = true;
			rbCmp->Load(jsonCmp);
		}
		else if (type == "collider")
		{
			ComponentCollider* colCmp = this->GetComponent<ComponentCollider>();
			if (colCmp == nullptr)
			{
				colCmp = this->CreateComponent<ComponentCollider>();
			}
			colCmp->active = true;
			colCmp->Load(jsonCmp);
		}
	}
	Json jsonChd = jsonFile.at("children");
	for (const auto& chdIt : jsonChd.items())
	{
		Json jsonChd = chdIt.value();
		GameObject* go = this->engine->GetSceneManager()->GetCurrentScene()->CreateEmptyGameObject();
		go->LoadPrefab(jsonChd);
		this->AttachChild(go);
	}
	return true;
}

bool GameObject::IsSelected()
{
	return engine->GetEditor()->panelGameObjectInfo.selectedGameObjectID == uid;
}