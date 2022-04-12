#include "R_Model.h"
#include "FSDefs.h"

R_Model::R_Model() : Resource(ResourceType::MODEL)
{
}

R_Model::~R_Model()
{
}

bool R_Model::SaveMeta(Json& json) const
{
	Json jsonResources;
	for (auto node : nodes)
	{
		Json jsonResource;
		if (node.mesh != 0)
		{
			std::string meshNode = node.name + MESH_EXTENSION;
			std::string meshPath = MESHES_DIR + std::to_string(node.mesh) + MESH_EXTENSION;
			jsonResource["uid"] = node.uid;
			jsonResource["type"] = ResourceType::MESH;
			jsonResource["name"] = meshNode.c_str();
			jsonResource["library_path"] = meshPath.c_str();
			jsonResources[node.name].push_back(jsonResource);
		}
		if (node.material != 0)
		{
			//std::string materialNode = node.name + MATERIAL_EXTENSION;
			//std::string materialPath = MATERIALS_DIR + std::to_string(node.material) + MATERIAL_EXTENSION;
			//jsonResource["uid"] = node.uid;
			//jsonResource["type"] = ResourceType::MATERIAL;
			//jsonResource["name"] = materialNode.c_str();
			//jsonResource["library_path"] = materialPath.c_str();
			//jsonResources[node.name].push_back(jsonResource);
		}
		//if (node.shader != 0)
		//{
		//	std::string shaderNode = node.name + SHADER_EXTENSION;
		//	//TODO: Should we put shader dir?
		//	//std::string shaderPath = SHADER_DIR + std::to_string(node.shader) + SHADER_EXTENSION;
		//	jsonResource["uid"] = node.uid;
		//	jsonResource["type"] = ResourceType::SHADER;
		//	jsonResource["name"] = shaderNode.c_str();
		//	//jsonResource["library_path"] = shaderPath.c_str();
		//	jsonResources[node.name].push_back(jsonResource);
		//}
		if (node.texture != 0)
		{
			std::string textureNode = node.textureName;
			std::string texturePath = TEXTURES_DIR + std::to_string(node.texture) + TEXTURE_EXTENSION;
			jsonResource["uid"] = node.uid;
			jsonResource["type"] = ResourceType::TEXTURE;
			jsonResource["name"] = textureNode.c_str();
			jsonResource["library_path"] = texturePath.c_str();
			jsonResources[node.name].push_back(jsonResource);
		}

		//TODO: ANIMATION WHEN READY

		json["resources"].push_back(jsonResources);
	}
	return true;
}

bool R_Model::LoadMeta(Json& json)
{
	return true;
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

void ModelNode::Save(Json& json) const
{
	json["name"] = name.c_str();
	json["uid"] = uid;
	json["parent_uid"] = parentUid;
	json["mesh_uid"] = mesh;
	json["material_uid"] = material;
	json["shader_uid"] = shader;
	json["texture_uid"] = texture;
	json["texture_name"] = textureName.c_str();
}

void ModelNode::Load(Json& json)
{
	name = json.at("name").get<std::string>();
	uid = json.at("uid");
	parentUid = json.at("parent_uid");
	mesh = json.at("mesh_uid");
	material = json.at("material_uid");
	shader = json.at("shader_uid");
	texture = json.at("texture_uid");
	textureName = json.at("texture_name").get<std::string>();
}
