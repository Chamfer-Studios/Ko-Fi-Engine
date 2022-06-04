#include "C_LightSource.h"

// Modules
#include "Engine.h"
#include "M_SceneManager.h"

// GameObject
#include "GameObject.h"
#include "C_Transform.h"
#include "C_Camera.h"
#include "C_Material.h"
#include "R_Material.h"
#include "Globals.h"
#include "SceneIntro.h"
#include "M_Renderer3D.h"

#include "FSDefs.h"

C_LightSource::C_LightSource(GameObject* parent) : Component(parent)
{
	type = ComponentType::LIGHT_SOURCE;
	sourceType = SourceType::DIRECTIONAL;
	DirectionalLight* dLight = new DirectionalLight();
	lightSource = (LightSource*)dLight;
	numOfDirectional++;
	lightSource->position = owner->GetTransform()->GetPosition();
	shadowCam = nullptr;
}

C_LightSource::~C_LightSource()
{
	CleanUp();
}

bool C_LightSource::Start()
{
	shadowCam = owner->GetComponent<C_Camera>();
	if (shadowCam == nullptr)
	{
		shadowCam = owner->CreateComponent<C_Camera>();
		shadowCam->SetProjectionType(C_Camera::CameraType::KOFI_ORTHOGRAPHIC);
		//make the cam look in the direction of the light rays
		shadowCam->LookAt(shadowCam->GetPosition() + ((DirectionalLight*)lightSource)->direction);

		shadowCam->SetIsSphereCullingActive(false);
		shadowCam->SetIsFrustumActive(false);
	}

	return true;
}

bool C_LightSource::Update(float dt)
{
	
	return true;
}

bool C_LightSource::PostUpdate(float dt)
{
	lightSource->position = owner->GetTransform()->GetPosition();
	if (sourceType == SourceType::DIRECTIONAL)
	{
		//Keep the direction of the camera updated. Maybe this does not work because it sets itself back to match the transform
		shadowCam->LookAt(shadowCam->GetPosition() + ((DirectionalLight*)lightSource)->direction);
		((DirectionalLight*)lightSource)->lightSpaceMatrix = shadowCam->GetCameraFrustum().ViewProjMatrix();

	}
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
	json["type"] = (int)type;

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
	case SourceType::FOCAL:
	{
		FocalLight* currentLight = (FocalLight*)lightSource;

		json["lightPosition"] = { currentLight->position.x, currentLight->position.y, currentLight->position.z };
		json["lightColor"] = { currentLight->color.x, currentLight->color.y, currentLight->color.z };

		json["ambientValue"] = currentLight->ambient;
		json["diffuseValue"] = currentLight->diffuse;
		json["specularValue"] = currentLight->specular;

		json["cutOffAngle"] = currentLight->cutOffAngle;
		json["lightDirection"] = { currentLight->lightDirection.x, currentLight->lightDirection.y, currentLight->lightDirection.z };

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
	case SourceType::FOCAL:
	{
		FocalLight* currentLight = (FocalLight*)ChangeSourceType(type);
		std::vector<float> values = json.at("lightPosition").get<std::vector<float>>();
		currentLight->position = (float3(values[0], values[1], values[2]));
		values.clear();

		values = json.at("lightColor").get<std::vector<float>>();
		currentLight->color = (float3(values[0], values[1], values[2]));
		values.clear();

		currentLight->ambient = json.at("ambientValue");
		currentLight->diffuse = json.at("diffuseValue");
		currentLight->specular = json.at("specularValue");

		currentLight->cutOffAngle = json.at("cutOffAngle");

		values = json.at("lightDirection").get<std::vector<float>>();
		currentLight->lightDirection = (float3(values[0], values[1], values[2]));
		values.clear();

		currentLight->constant = json.at("constantAttenuationValue");
		currentLight->linear = json.at("linearAttenuationValue");
		currentLight->quadratic = json.at("quadraticAttenuationValue");

		break;
	}
	}
	sType = (int)sourceType;
}

bool C_LightSource::InspectorDraw(PanelChooser* chooser)
{
	bool ret = true; // TODO: We don't need it to return a bool... Make it void when possible.

	if (ImGui::CollapsingHeader("Component LightSource", ImGuiTreeNodeFlags_AllowItemOverlap))
	{	
		if (DrawDeleteButton(owner, this))
			return true;

		
		ImGui::Combo("###combo", &sType, "Directional Light Source\0Point Light Source\0Focal Light Source");

		ImGui::SameLine();

		if ((ImGui::Button("Assign Type")))
		{
			switch (sType)
			{
			case (int)SourceType::DIRECTIONAL: ChangeSourceType((SourceType)sType); break;
			case (int)SourceType::POINT: ChangeSourceType((SourceType)sType); break;
			case (int)SourceType::FOCAL: ChangeSourceType((SourceType)sType); break;
			}
			sType = (int)sourceType;
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
				//TODO change light direction
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
			
			ImGui::Image((ImTextureID)owner->GetEngine()->GetRenderer()->depthMapTexture, ImVec2(512, 512));

			if ((ImGui::Button("Activate Shadow Cast")))
			{
				CastShadows();
				owner->GetEngine()->GetSceneManager()->GetCurrentScene()->SetShadowCaster(owner);
			}

			//depth map resolution levels default 4096px x 4096px
			//if (owner == owner->GetEngine()->GetSceneManager()->GetCurrentScene()->GetShadowCaster())
			//{
			//	float depthMapResolution = owner->GetEngine()->GetRenderer()->depthMapResolution;
			//	if (ImGui::DragFloat("Depth Map Resolution", &depthMapResolution, 0.1f, 0.0f, 1.0f, "%.1f"))
			//	{
			//		owner->GetEngine()->GetRenderer()->depthMapResolution = depthMapResolution;
			//	}
			//}

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
			/*float ambientValue = currentLight->ambient;
			if (ImGui::DragFloat("Ambient Light Value", &ambientValue, 0.1f, 0.0f, 1.0f, "%.1f"))
			{
				currentLight->ambient = ambientValue;
			}*/
			float diffuseValue = currentLight->diffuse;
			if (ImGui::DragFloat("Diffuse Light Value", &diffuseValue, 0.1f, 0.0f, 1.0f, "%.1f"))
			{
				currentLight->diffuse = diffuseValue;
			}

			float constantValue = currentLight->constant;
			if (ImGui::DragFloat("General Attenuation", &constantValue, 0.1f, 0.0f, 1.0f, "%.1f"))
			{
				currentLight->constant = constantValue;
			}
			
			ImGui::Spacing();
			ImGui::Text("    Advanced attenuation parameters");
			ImGui::Spacing();

			float linearValue = currentLight->linear;
			if (ImGui::DragFloat("(0.0 - 0.3) Mid-rage Attenuation", &linearValue, 0.01f, 0.0f, .3f, "%.2f"))
			{
				currentLight->linear = linearValue;
			}
			float quadraticValue = currentLight->quadratic;
			if (ImGui::DragFloat("(0.0 - 0.2) Long range Attenuation", &quadraticValue, 0.01f, 0.0f, .2f, "%.2f"))
			{
				currentLight->quadratic = quadraticValue;
			}
			break;
		} 
		case SourceType::FOCAL:
		{
			FocalLight* currentLight = (FocalLight*)lightSource;

			float color[3] = { currentLight->color.x, currentLight->color.y, currentLight->color.z };
			if (ImGui::DragFloat3("light Color", color, 0.01f, 0.0f, 1.0f, "%.1f"))
			{
				currentLight->color = { color[0], color[1], color[2] };
			}
			/*float ambientValue = currentLight->ambient;
			if (ImGui::DragFloat("Ambient Light Value", &ambientValue, 0.1f, 0.0f, 1.0f, "%.1f"))
			{
				currentLight->ambient = ambientValue;
			}*/
			float diffuseValue = currentLight->diffuse;
			if (ImGui::DragFloat("Diffuse Light Value", &diffuseValue, 0.1f, 0.0f, 9999.0f, "%.1f"))
			{
				currentLight->diffuse = diffuseValue;
			}

			float cutOffValue = (acos(currentLight->cutOffAngle)) * RADTODEG;
			if (ImGui::DragFloat("Light Cone Angle", &cutOffValue, 0.1f, 0.0f, 180.0f, "%.1f"))
			{
				currentLight->cutOffAngle = cos(cutOffValue * DEGTORAD);
			}

			float range = currentLight->range;
			if (ImGui::DragFloat("Light Cone Range", &range, 0.1f))
			{
				currentLight->range = range;
			}

			float direction[3] = { currentLight->lightDirection.x, currentLight->lightDirection.y, currentLight->lightDirection.z };
			if (ImGui::DragFloat3("Light Cone Direction", direction, 0.1f, -10000.0f, 10000.0f, "%.1f"))
			{
				currentLight->lightDirection = { direction[0], direction[1], direction[2] };
			}

			float constantValue = currentLight->constant;
			if (ImGui::DragFloat("General Attenuation", &constantValue, 0.1f, 0.0f, 1.0f, "%.1f"))
			{
				currentLight->constant = constantValue;
			}

			ImGui::Spacing();
			ImGui::Text("    Advanced attenuation parameters");
			ImGui::Spacing();

			float linearValue = currentLight->linear;
			if (ImGui::DragFloat("(0.0 - 0.3) Mid-rage Attenuation", &linearValue, 0.01f, 0.0f, .3f, "%.2f"))
			{
				currentLight->linear = linearValue;
			}
			float quadraticValue = currentLight->quadratic;
			if (ImGui::DragFloat("(0.0 - 0.2) Long range Attenuation", &quadraticValue, 0.001f, 0.0f, .2f, "%.3f"))
			{
				currentLight->quadratic = quadraticValue;
			}
			break;
		}
		} 
		ImGui::NewLine();
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

			shadowCam = owner->GetComponent<C_Camera>();
			if (shadowCam == nullptr)
			{
				shadowCam = owner->CreateComponent<C_Camera>();
				shadowCam->SetProjectionType(C_Camera::CameraType::KOFI_ORTHOGRAPHIC);
				
				//make the cam look in the direction of the light rays
				float3 tmpRight = float3(1.0f, 0.0f, 0.0f).Cross(((DirectionalLight*)lightSource)->direction);
				shadowCam->LookAt(shadowCam->GetPosition() + ((DirectionalLight*)lightSource)->direction);
			}
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

			shadowCam = owner->GetComponent<C_Camera>();
			if (shadowCam != nullptr)
			{
				owner->DeleteComponent(shadowCam);
			}
		}
		else
			CONSOLE_LOG("[C_LightSource]: MAX of point lights reached");
		break;
	}
	case SourceType::FOCAL:
	{
		if (numOfFocal < MAX_FOCAL_LIGHTS)
		{
			FocalLight* fLight = new FocalLight();
			lightSource = (LightSource*)fLight;
			sourceType = type;
			numOfFocal++;

			shadowCam = owner->GetComponent<C_Camera>();
			if (shadowCam != nullptr)
			{
				owner->DeleteComponent(shadowCam);
			}
		}
		else
			CONSOLE_LOG("[C_LightSource]: MAX of focal lights reached");
		break;
	}
	}

	return lightSource;
}

void C_LightSource::SetDirection(float3 direction)
{
	try {
		FocalLight* f = (FocalLight*)lightSource;
		f->lightDirection = direction;
	}
	catch (...) {

	}
}

void C_LightSource::SetRange(float range)
{
	try {
		FocalLight* f = (FocalLight*)lightSource;
		f->range = range;
	}
	catch (...) {

	}
}

void C_LightSource::SetAngle(float angle)
{
	try {
		FocalLight* f = (FocalLight*)lightSource;
		f->cutOffAngle = cos(angle * DEGTORAD);;
	}
	catch (...) {

	}
}

//function to make the light source to cast shadows (currently only one shadow caster allowed)
void C_LightSource::CastShadows()
{
	if (!shadowCam)
	{
		CONSOLE_LOG("shadowCam is null");
		return;
	}

	C_Material* cMat = nullptr;
	R_Material* rMat = nullptr;

	if (!owner->GetComponent<C_Material>())
	{
		cMat = owner->CreateComponent<C_Material>();
		rMat = new R_Material();

	}
	else
	{
		cMat = owner->GetComponent<C_Material>();
		rMat = cMat->GetMaterial();
	}

	//make the material to have the depth shader. This will allow accessing it from the renderer.
	std::string shaderPath = ASSETS_SHADERS_DIR;
	shaderPath = shaderPath + "simple_depth_shader" + SHADER_EXTENSION;
	rMat->SetAssetPath(shaderPath.c_str());
	
	Importer::GetInstance()->materialImporter->LoadAndCreateShader(rMat->GetAssetPath(), rMat);
	cMat->SetMaterial(rMat);

	owner->GetEngine()->GetSceneManager()->GetCurrentScene()->SetShadowCaster(owner);
}

LightSource::LightSource()
{
	position = float3::zero;
	color = float3(1.0f, 1.0f, 1.0f);

	ambient = 0.5;
	diffuse = 0.8;
	specular = 0;
}

DirectionalLight::DirectionalLight() : LightSource()
{
	direction = float3(1.0f, 1.0f, 0.0f);
}

PointLight::PointLight() : LightSource()
{
	ambient = 0.0f;
	constant = 1.00f;
	linear = 0.020f;
	quadratic = 0.020f;
}

FocalLight::FocalLight() : LightSource()
{
	cutOffAngle = 0.965f; //cosinus of 15�
	range = 200.0f;
	lightDirection = float3(0.0f, 1.0f, 0.0f);
	ambient = 0.0f;
	constant = 1.00f;
	linear = 0.020f;
	quadratic = 0.02f;
}
