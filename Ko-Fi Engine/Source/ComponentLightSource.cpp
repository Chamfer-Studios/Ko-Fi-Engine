#include "ComponentLightSource.h"
#include "ComponentTransform.h"
#include "Globals.h"
#include "Engine.h"
#include "SceneManager.h"
#include "SceneIntro.h"

ComponentLightSource::ComponentLightSource(GameObject* parent) : Component(parent)
{
	type = ComponentType::LIGHT_SOURCE;
	sourceType = SourceType::DIRECTIONAL;
	DirectionalLight* dLight = new DirectionalLight();
	lightSource = (LightSource*)dLight;
	lightSource->position = owner->GetTransform()->GetPosition();
}

ComponentLightSource::~ComponentLightSource()
{
}

bool ComponentLightSource::Start()
{
	return true;
}

bool ComponentLightSource::Update(float dt)
{
	lightSource->position = owner->GetTransform()->GetPosition();

	return true;
}

bool ComponentLightSource::CleanUp()
{
	RELEASE(lightSource);

	return true;
}

void ComponentLightSource::Save(Json& json) const
{
}

void ComponentLightSource::Load(Json& json)
{
}

bool ComponentLightSource::InspectorDraw(PanelChooser* chooser)
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
			//dragfloat constantVal
			//dragfloat linearVal
			//dragfloat quadraticVal

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
	}
	return ret;
}

LightSource* ComponentLightSource::ChangeSourceType(SourceType type)
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
			CONSOLE_LOG("[ComponentLightSource]: MAX of directional lights reached");
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
			CONSOLE_LOG("[ComponentLightSource]: MAX of point lights reached");
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
			CONSOLE_LOG("[ComponentLightSource]: MAX of focal lights reached");*/
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
