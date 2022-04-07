#include "GameObject.h"
#include "Engine.h"

#include "Primitive.h"
#include "Globals.h"

#include "C_Mesh.h"
#include "C_Material.h"
#include "ComponentParticle.h"
#include "C_Camera.h"
#include "ComponentScript.h"
#include "C_Animator.h"
#include "C_Collider.h"
#include "ComponentCanvas.h"
#include "ComponentTransform2D.h"
#include "ComponentButton.h"
#include "ComponentImage.h"
#include "ComponentText.h"
#include "C_RigidBody.h"
#include "ComponentTransform.h"
#include "C_Info.h"
#include "C_AudioSource.h"
#include "C_AudioSwitch.h"
#include "ComponentWalkable.h"
#include "ComponentFollowPath.h"
#include "C_LightSource.h"
#include "Material.h"

// Used with a path for the .fbx load
GameObject::GameObject(int uid, KoFiEngine* engine, const char* name, bool _is3D)
{
	active = true;
	this->uid = uid;
	this->engine = engine;

	if (name == nullptr)
	{
		std::string tmp = std::string("GameObject ") + std::to_string(uid);
		SetName(tmp.c_str());
	}
	else
		SetName(name);

	CreateComponent<C_Info>();

	is3D = _is3D;
	if (is3D)
		transform = CreateComponent<ComponentTransform>();

	tag = Tag::TAG_UNTAGGED;

	this->parent = nullptr;
}

GameObject::GameObject()
{
	active = true;
	children.clear();
	children.shrink_to_fit();

	this->name = "GameObject " + std::to_string(uid);

	this->uid = uid;
	this->engine = engine;

	CreateComponent<C_Info>();
	transform = CreateComponent<ComponentTransform>();

	tag = Tag::TAG_UNTAGGED;

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
		if (component)
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
		if (component->GetType() != ComponentType::MESH)
		{
			// This is the dirty patch
			component->CleanUp();
			RELEASE(component);
		}
	}

	components.clear();
	components.shrink_to_fit();

	children.clear();
	children.shrink_to_fit();

	parent = nullptr;

	return true;
}

bool GameObject::OnPlay()
{
	bool ret = true;
	for (Component* component : components)
	{
		ret = component->OnPlay();
	}
	return ret;
}

bool GameObject::OnPause()
{
	bool ret = true;

	for (Component* component : components)
	{
		ret = component->OnPause();
	}

	return ret;
}

bool GameObject::OnStop()
{
	bool ret = true;

	for (Component* component : components)
	{
		ret = component->OnStop();
	}

	return ret;
}

bool GameObject::OnResume()
{
	bool ret = true;

	for (Component* component : components)
	{
		ret = component->OnResume();
	}

	return ret;
}

bool GameObject::OnTick()
{
	bool ret = true;

	for (Component* component : components)
	{
		ret = component->OnTick();
	}

	return ret;
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


Component* GameObject::AddComponentByType(ComponentType componentType)
{
	// Check if it is repeated
	for (Component* component : components)
	{
		if (component->GetType() == componentType)
		{
			if (componentType != ComponentType::SCRIPT)
			{
				LOG_BOTH("Components cannot be duplicated!");
				return component;
			}
		}
	}

	Component* c = nullptr;
	switch (componentType)
	{
	case ComponentType::MESH:
	{
		//// Set Default Material
		//c = this->CreateComponent<C_Material>();
		//Material* material = new Material();
		//Importer::GetInstance()->materialImporter->LoadAndCreateShader(material->GetShaderPath(), material);
		//this->GetComponent<C_Material>()->SetMaterial(material);

		//// Set a Default Model
		c = this->CreateComponent<C_Mesh>();
		//Mesh* mesh = new Mesh();
		//Importer::GetInstance()->meshImporter->Load("Library/Meshes/Sphere.sugar", mesh);
		//this->GetComponent<C_Mesh>()->SetMesh(mesh);

		break;
	}
	case ComponentType::MATERIAL:
	{
		c = this->CreateComponent<C_Material>();
		break;
	}
	case ComponentType::PARTICLE:
	{
		c = this->CreateComponent<ComponentParticle>();
		break;
	}
	case ComponentType::CAMERA:
	{
		c = this->CreateComponent<C_Camera>();
		break;
	}
	case ComponentType::COLLIDER:
	{
		if (!this->GetComponent<C_RigidBody>())
			AddComponentByType(ComponentType::RIGID_BODY);

		c = this->CreateComponent<C_Collider>();
		break;
	}
	case ComponentType::SCRIPT:
	{
		c = this->CreateComponent<ComponentScript>();
		break;
	}
	case ComponentType::RIGID_BODY:
	{
		c = this->CreateComponent<C_RigidBody>();
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
		c = this->CreateComponent<C_Info>();
		break;
	}
	case ComponentType::WALKABLE:
	{
		c = this->CreateComponent<ComponentWalkable>();
		break;
	}
	case ComponentType::FOLLOW_PATH:
	{
		c = this->CreateComponent<ComponentFollowPath>();
		break;
	}
	case ComponentType::AUDIO_SOURCE:
	{
		c = this->CreateComponent<C_AudioSource>();
		break;
	}
	case ComponentType::AUDIO_SWITCH:
	{
		c = this->CreateComponent<C_AudioSwitch>();
		break;
	}
	case ComponentType::ANIMATOR:
	{
		c = this->CreateComponent<C_Animator>();
		break;
	}
	case ComponentType::LIGHT_SOURCE:
	{
		c = this->CreateComponent<C_LightSource>();
		engine->GetSceneManager()->GetCurrentScene()->AddLight(this);
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
	//child->PropagateTransform();
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
			go->transform->RecomputeGlobalMatrix();
	}
}

void GameObject::SetName(const char* name)
{
	if (engine->GetSceneManager() == nullptr)
		this->name = name;
	else
		this->name = SetObjectNumberedName(name).c_str();
}

const char *GameObject::GetName() const
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

GameObject* GameObject::GetParent() const
{
	return parent;
}

ComponentTransform *GameObject::GetTransform() const
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
	return GetComponent<C_Mesh>()->GetGlobalAABB();
}

bool GameObject::PrefabSaveJson()
{
	bool ret = false;

	JsonHandler jsonHandler;
	Json jsonFile;

	const char* name = this->name.c_str();

	this->PrefabSave(jsonFile);

	std::string path = "Assets/Prefabs/" + std::string(name) + "_prefab.json";
	
	this->prefabPath = path;

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
			C_Mesh* meshCmp = (C_Mesh*)component;
			meshCmp->Save(jsonComponent);
			break;
		}
		case ComponentType::MATERIAL:
		{
			C_Material* materialCmp = (C_Material*)component;
			materialCmp->Save(jsonComponent);
			break;
		}
		case ComponentType::INFO:
		{
			C_Info* infoCmp = (C_Info*)component;
			infoCmp->Save(jsonComponent);
			break;
		}
		case ComponentType::CAMERA:
		{
			C_Camera* cameraCmp = (C_Camera*)component;
			cameraCmp->Save(jsonComponent);
			break;
		}
		case ComponentType::RIGID_BODY:
		{
			C_RigidBody* rigidBodyCmp = (C_RigidBody*)component;
			rigidBodyCmp->Save(jsonComponent);
			break;
		}
		case ComponentType::COLLIDER:
		{
			C_Collider* collisionCmp = (C_Collider*)component;
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

bool GameObject::LoadPrefabJson(const char* path, bool exists)
{
	bool ret = false;

	JsonHandler jsonHandler;
	Json jsonFile;

	this->prefabPath = path;

	ret = jsonHandler.LoadJson(jsonFile, path);

	if (!jsonFile.is_null())
	{
		ret = true;
		if (exists)
			this->UpdatePrefab(jsonFile);
		else
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
		if (type == "mesh")
		{
			C_Mesh* meshCmp = this->GetComponent<C_Mesh>();
			if (meshCmp == nullptr)
			{
				meshCmp = this->CreateComponent<C_Mesh>();
			}
			meshCmp->active = true;
			meshCmp->Load(jsonCmp);
		}
		else if (type == "material")
		{
			C_Material* materialCmp = this->GetComponent<C_Material>();
			if (materialCmp == nullptr)
			{
				materialCmp = this->CreateComponent<C_Material>();
			}
			materialCmp->active = true;
			materialCmp->Load(jsonCmp);
		}
		else if (type == "info")
		{
			C_Info* infoCmp = this->GetComponent<C_Info>();
			infoCmp->active = true;
			infoCmp->Load(jsonCmp); // does nothing as of now
		}
		else if (type == "camera")
		{
			C_Camera* cameraCmp = this->GetComponent<C_Camera>();
			if (cameraCmp == nullptr)
			{
				cameraCmp = this->CreateComponent<C_Camera>();
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
			C_RigidBody* rbCmp = this->GetComponent<C_RigidBody>();
			if (rbCmp == nullptr)
			{
				rbCmp = this->CreateComponent<C_RigidBody>();
			}
			rbCmp->active = true;
			rbCmp->Load(jsonCmp);
		}
		else if (type == "collider")
		{
			C_Collider* colCmp = this->GetComponent<C_Collider>();
			if (colCmp == nullptr)
			{
				colCmp = this->CreateComponent<C_Collider>();
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

bool GameObject::UpdatePrefab(Json& jsonFile)
{
	this->isPrefab = jsonFile.at("isPrefab");
	this->active = jsonFile.at("active");
	Json jsonCmp = jsonFile.at("components");
	for (const auto& cmpIt : jsonCmp.items())
	{
		Json jsonCmp = cmpIt.value();
		bool active = jsonCmp.at("active");
		std::string type = jsonCmp.at("type");

		if (type == "mesh")
		{
			C_Mesh* meshCmp = this->GetComponent<C_Mesh>();
			if (meshCmp == nullptr)
			{
				meshCmp = this->CreateComponent<C_Mesh>();
			}
			meshCmp->active = true;
			meshCmp->Load(jsonCmp);
		}
		else if (type == "material")
		{
			C_Material* materialCmp = this->GetComponent<C_Material>();
			if (materialCmp == nullptr)
			{
				materialCmp = this->CreateComponent<C_Material>();
			}
			materialCmp->active = true;
			materialCmp->Load(jsonCmp);
		}
		else if (type == "info")
		{
			C_Info* infoCmp = this->GetComponent<C_Info>();
			infoCmp->active = true;
			infoCmp->Load(jsonCmp); //does nothing as of now
		}
		else if (type == "camera")
		{
			C_Camera* cameraCmp = this->GetComponent<C_Camera>();
			if (cameraCmp == nullptr)
			{
				cameraCmp = this->CreateComponent<C_Camera>();
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
			C_RigidBody* rbCmp = this->GetComponent<C_RigidBody>();
			if (rbCmp == nullptr)
			{
				rbCmp = this->CreateComponent<C_RigidBody>();
			}
			rbCmp->active = true;
			rbCmp->Load(jsonCmp);
		}
		else if (type == "collider")
		{
			C_Collider* colCmp = this->GetComponent<C_Collider>();
			if (colCmp == nullptr)
			{
				colCmp = this->CreateComponent<C_Collider>();
			}
			colCmp->active = true;
			colCmp->Load(jsonCmp);
		}
	}
	Json jsonChd = jsonFile.at("children");
	for (const auto& chdIt : jsonChd.items())
	{
		Json jsonChd = chdIt.value();
		std::vector<GameObject*> gos = this->GetChildren();
		bool childFound = false;
		for (std::vector<GameObject*>::iterator chdIt = gos.begin(); chdIt != gos.end(); ++chdIt)
		{
			if ((*chdIt)->name == jsonChd.at("name"))
			{
				childFound = true;
				(*chdIt)->UpdatePrefab(jsonChd);
			}
		}
		if (!childFound)
		{
			GameObject* go = this->engine->GetSceneManager()->GetCurrentScene()->CreateEmptyGameObject();
			go->UpdatePrefab(jsonChd);
			this->AttachChild(go);
		}
	}
	return true;
}

bool GameObject::IsSelected()
{
	return engine->GetEditor()->panelGameObjectInfo.selectedGameObjectID == uid;
}

void GameObject::LoadSceneFromName(std::string name)
{
	Importer::GetInstance()->sceneImporter->Load(engine->GetSceneManager()->GetCurrentScene(), name.c_str());
}

std::string GameObject::SetObjectNumberedName(const char* _name)
{
	return _name;

	int count = 0;
	std::string name = std::string(_name);
	std::string number = std::string("");
	std::string chainName = name + number;
	Scene* scene = engine->GetSceneManager()->GetCurrentScene();

	if (scene->IsGameObjectInScene(name)) // Check if there is a replic or not || BakerHouse, not BakerHouse0
	{
		count++;
		number = std::to_string(count);
		chainName = name + number;
	}
	else
		return name; // If there is no object with that name return the name asigned


	while (scene->IsGameObjectInScene(chainName) != false) // Check if replics exists || BakerHouse0, BakerHouse1... 
	{
		count++;
		number = std::to_string(count);
		chainName = name + number;
	}

	return chainName;
}

void GameObject::SetChangeScene(bool changeSceneLua, std::string sceneNameLua)
{
	changeScene = changeSceneLua;
	sceneName = sceneNameLua;
}