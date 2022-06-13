#include "R_Model.h"
#include "FSDefs.h"
#include <filesystem>

R_Model::R_Model() : Resource(ResourceType::MODEL),
animation(0),
animationName("")
{
}

R_Model::~R_Model()
{
	for (auto node : nodes)
	{
		node.~ModelNode();
	}
	nodes.clear();
	nodes.shrink_to_fit();

	animationName.clear();
	animationName.shrink_to_fit();
}

bool R_Model::SaveMeta(Json& json) const
{
	bool ret = true;

	std::vector<UID> savedMeshesUid;
	std::vector<UID> savedTexturesUid;

	json["contained_resources"].array();
	Json jsonResourceMesh;
	Json jsonResourceTexture;
	for (const auto& node : nodes)
	{
		if (node.mesh != 0)
		{
			bool alreadySaved = false;
			for (const auto& meshIt : savedMeshesUid)
			{
				if (meshIt == node.mesh)
				{
					alreadySaved = true;
					break;
				}
			}

			if (!alreadySaved)
			{
				std::string meshName = node.name + MESH_EXTENSION;
				std::string meshPath = MESHES_DIR + std::to_string(node.mesh) + MESH_EXTENSION;
				jsonResourceMesh["uid"] = node.mesh;
				jsonResourceMesh["type"] = ResourceType::MESH;
				jsonResourceMesh["asset_file"] = meshName;
				jsonResourceMesh["library_path"] = meshPath;
				json["contained_resources"].push_back(jsonResourceMesh);
				savedMeshesUid.push_back(node.mesh);
			}
		}
		if (node.texture != 0 && node.texturePath != "")
		{
			bool alreadySaved = false;
			for (const auto& textureIt : savedTexturesUid)
			{
				if (textureIt == node.texture)
				{
					alreadySaved = true;
					break;
				}
			}

			if (!alreadySaved)
			{
				std::string texturePath = TEXTURES_DIR + std::to_string(node.texture) + TEXTURE_EXTENSION;
				jsonResourceTexture["uid"] = node.texture;
				jsonResourceTexture["type"] = ResourceType::TEXTURE;
				std::filesystem::path textureName = node.texturePath;
				jsonResourceTexture["asset_file"] = textureName.filename().string();
				jsonResourceTexture["asset_path"] = node.texturePath;
				jsonResourceTexture["library_path"] = texturePath;
				json["contained_resources"].push_back(jsonResourceTexture);
				savedTexturesUid.push_back(node.texture);
			}
		}
	}

	savedMeshesUid.clear();
	savedMeshesUid.shrink_to_fit();
	savedTexturesUid.clear();
	savedTexturesUid.shrink_to_fit();

	if (animation != 0 && animationName != "")
	{
		Json jsonAnim;
		std::string animationFile = animationName + ANIMATION_EXTENSION;
		std::string animationPath = ANIMATIONS_DIR + std::to_string(animation) + ANIMATION_EXTENSION;
		jsonAnim["uid"] = animation;
		jsonAnim["type"] = ResourceType::ANIMATION;
		jsonAnim["asset_file"] = animationFile;
		jsonAnim["library_path"] = animationPath;
		json["contained_resources"].push_back(jsonAnim);
	}

	return ret;
}

// MODEL NODE ------------------------------------------------------------------------------------------------

ModelNode::ModelNode() :
	name(""),
	uid(0),
	parentUid(0),
	mesh(0),
	texture(0),
	texturePath(""),
	position(float3::zero),
	rotation(Quat::identity),
	scale(float3::zero)
{}

ModelNode::ModelNode(std::string name, UID uid, UID parentUid, UID mesh, UID texture, std::string texturePath, float3 position, Quat rotation, float3 scale) :
name(name),
uid(uid),
parentUid(parentUid),
mesh(mesh),
texture(texture),
texturePath(texturePath),
position(position),
rotation(rotation),
scale(scale)
{}

ModelNode::~ModelNode()
{
	name.clear();
	name.shrink_to_fit();
	texturePath.clear();
	texturePath.shrink_to_fit();
}
