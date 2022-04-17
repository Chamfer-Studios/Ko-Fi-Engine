#ifndef __MODEL_H__
#define __MODEL_H__

#include "json.hpp"
#include "Resource.h"
#include "MathGeoLib/Math/float3.h"
#include "MathGeoLib/Math/Quat.h"

using Json = nlohmann::json;

class ModelNode
{
public:
	ModelNode();
	ModelNode(std::string name, UID uid, UID parentUid, UID mesh, UID material, UID texture, std::string textureName, float3 pos, Quat rot, float3 scale);
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
	float3 pos;
	Quat rot;
	float3 scale;
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