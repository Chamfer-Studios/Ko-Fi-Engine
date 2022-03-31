#ifndef __MODEL_H__
#define __MODEL_H__

#include <vector>
#include <string>
#include "json.hpp"
#include "Resource.h"

using Json = nlohmann::json;

class ModelNode
{
public:
	ModelNode();
	ModelNode(std::string name, UID uid, UID parentUid, UID mesh, UID material, UID shader, UID texture, std::string textureName);
	~ModelNode();
	void Save(const Json& json) const;
	void Load(Json& json);

public:
	std::string name;
	UID uid;
	UID parentUid;
	UID mesh;
	UID material;
	UID shader;
	UID texture;
	std::string textureName;
};

class Model : public Resource
{
public:
	Model();
	~Model();
	void Save(const Json& json) const;
	void Load(Json& json);

public:
	std::vector<ModelNode> nodes;
};

#endif // !__MODEL_H__