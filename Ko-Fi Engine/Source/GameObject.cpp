#include "GameObject.h"

#include "Primitive.h"
#include "Globals.h"

// Modules
#include "Engine.h"
#include "M_Editor.h"
#include "M_SceneManager.h"
#include "M_Input.h"
#include "M_Camera3D.h"

// Components
#include "C_Mesh.h"
#include "C_Material.h"
#include "C_Particle.h"
#include "C_Camera.h"
#include "C_Script.h"
#include "C_Animator.h"
#include "C_BoxCollider.h"
#include "C_CapsuleCollider.h"
#include "C_SphereCollider.h"
#include "C_Canvas.h"
#include "C_Transform2D.h"
#include "C_Button.h"
#include "C_Image.h"
#include "C_Text.h"
#include "C_RigidBody.h"
#include "C_Transform.h"
#include "C_Info.h"
#include "C_AudioSource.h"
#include "C_AudioSwitch.h"
#include "C_Walkable.h"
#include "C_FollowPath.h"
#include "C_LightSource.h"

// Resources
#include "R_Material.h"

// Used with a path for the .fbx load
GameObject::GameObject(UID uid, KoFiEngine* engine, const char* name, bool _is3D)
{
	active = true;
	this->uid = uid;
	this->engine = engine;

	SetName(name);

	AddComponentByType(ComponentType::INFO);

	is3D = _is3D;
	if (is3D)
		transform = (C_Transform*)AddComponentByType(ComponentType::TRANSFORM);

	tag = TAG::TAG_UNTAGGED;

	this->parent = nullptr;
}

GameObject::GameObject()
{
	active = true;
	children.clear();
	children.shrink_to_fit();

	SetName(nullptr);

	this->uid = 0;
	this->engine = nullptr;

	AddComponentByType(ComponentType::INFO);
	if (is3D)
		transform = (C_Transform*)AddComponentByType(ComponentType::TRANSFORM);

	tag = TAG::TAG_UNTAGGED;

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
		ret = component->Start();

	return ret;
}

bool GameObject::PreUpdate()
{
	bool ret = true;

	for (Component* component : componentsToBeDeleted)
	{
		auto componentIt = std::find(components.begin(), components.end(), component);
		if (componentIt != components.end())
		{
			//(*componentIt)->CleanUp();
			RELEASE(*componentIt);
			components.erase(componentIt);
			components.shrink_to_fit();
		}
	}

	componentsToBeDeleted.clear();
	componentsToBeDeleted.shrink_to_fit();;

	for (Component* component : components)
		ret = component->PreUpdate();

	return ret;
}

bool GameObject::Update(float dt)
{
	OPTICK_EVENT();

	bool ret = true;

	for (int i = 0; i < components.size(); i++)
	{
		Component* component = components[i];
		if (component && component->type != ComponentType::SCRIPT)
			ret = component->Update(dt);
	}

	return ret;
}

bool GameObject::InitUpdateScripts(float dt)
{
	OPTICK_EVENT();

	bool ret = true;

	for (int i = 0; i < components.size(); i++)
	{
		Component* component = components[i];
		if (component && component->type == ComponentType::SCRIPT)
			static_cast<C_Script*>(component)->InitScriptUpdate(dt);
	}

	return ret;
}

bool GameObject::DoUpdateScripts(float dt)
{
	OPTICK_EVENT();

	bool ret = true;

	for (int i = 0; i < components.size(); i++)
	{
		Component* component = components[i];
		if (component && component->type == ComponentType::SCRIPT) {
			if (!(static_cast<C_Script*>(component)->isAsync))
				static_cast<C_Script*>(component)->DoScriptUpdate(dt);
		}
	}

	return ret;
}

bool GameObject::DoUpdateAsyncScripts(float dt)
{
	OPTICK_EVENT();

	bool ret = true;

	for (int i = 0; i < components.size(); i++)
	{
		Component* component = components[i];
		if (component && component->type == ComponentType::SCRIPT)
		{
			if (static_cast<C_Script*>(component)->isAsync)
				static_cast<C_Script*>(component)->DoScriptUpdate(dt);
		}
	}

	return ret;
}

bool GameObject::PostUpdate(float dt)
{
	bool ret = true;

	for (Component* component : components)
		ret = component->PostUpdate(dt);

	// Propagate isActive to children if needed
	if (isActiveWindow)
		PropragateIsActive();

	return ret;
}

bool GameObject::CleanUp()
{
	for (auto& component : components)
	{
		RELEASE(component);
	}

	//for (std::vector<Component*>::iterator component = components.begin(); component != components.end();)
	//{
	//	(*component)->CleanUp();
	//	RELEASE(*component);
	//	component = components.erase(component);
	//}

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
		ret = component->OnPlay();

	return ret;
}

bool GameObject::OnSceneSwitch()
{
	bool ret = true;
	for (Component* component : components)
		ret = component->OnSceneSwitch();

	return ret;
}

bool GameObject::OnPause()
{
	bool ret = true;

	for (Component* component : components)
		ret = component->OnPause();

	return ret;
}

bool GameObject::OnStop()
{
	bool ret = true;

	for (Component* component : components)
		ret = component->OnStop();

	return ret;
}

bool GameObject::OnResume()
{
	bool ret = true;

	for (Component* component : components)
		ret = component->OnResume();

	return ret;
}

bool GameObject::OnTick()
{
	bool ret = true;

	for (Component* component : components)
		ret = component->OnTick();

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

std::vector<C_Script*> GameObject::GetAllScripts()
{
	std::vector<C_Script*> ret;
	for (const auto& comp : components)
	{
		if (comp->type == ComponentType::SCRIPT)
			ret.push_back((C_Script*)comp);
	}
	return ret;
}

void GameObject::DeleteComponent(Component* component)
{
	componentsToBeDeleted.push_back(component);
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
		c = this->CreateComponent<C_Mesh>();
		break;
	}
	case ComponentType::MATERIAL:
	{
		c = this->CreateComponent<C_Material>();
		break;
	}
	case ComponentType::PARTICLE:
	{
		c = this->CreateComponent<C_Particle>();
		break;
	}
	case ComponentType::CAMERA:
	{
		c = this->CreateComponent<C_Camera>();
		break;
	}
	case ComponentType::BOX_COLLIDER:
	{
		if (!this->GetComponent<C_RigidBody>())
			AddComponentByType(ComponentType::RIGID_BODY);

		c = this->CreateComponent<C_BoxCollider>();
		break;
	}
	case ComponentType::CAPSULE_COLLIDER:
	{
		if (!this->GetComponent<C_RigidBody>())
			AddComponentByType(ComponentType::RIGID_BODY);

		c = this->CreateComponent<C_CapsuleCollider>();
		break;
	}
	case ComponentType::SPHERE_COLLIDER:
	{
		if (!this->GetComponent<C_RigidBody>())
			AddComponentByType(ComponentType::RIGID_BODY);

		c = this->CreateComponent<C_SphereCollider>();
		break;
	}
	case ComponentType::SCRIPT:
	{
		c = this->CreateComponent<C_Script>();
		break;
	}
	case ComponentType::RIGID_BODY:
	{
		c = this->CreateComponent<C_RigidBody>();
		break;
	}
	case ComponentType::TRANSFORM2D:
	{
		c = this->CreateComponent<C_Transform2D>();
		break;
	}
	case ComponentType::CANVAS:
	{
		c = this->CreateComponent<C_Canvas>();
		break;
	}
	case ComponentType::IMAGE:
	{
		c = this->CreateComponent<C_Image>();
		break;
	}
	case ComponentType::BUTTON:
	{
		c = this->CreateComponent<C_Button>();
		break;
	}
	case ComponentType::TEXT:
	{
		c = this->CreateComponent<C_Text>();
		break;
	}
	case ComponentType::TRANSFORM:
	{
		c = this->CreateComponent<C_Transform>();
		break;
	}
	case ComponentType::INFO:
	{
		c = this->CreateComponent<C_Info>();
		break;
	}
	case ComponentType::WALKABLE:
	{
		c = this->CreateComponent<C_Walkable>();
		break;
	}
	case ComponentType::FOLLOW_PATH:
	{
		c = this->CreateComponent<C_FollowPath>();
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
	child->parentUid = this->uid;
	child->parent = this;
	children.push_back(child);
	//child->PropagateTransform();
}

void GameObject::RemoveChild(GameObject* child)
{
	auto it = std::find(children.begin(), children.end(), child);
	if (it != children.end())
		children.erase(it);
}

void GameObject::PropagateTransform()
{
	for (GameObject* go : children)
	{
		if (go->transform != nullptr)
			go->transform->RecomputeGlobalMatrix();
	}
}

void GameObject::SetName(const char* name, bool numbered)
{
	if (numbered) {
		this->name = SetObjectNumberedName(name).c_str();
	} else {
		this->name = name;
	}
}

const char* GameObject::GetName() const
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

C_Transform* GameObject::GetTransform() const
{
	OPTICK_EVENT();
	return this->transform;
}

std::vector<Component*> GameObject::GetComponents() const
{
	return components;
}

void GameObject::SetUID(UID uid)
{
	this->uid = uid;
}

UID GameObject::GetUID() const
{
	return uid;
}

void GameObject::SetParentUID(UID uid)
{
	this->parentUid = uid;
}

UID GameObject::GetParentUID() const
{
	return parentUid;
}

bool GameObject::HasChildrenWithUID(UID uid)
{
	for (std::vector<GameObject*>::iterator child = children.begin(); child != children.end(); child++)
	{
		if ((*child)->uid == uid)
			return true;
	}
	return false;
}

bool GameObject::HasParentWithUID(UID uid)
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
	jsonFile["name"] = name;
	jsonFile["active"] = active;
	//jsonFile["UID"] = uid;
	jsonFile["is3D"] = is3D;
	jsonFile["isPrefab"] = isPrefab;
	jsonFile["tag"] = (uint)tag;

	/*if (GetParent())
		jsonFile["parent_UID"] = GetParent()->GetUID();
	else
		jsonFile["parent_UID"] = uid;*/

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
		{
			jsonComponent["type"] = "NONE";
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
		case ComponentType::PARTICLE:
		{
			C_Particle* particleCmp = (C_Particle*)component;
			particleCmp->Save(jsonComponent);
			break;
		}
		case ComponentType::CAMERA:
		{
			C_Camera* cameraCmp = (C_Camera*)component;
			cameraCmp->Save(jsonComponent);
			break;
		}
		case ComponentType::BOX_COLLIDER:
		{
			C_BoxCollider* boxColCmp = (C_BoxCollider*)component;
			boxColCmp->Save(jsonComponent);
			break;
		}
		case ComponentType::SPHERE_COLLIDER:
		{
			C_SphereCollider* sphereColCmp = (C_SphereCollider*)component;
			sphereColCmp->Save(jsonComponent);
			break;
		}
		case ComponentType::CAPSULE_COLLIDER:
		{
			C_CapsuleCollider* capsuleColCmp = (C_CapsuleCollider*)component;
			capsuleColCmp->Save(jsonComponent);
			break;
		}
		case ComponentType::SCRIPT:
		{
			C_Script* scriptCmp = (C_Script*)component;
			scriptCmp->Save(jsonComponent);
			break;
		}
		case ComponentType::RIGID_BODY:
		{
			C_RigidBody* rigidBodyCmp = (C_RigidBody*)component;
			rigidBodyCmp->Save(jsonComponent);
			break;
		}
		case ComponentType::TRANSFORM2D:
		{
			C_Transform2D* transform2DCmp = (C_Transform2D*)component;
			transform2DCmp->Save(jsonComponent);
			break;
		}
		case ComponentType::CANVAS:
		{
			C_Canvas* canvasCmp = (C_Canvas*)component;
			canvasCmp->Save(jsonComponent);
			break;
		}
		case ComponentType::IMAGE:
		{
			C_Image* imageCmp = (C_Image*)component;
			imageCmp->Save(jsonComponent);
			break;
		}
		case ComponentType::BUTTON:
		{
			C_Button* buttonCmp = (C_Button*)component;
			buttonCmp->Save(jsonComponent);
			break;
		}
		case ComponentType::TEXT:
		{
			C_Text* textCmp = (C_Text*)component;
			textCmp->Save(jsonComponent);
			break;
		}
		case ComponentType::TRANSFORM:
		{
			C_Transform* transformCmp = (C_Transform*)component;
			transformCmp->Save(jsonComponent);
			break;
		}
		case ComponentType::INFO:
		{
			C_Info* infoCmp = (C_Info*)component;
			infoCmp->Save(jsonComponent);
			break;
		}
		case ComponentType::AUDIO_SOURCE:
		{
			C_AudioSource* audioSrcCmp = (C_AudioSource*)component;
			audioSrcCmp->Save(jsonComponent);
			break;
		}
		case ComponentType::AUDIO_SWITCH:
		{
			C_AudioSwitch* audioSwitchCmp = (C_AudioSwitch*)component;
			audioSwitchCmp->Save(jsonComponent);
			break;
		}
		case ComponentType::ANIMATOR:
		{
			C_Animator* cAnimator = (C_Animator*)component;
			cAnimator->Save(jsonComponent);
			break;
		}
		case ComponentType::WALKABLE:
		{
			C_Walkable* walkableCmp = (C_Walkable*)component;
			walkableCmp->Save(jsonComponent);
			break;
		}
		case ComponentType::FOLLOW_PATH:
		{
			C_FollowPath* followCmp = (C_FollowPath*)component;
			followCmp->Save(jsonComponent);
			break;
		}
		case ComponentType::LIGHT_SOURCE:
		{
			C_LightSource* componentLightSource = (C_LightSource*)component;
			componentLightSource->Save(jsonComponent);
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

	return ret;
}

bool GameObject::LoadPrefab(Json& jsonFile)
{
	
	SetName(std::string(jsonFile.at("name")).c_str());
	isPrefab = jsonFile.at("isPrefab");
	active = jsonFile.at("active");
	if (jsonFile.contains("tag"))
		tag = (TAG)jsonFile["tag"];
	if (jsonFile.contains("is3D"))
		is3D = jsonFile.at("is3D");
	//if (jsonFile.contains("parent_UID"))
		//parentUid = jsonFile.at("parent_UID");
	//if (jsonFile.contains("UID"))
		//uid = jsonFile.at("UID");

	if (GetParent())
		parentUid = GetParent()->GetUID();

	Json jsonCmp = jsonFile.at("components");
	for (const auto& cmpIt : jsonCmp.items())
	{
		Json jsonCmp = cmpIt.value();
		bool active = jsonCmp.at("active");
		if (jsonCmp.contains("type"))
		{
			ComponentType type = (ComponentType)jsonCmp.at("type").get<int>();
			switch (type)
			{
			case ComponentType::SCRIPT:
			{
				C_Script* scriptCmp = nullptr;
				for (auto c : GetComponents())
				{
					if (c->type == ComponentType::SCRIPT)
					{
						int cID = ((C_Script*)c)->id;
						if (jsonCmp.find("id") != jsonCmp.end())
						{
							if (cID == jsonCmp.at("id"))
								scriptCmp = (C_Script*)c;
						}
					}
				}

				if (scriptCmp == nullptr)
					scriptCmp = (C_Script*)AddComponentByType(ComponentType::SCRIPT);

				scriptCmp->active = active;
				scriptCmp->Load(jsonCmp);
				break;
			}
			case ComponentType::TRANSFORM:
			{
				C_Transform* transformCmp = GetComponent<C_Transform>();
				transformCmp->active = active;
				transformCmp->Load(jsonCmp);
				break;
			}
			case ComponentType::INFO:
			{
				C_Info* infoCmp = GetComponent<C_Info>();
				infoCmp->active = active;
				//infoCmp->Load(jsonCmp); // Does nothing as of now
				break;
			}
			case ComponentType::NONE:
			{
				KOFI_ERROR(" Importer: Component type is none, something went wrong!");
				return false;
				break;
			}
			default:
			{
				Component* component = AddComponentByType(type);
				if (component != nullptr)
				{
					component->active = active;
					component->Load(jsonCmp);
				}
				break;
			}
			}
		}
	}
	for (const auto& comp : components)
	{
		if (comp->type == ComponentType::BUTTON || comp->type == ComponentType::CANVAS || comp->type == ComponentType::IMAGE || comp->type == ComponentType::TEXT || comp->type == ComponentType::TRANSFORM2D)
		{
			if (transform)
			{
				this->DeleteComponent(transform);
			}
		}
	}
	Json jsonChd = jsonFile.at("children");
	for (const auto& chdIt : jsonChd.items())
	{
		Json jsonChd = chdIt.value();
		bool is3D = jsonChd.at("is3D");
		GameObject* go = this->engine->GetSceneManager()->GetCurrentScene()->CreateEmptyGameObject(nullptr, this, is3D);
		go->LoadPrefab(jsonChd);
		this->AttachChild(go);
	}
	return true;
}

bool GameObject::UpdatePrefab(Json& jsonFile)
{
	SetName(std::string(jsonFile.at("name")).c_str());
	isPrefab = jsonFile.at("isPrefab");
	active = jsonFile.at("active");
	if (jsonFile.contains("tag"))
		tag = (TAG)jsonFile["tag"];
	if (jsonFile.contains("is3D"))
		is3D = jsonFile.at("is3D");
	if (jsonFile.contains("parent_UID"))
		parentUid = jsonFile.at("parent_UID");
	if (jsonFile.contains("UID"))
		uid = jsonFile.at("UID");

	Json jsonCmp = jsonFile.at("components");
	for (const auto& cmpIt : jsonCmp.items())
	{
		Json jsonCmp = cmpIt.value();
		bool active = jsonCmp.at("active");
		if (jsonCmp.contains("type"))
		{
			ComponentType type = (ComponentType)jsonCmp.at("type").get<int>();
			switch (type)
			{
			case ComponentType::SCRIPT:
			{
				C_Script* scriptCmp = nullptr;
				for (auto c : GetComponents())
				{
					if (c->type == ComponentType::SCRIPT)
					{
						int cID = ((C_Script*)c)->id;
						if (jsonCmp.find("id") != jsonCmp.end())
						{
							if (cID == jsonCmp.at("id"))
								scriptCmp = (C_Script*)c;
						}
					}
				}

				if (scriptCmp == nullptr)
					scriptCmp = (C_Script*)AddComponentByType(ComponentType::SCRIPT);

				scriptCmp->active = active;
				scriptCmp->Load(jsonCmp);
				break;
			}
			case ComponentType::TRANSFORM:
			{
				C_Transform* transformCmp = GetComponent<C_Transform>();
				transformCmp->active = active;
				transformCmp->Load(jsonCmp);
				break;
			}
			case ComponentType::INFO:
			{
				C_Info* infoCmp = GetComponent<C_Info>();
				infoCmp->active = active;
				//infoCmp->Load(jsonCmp); // Does nothing as of now
				break;
			}
			case ComponentType::NONE:
			{
				KOFI_ERROR(" Importer: Component type is none, something went wrong!");
				return false;
				break;
			}
			default:
			{
				Component* component = AddComponentByType(type);
				if (component != nullptr)
				{
					component->active = active;
					component->Load(jsonCmp);
				}
				break;
			}
			}
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
	bool contains = false;

	for (int i = 0; i < engine->GetEditor()->panelGameObjectInfo.selectedGameObjects.size(); i++)
	{
		if (engine->GetEditor()->panelGameObjectInfo.selectedGameObjects[i] == uid)
			contains = true;
		else
			contains = false;
	}
	return contains;
}

void GameObject::LoadSceneFromName(std::string name)
{
	Importer::GetInstance()->sceneImporter->LoadScene(engine->GetSceneManager()->GetCurrentScene(), name.c_str());
}

std::string GameObject::SetObjectNumberedName(const char* _name)
{
	if (engine->GetSceneManager() == nullptr)
		return _name;

	if (_name == nullptr)
	{
		std::string tmp = std::string("GameObject ") + std::to_string(uid);
		return tmp;
	}

	int count = 0;
	std::string name = std::string(_name);
	std::string number = std::string("");
	std::string chainName = name + number;
	Scene* scene = engine->GetSceneManager()->GetCurrentScene();

	if (scene->IsGameObjectInScene(name)) // Check if there is a replic or not || BakerHouse, not BakerHouse0
	{
		count++;
		number = std::to_string(count);
		chainName = name + " (" + number + ")";
	}
	else
		return name; // If there is no object with that name return the name asigned

	while (scene->IsGameObjectInScene(chainName) != false) // Check if replics exists || BakerHouse0, BakerHouse1...
	{
		count++;
		number = std::to_string(count);
		chainName = name + " (" + number + ")";
	}

	return chainName;
}

void GameObject::SetChangeScene(bool changeSceneLua, std::string sceneNameLua)
{
	changeScene = changeSceneLua;
	sceneName = sceneNameLua;
}

void GameObject::PropragateIsActive()
{
	if (children.size() == 0)
	{
		isActiveWindow = false;
		return;
	}

	if (ImGui::BeginPopupModal("Apply To Children"))
	{
		ImGui::SetWindowSize(ImVec2(250, 120));
		ImGui::Text("Do you want to apply to all children?");
		ImGui::Spacing();

		if (ImGui::Button("YES", ImVec2(70, 30)))
		{
			SetIsActiveToChildren(children, active);
			isActiveWindow = false;
		}
		ImGui::SameLine();
		if (ImGui::Button("NO", ImVec2(70, 30)))
		{
			isActiveWindow = false;
		}

		ImGui::EndPopup();
	}
	ImGui::OpenPopup("Apply To Children");

}

void GameObject::SetIsActiveToChildren(std::vector<GameObject*>& list, bool value)
{
	for (GameObject* go : list)
	{
		go->active = value;
		if (go->children.size() > 0)
		{
			SetIsActiveToChildren(go->children, value);
		}
	}
}
void GameObject::OnStoped()
{
	isQuitting = true;
}

GameObject* GameObject::GetChildWithName(std::string childName)
{
	for (std::vector<GameObject*>::iterator child = children.begin(); child != children.end(); child++)
	{
		if ((*child)->name == childName)
			return (*child);
	}
	return nullptr;
}

void GameObject::Active(bool isActive)
{
	std::vector<GameObject*> childrenList = this->GetChildren();
	for (std::vector<GameObject*>::iterator chdIt = childrenList.begin(); chdIt != childrenList.end(); ++chdIt)
		(*chdIt)->Active(isActive);

	this->active = isActive;
}

void GameObject::Quit()
{
	this->GetEngine()->GetInput()->quitGame = true;
}
