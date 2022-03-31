#include "Model.h"

Model::Model() : Resource(ResourceType::MODEL)
{
}

Model::~Model()
{
}

void Model::Save(const Json& json) const
{
}

void Model::Load(Json& json)
{
}

// MODEL NODE ------------------------------------------------------------------------------------------------

ModelNode::ModelNode() :
	name("NAME"),
	uid(0),
	parentUid(0),
	mesh(0),
	material(0),
	shader(0),
	texture(0),
	textureName(0)
{}

ModelNode::ModelNode(std::string name, UID uid, UID parentUid, UID mesh, UID material, UID shader, UID texture, std::string textureName) :
name(name),
uid(uid),
parentUid(parentUid),
mesh(mesh),
material(material),
shader(shader),
texture(texture),
textureName(textureName)
{}

ModelNode::~ModelNode()
{
}

void ModelNode::Save(const Json& json) const
{
}

void ModelNode::Load(Json& json)
{
}
