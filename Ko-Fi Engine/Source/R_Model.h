#ifndef __MODEL_H__
#define __MODEL_H__

#include "json.hpp"
#include "Resource.h"

using Json = nlohmann::json;

class ModelNode
{
public:
	ModelNode();
	ModelNode(std::string name, UID uid, UID parentUid, UID mesh, UID material, UID texture, std::string textureName);
	~ModelNode();
	void Save(Json& json) const;
	void Load(Json& json);

public:
	std::string name;
	UID uid;
	UID parentUid;
	UID mesh;
	UID material;
	UID texture;
	std::string textureName;
};

class R_Model : public Resource
{
public:
	R_Model();
	~R_Model();
	bool SaveMeta(Json& json) const override;
	bool LoadMeta(Json& json) override;

public:
	std::vector<ModelNode> nodes;
};

#endif // !__MODEL_H__