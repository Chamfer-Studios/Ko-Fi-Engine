#include "C_LightSource.h"

// Modules
#include "Engine.h"
#include "M_SceneManager.h"

// GameObject
#include "GameObject.h"
#include "C_Transform.h"

#include "Globals.h"
#include "SceneIntro.h"

C_LightSource::C_LightSource(GameObject* parent) : Component(parent)
{
	type = ComponentType::LIGHT_SOURCE;
	sourceType = SourceType::DIRECTIONAL;
	DirectionalLight* dLight = new DirectionalLight();
	lightSource = (LightSource*)dLight;
	lightSource->position = owner->GetTransform()->GetPosition();
}

C_LightSource::~C_LightSource()
{
}

bool C_LightSource::Start()
{
	return true;
}

bool C_LightSource::Update(float dt)
{
	lightSource->position = owner->GetTransform()->GetGlobalTransform().TranslatePart();

	return true;
}

bool C_LightSource::CleanUp()
{
	RELEASE(lightSource);
	owner->GetEngine()->GetSceneManager()->GetCurrentScene()->RemoveLight(owner);
	return true;
}

void C_LightSource::Save(Json& json) const
{
	json["type"] = "lightSource";
	json["sourceType"] = (int)sourceType;

	switch (sourceType)
	{
	case SourceType::DIRECTIONAL:
	{
		DirectionalLight* currentLight = (DirectionalLight*)lightSource;
		
		json["lightPosition"] = { currentLight->position.x, currentLight->position.y, currentLight->position.z };
		json["lightColor"] = { currentLight->color.x, currentLight->color.y, currentLight->color.z };
	
		json["lightDirection"] = { currentLight->direction.x, currentLight->direction.y, currentLight->direction.z };
		
		json["ambientValue"] = currentLight->ambient;
		json["diffuseValue"] = currentLight->diffuse;
		json["specularValue"] = currentLight->specular;

		break;
	}
	case SourceType::POINT:
	{
		PointLight* currentLight = (PointLight*)lightSource;
		
		json["lightPosition"] = { currentLight->position.x, currentLight->position.y, currentLight->position.z };
		json["lightColor"] = { currentLight->color.x, currentLight->color.y, currentLight->color.z };
	
		json["ambientValue"] = currentLight->ambient;
		json["diffuseValue"] = currentLight->diffuse;
		json["specularValue"] = currentLight->specular;
		
		json["constantAttenuationValue"] = currentLight->constant;
		json["linearAttenuationValue"] = currentLight->linear;
		json["quadraticAttenuationValue"] = currentLight->quadratic;

		break;
	}
	}
}

void C_LightSource::Load(Json& json)
{
	SourceType type = (SourceType)json.at("sourceType");

	switch (type)
	{
	case SourceType::DIRECTIONAL:
	{
		DirectionalLight* currentLight = (DirectionalLight*)ChangeSourceType(type);
		std::vector<float> values = json.at("lightPosition").get<std::vector<float>>();
		currentLight->position = (float3(values[0], values[1], values[2]));
		values.clear();

		values = json.at("lightColor").get<std::vector<float>>();
		currentLight->color = (float3(values[0], values[1], values[2]));
		values.clear();

		values = json.at("lightDirection").get<std::vector<float>>();
		currentLight->direction = (float3(values[0], values[1], values[2]));
		values.clear();

		currentLight->ambient = json.at("ambientValue");
		currentLight->diffuse = json.at("diffuseValue");
		currentLight->specular = json.at("specularValue");

		break;
	}
	case SourceType::POINT:
	{
		PointLight* currentLight = (PointLight*)ChangeSourceType(type);
		std::vector<float> values = json.at("lightPosition").get<std::vector<float>>();
		currentLight->position = (float3(values[0], values[1], values[2]));
		values.clear();

		values = json.at("lightColor").get<std::vector<float>>();
		currentLight->color = (float3(values[0], values[1], values[2]));
		values.clear();

		currentLight->ambient = json.at("ambientValue");
		currentLight->diffuse = json.at("diffuseValue");
		currentLight->specular = json.at("specularValue");

		currentLight->constant = json.at("constantAttenuationValue");
		currentLight->linear = json.at("linearAttenuationValue");
		currentLight->quadratic = json.at("quadraticAttenuationValue");

		break;
	}
	}
	
}

bool C_LightSource::InspectorDraw(PanelChooser* chooser)
{
	bool ret = true; // TODO: We don't need it to return a bool... Make it void when possible.

	if (ImGui::CollapsingHeader("Component LightSource"))
	{	
		ImGui::Combo("###combo", &sType, "Directional Light Source\0Point Light Source\0Focal Light Source");

		ImGui::SameLine();

		if ((ImGui::Button("Assign Type")))
		{
			switch (sType)
			{
			case (int)SourceType::DIRECTIONAL: ChangeSourceType((SourceType)sType); break;
			case (int)SourceType::POINT: ChangeSourceType((SourceType)sType); break;
			//case (int)SourceType::SPOT: ChangeSourceType((SourceType)sType); break;
			}
		}

		switch (sourceType)
		{
		case SourceType::DIRECTIONAL:
		{
			DirectionalLight* currentLight = (DirectionalLight*)lightSource;

			float color[3] = { currentLight->color.x, currentLight->color.y, currentLight->color.z };
			if (ImGui::DragFloat3("light Color", color, 0.01f, 0.0f, 1.0f, "%.1f"))
			{
				currentLight->color = { color[0], color[1], color[2] };
			}

			float direction[3] = { currentLight->direction.x, currentLight->direction.y, currentLight->direction.z };
			if (ImGui::DragFloat3("light Direction", direction, 0.1f, -10000.0f, 10000.0f, "%.1f"))
			{
				currentLight->direction = { direction[0], direction[1], direction[2] };
			}

			float ambientValue = currentLight->ambient;
			if (ImGui::DragFloat("Ambient Light Value", &ambientValue, 0.1f, 0.0f, 1.0f, "%.1f"))
			{
				currentLight->ambient = ambientValue;
			}

			float diffuseValue = currentLight->diffuse;
			if (ImGui::DragFloat("Diffuse Light Value", &diffuseValue, 0.1f, 0.0f, 1.0f, "%.1f"))
			{
				currentLight->diffuse = diffuseValue;
			}
			break;
		}
		case SourceType::POINT:
		{
			PointLight* currentLight = (PointLight*)lightSource;

			float color[3] = { currentLight->color.x, currentLight->color.y, currentLight->color.z };
			if (ImGui::DragFloat3("light Color", color, 0.01f, 0.0f, 1.0f, "%.1f"))
			{
				currentLight->color = { color[0], color[1], color[2] };
			}
			float ambientValue = currentLight->ambient;
			if (ImGui::DragFloat("Ambient Light Value", &ambientValue, 0.1f, 0.0f, 1.0f, "%.1f"))
			{
				currentLight->ambient = ambientValue;
			}
			float diffuseValue = currentLight->diffuse;
			if (ImGui::DragFloat("Diffuse Light Value", &diffuseValue, 0.1f, 0.0f, 1.0f, "%.1f"))
			{
				currentLight->diffuse = diffuseValue;
			}

			float constantValue = currentLight->constant;
			if (ImGui::DragFloat("Constant Light Attenuation", &constantValue, 0.1f, 0.0f, 1.0f, "%.1f"))
			{
				currentLight->constant = constantValue;
			}
			float linearValue = currentLight->linear;
			if (ImGui::DragFloat("Linear Light Attenuation", &linearValue, 0.1f, 0.0f, 1.0f, "%.1f"))
			{
				currentLight->linear = linearValue;
			}
			float quadraticValue = currentLight->quadratic;
			if (ImGui::DragFloat("Quadratic Light Attenuation", &quadraticValue, 0.1f, 0.0f, 1.0f, "%.1f"))
			{
				currentLight->quadratic = quadraticValue;
			}
			break;
		} 
		} 
		ImGui::NewLine();

		DrawDeleteButton(owner, this);
	}
	else
		DrawDeleteButton(owner, this);

	return ret;
}

LightSource* C_LightSource::ChangeSourceType(SourceType type)
{
	if (type == sourceType)
		return this->GetLightSource();
	
	if (lightSource != nullptr)
	{
		if (sourceType == SourceType::DIRECTIONAL) { numOfDirectional--; }
		else if (sourceType == SourceType::POINT) { numOfPoint--; }
		else { numOfFocal--; }

		RELEASE(lightSource);
	}

	switch (type)
	{
	case SourceType::DIRECTIONAL:
	{
		if (numOfDirectional < MAX_DIR_LIGHTS)
		{
			DirectionalLight* dLight = new DirectionalLight();
			lightSource = (LightSource*)dLight;
			sourceType = type;
			numOfDirectional++;
		}
		else
			CONSOLE_LOG("[C_LightSource]: MAX of directional lights reached");
		break;
	}
	case SourceType::POINT:
	{
		if (numOfPoint < MAX_POINT_LIGHTS)
		{
			PointLight* pLight = new PointLight();
			lightSource = (LightSource*)pLight;
			sourceType = type;
			numOfPoint++;
		}
		else
			CONSOLE_LOG("[C_LightSource]: MAX of point lights reached");
		break;
	}
	case SourceType::FOCAL:
	{
		/*if (numOfFocal < MAX_FOCAL_LIGHTS)
		{
			FocalLight* fLight = new FocalLight();
			lightSource = (LightSource*)fLight;
			numOfFocal++;
		}
		else
			CONSOLE_LOG("[C_LightSource]: MAX of focal lights reached");*/
		break;
	}
	}

	return lightSource;
}

LightSource::LightSource()
{
	position = float3::zero;
	color = float3(1.0f, 1.0f, 1.0f);

	ambient = 0.2;
	diffuse = 0.8;
	specular = 0;
}

DirectionalLight::DirectionalLight() : LightSource()
{
	direction = float3(0.0f, 0.0f, 0.0f);
}

PointLight::PointLight() : LightSource()
{
	constant = 1.00f;
	linear = 0.220f;
	quadratic = 0.20f;
}
