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
	bool ret = true;

	json["contained_resources"].array();
	Json jsonResource;
	for (const auto& node : nodes)
	{
		if (node.mesh != 0)
		{
			std::string meshName = node.name + MESH_EXTENSION;
			std::string meshPath = MESHES_DIR + std::to_string(node.mesh) + MESH_EXTENSION;
			jsonResource["uid"] = node.uid;
			jsonResource["type"] = ResourceType::MESH;
			jsonResource["asset_file"] = meshName;
			jsonResource["library_path"] = meshPath;
			json["contained_resources"].push_back(jsonResource);
		}
		if (node.material != 0)
		{
			std::string materialName = node.name + SHADER_EXTENSION;
			std::string shaderPath = SHADERS_DIR + std::to_string(node.material) + SHADER_EXTENSION;
			jsonResource["uid"] = node.uid;
			jsonResource["type"] = ResourceType::MATERIAL;
			jsonResource["asset_file"] = materialName;
			jsonResource["library_path"] = shaderPath;
			json["contained_resources"].push_back(jsonResource);
		}
		if (node.texture != 0)
		{
			std::string textureName = node.textureName;
			std::string fileName = node.name + TEXTURE_EXTENSION;
			std::string texturePath = TEXTURES_DIR + std::to_string(node.texture) + TEXTURE_EXTENSION;
			jsonResource["uid"] = node.uid;
			jsonResource["type"] = ResourceType::TEXTURE;
			jsonResource["asset_file"] = fileName;
			jsonResource["library_path"] = texturePath;
			jsonResource["texture_name"] = textureName;
			json["contained_resources"].push_back(jsonResource);
		}
	}

	for (const auto& anim : animations)
	{
		std::string animationName = anim.second + ANIMATION_EXTENSION;
		std::string animationPath = ANIMATIONS_DIR + std::to_string(anim.first) + ANIMATION_EXTENSION;
		jsonResource["uid"] = anim.first;
		jsonResource["type"] = ResourceType::ANIMATION;
		jsonResource["asset_file"] = animationName;
		jsonResource["library_path"] = animationPath;
		json["contained_resources"].push_back(jsonResource);
	}

	return ret;
}

// MODEL NODE ------------------------------------------------------------------------------------------------

ModelNode::ModelNode() :
	name(""),
	uid(0),
	parentUid(0),
	mesh(0),
	material(0),
	texture(0),
	textureName(""),
	position(float3::zero),
	rotation(Quat::identity),
	scale(float3::zero)
{}

ModelNode::ModelNode(std::string name, UID uid, UID parentUid, UID mesh, UID material, UID texture, std::string textureName, float3 position, Quat rotation, float3 scale) :
name(name),
uid(uid),
parentUid(parentUid),
mesh(mesh),
material(material),
texture(texture),
textureName(textureName),
position(position),
rotation(rotation),
scale(scale)
{}

ModelNode::~ModelNode()
{
}
