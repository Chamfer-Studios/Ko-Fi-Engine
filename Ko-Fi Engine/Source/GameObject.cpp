#include "GameObject.h"

#include "Primitive.h"

// Used without a path because we use a primitive
GameObject::GameObject(uint id)
{
    active = true;
    this->directory = nullptr; // As we use this constructor for primitives, we don't need a path...
    name = "GameObject" + std::to_string(id);
    this->id = id;
    CreateComponent(COMPONENT_TYPE::COMPONENT_INFO);
}

// Used with a path for the .fbx load
GameObject::GameObject(const char* path, uint id)
{
    active = true;
    //LoadModel(path);
    this->directory = path;
    name = "GameObject" + std::to_string(id);
    this->id = id;
    CreateComponent(COMPONENT_TYPE::COMPONENT_INFO);
}

GameObject::~GameObject()
{
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
        ret = new ComponentTransform();
        break;
    case COMPONENT_TYPE::COMPONENT_MESH:
        ret = new ComponentMesh(directory);
        break;
    case COMPONENT_TYPE::COMPONENT_MATERIAL:
        ret = new ComponentMaterial();
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
        ret = new ComponentTransform();
        break;
    case COMPONENT_TYPE::COMPONENT_MESH:
        ret = new ComponentMesh(subtype);
        break;
    case COMPONENT_TYPE::COMPONENT_MATERIAL:
        ret = new ComponentMaterial();
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


void GameObject::SetName(std::string name)
{
    this->name = name;
}

std::string GameObject::GetName()
{
    return name;
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
