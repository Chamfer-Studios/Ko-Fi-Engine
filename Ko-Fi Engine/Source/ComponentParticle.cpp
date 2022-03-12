#include "ComponentParticle.h"
#include "ParticleModule.h"
#include "Renderer3D.h"
#include "Engine.h"
#include "Log.h"

#include "MathGeoLib/Math/float4x4.h"

ComponentParticle::ComponentParticle(GameObject* parent) : Component(parent)
{
	type = ComponentType::PARTICLE;

	emitterInstances.clear();
	emitters.clear();
}

ComponentParticle::~ComponentParticle()
{
}

bool ComponentParticle::Start()
{
	Emitter* e = new Emitter();
	emitters.push_back(e);
	return true;
}

bool ComponentParticle::Update(float dt)
{
	// SHOULD I DO THIS?????
	//for (std::vector<Emitter*>::iterator it = emitters.begin(); it < emitters.end(); ++it)
	//{
	//	(*it)->Update(dt);
	//}

	for (auto it : emitterInstances)
	{
		it->Update(dt);
		for (auto particle : it->particles)
		{
			owner->GetEngine()->GetRenderer()->AddParticle( nullptr, particle->CurrentColor,
				float4x4::FromTRS(particle->position, particle->rotation, particle->scale), 
				particle->distanceToCamera);
		}
	}

	return true;
}

bool ComponentParticle::PostUpdate(float dt)
{
	return true;
}

bool ComponentParticle::CleanUp()
{
	//DELETE TEXTURES

	for (std::vector<Emitter*>::iterator it = emitters.begin(); it < emitters.end(); ++it)
	{
		it = emitters.erase(it);
	}

	for (std::vector<EmitterInstance*>::iterator it = emitterInstances.begin(); it < emitterInstances.end(); ++it)
	{
		it = emitterInstances.erase(it);
	}

	return true;
}

bool ComponentParticle::InspectorDraw(PanelChooser* chooser)
{
	bool ret = true;

	if (ImGui::CollapsingHeader("Particle System", ImGuiTreeNodeFlags_DefaultOpen))
	{
		ImGui::Text("Emitters: %d", emitters.size());

		ImGui::Separator();

		for (Emitter* emitter : emitters)
		{
			ImGui::Text("%s", emitter->name.c_str());
			ImGui::Text("Modules: %d", emitter->modules.size());

			ImGui::Separator();

			for (ParticleModule* module : emitter->modules)
			{
				switch (module->type)
				{
				case ParticleModuleType::DEFAULT:
					if (ImGui::CollapsingHeader("Default", ImGuiTreeNodeFlags_DefaultOpen))
					{
						EmitterDefault* e = (EmitterDefault*)module;
						float timer = e->timer;
						if (ImGui::InputFloat("Timer: ", &timer, 0.0f, 100.0f))
						{
							e->timer = timer;
						}
						float spawnTime = e->spawnTime;
						if (ImGui::InputFloat("SpawnTime: ", &spawnTime, 0.0f, 25.0f))
						{
							e->spawnTime = spawnTime;
						}
						float initialLifetime = e->initialLifetime;
						if (ImGui::InputFloat("InitialLifetime: ", &initialLifetime, 0.0f, 60.0f))
						{
							e->initialLifetime = initialLifetime;
						}
					}
					break;
				case ParticleModuleType::MOVEMENT:
					if (ImGui::CollapsingHeader("Movement", ImGuiTreeNodeFlags_DefaultOpen))
					{
						EmitterMovement* e = (EmitterMovement*)module;

						float initialIntensity = e->initialIntensity;
						if (ImGui::DragFloat("InitialIntensity: ", &initialIntensity,0.1f, 0.0f, 100.0f,"%.1f"))
						{
							e->initialIntensity = initialIntensity;
						}
						float finalIntensity = e->finalIntensity;
						if (ImGui::DragFloat("FinalIntensity: ", &finalIntensity,0.1f, 0.0f, 25.0f,"%.1f"))
						{
							e->finalIntensity = finalIntensity;
						}

						ImGui::Separator();

						float initialDirection[3] = { e->initialDirection.x,e->initialDirection.y,e->initialDirection.z };
						if (ImGui::DragFloat3("initialDirection", initialDirection, 0.1f, -10000.0f, 10000.0f, "%.1f"))
						{
							e->initialDirection = { initialDirection[0],initialDirection[1],initialDirection[2] };
						}
						float finalDirection[3] = { e->finalDirection.x,e->finalDirection.y,e->finalDirection.z };
						if (ImGui::DragFloat3("finalDirection", finalDirection, 0.1f, -10000.0f, 10000.0f, "%.1f"))
						{
							e->finalDirection = { finalDirection[0],finalDirection[1],finalDirection[2] };
						}

						ImGui::Separator();

						float initialPosition[3] = { e->initialPosition.x,e->initialPosition.y,e->initialPosition.z };
						if (ImGui::DragFloat3("initialPosition", initialPosition, 0.1f, -10000.0f, 10000.0f, "%.1f"))
						{
							e->initialPosition = { initialPosition[0],initialPosition[1],initialPosition[2] };
						}
						float finalPosition[3] = { e->finalPosition.x,e->finalPosition.y,e->finalPosition.z };
						if (ImGui::DragFloat3("finalPosition", finalPosition, 0.1f, -10000.0f, 10000.0f, "%.1f"))
						{
							e->finalPosition = { finalPosition[0],finalPosition[1],finalPosition[2] };
						}

						ImGui::Separator();

						float initialAcceleration[3] = { e->initialAcceleration.x,e->initialAcceleration.y,e->initialAcceleration.z };
						if (ImGui::DragFloat3("initialAcceleration", initialAcceleration, 0.1f, -10000.0f, 10000.0f, "%.1f"))
						{
							e->initialAcceleration = { initialAcceleration[0],initialAcceleration[1],initialAcceleration[2] };
						}
						float finalAcceleration[3] = { e->finalAcceleration.x,e->finalAcceleration.y,e->finalAcceleration.z };
						if (ImGui::DragFloat3("finalAcceleration", finalAcceleration, 0.1f, -10000.0f, 10000.0f, "%.1f"))
						{
							e->finalAcceleration = { finalAcceleration[0],finalAcceleration[1],finalAcceleration[2] };
						}
					}
					break;
				case ParticleModuleType::COLOR:
					if (ImGui::CollapsingHeader("Color", ImGuiTreeNodeFlags_DefaultOpen))
					{
						EmitterColor* e = (EmitterColor*)module;
						ImGui::Text("Colors: %d", e->colorOverTime.size());

						ImGui::Separator();

						int posList = 0;
						for (std::vector<FadeColor>::iterator color = e->colorOverTime.begin(); color < e->colorOverTime.end(); ++color)
						{
							InspectorDrawColor((*color), posList);
							++posList;
						}
						if (ImGui::Button("Add Color"))
						{
							e->colorOverTime.push_back(FadeColor(Color(0.0f, 0.0f, 0.0f, 1.0f), 0.5f));
						}
					}
					break;
				case ParticleModuleType::SIZE:
					if (ImGui::CollapsingHeader("Size", ImGuiTreeNodeFlags_DefaultOpen))
					{
						EmitterSize* e = (EmitterSize*)module;
						float initialSize[3] = { e->initialSize.x,e->initialSize.y,e->initialSize.z };
						if (ImGui::DragFloat3("initialSize", initialSize, 0.1f, -10000.0f, 10000.0f,"%.1f"))
						{
							e->initialSize = { initialSize[0],initialSize[1],initialSize[2] };
						}
						float finalSize[3] = { e->finalSize.x,e->finalSize.y,e->finalSize.z };
						if (ImGui::DragFloat3("finalSize", finalSize, 0.1f, -10000.0f, 10000.0f,"%f.1f"))
						{
							e->finalSize = { finalSize[0],finalSize[1],finalSize[2] };
						}
					}
					break;
				default:
					break;
				}

			}
			ImGui::Combo("##modules", &moduleToAdd, "Add Module\0Movement\0Color\0Size");

			ImGui::SameLine();

			if ((ImGui::Button("ADD")))
			{
				++moduleToAdd;
				if (moduleToAdd != (int)ComponentType::NONE)
				{
					emitter->AddModuleByType((ParticleModuleType)moduleToAdd);
					moduleToAdd = 0;
				}
			}

			ImGui::Separator();
		}
		if (ImGui::Button("Add Emitter"))
		{
			Emitter* e = new Emitter();
			emitters.push_back(e);
		}
	}
	return ret;
}

void ComponentParticle::InspectorDrawColor(FadeColor& color,int index)
{
	float c[4] = { color.color.r,color.color.g,color.color.b,color.color.a };
	std::string colorName = "Color " + std::to_string(index);
	if (ImGui::ColorEdit4(colorName.c_str(), c))
	{
		color.color.r = c[0];
		color.color.g = c[1];
		color.color.b = c[2];
		color.color.a = c[3];
	}
	float p = color.pos;
	std::string positionName = "Position " + std::to_string(index);
	if (ImGui::DragFloat(positionName.c_str(), &p, 0.005f, 0.0f, 1.0f, "%.3f"))
	{
		color.pos = p;
	}
}

void ComponentParticle::ClearParticles()
{
	for (std::vector<EmitterInstance*>::iterator it = emitterInstances.begin(); it < emitterInstances.end(); ++it)
	{
		(*it)->KillParticles();
	}
}

void ComponentParticle::StopParticleSpawn()
{
	for (std::vector<EmitterInstance*>::iterator it = emitterInstances.begin(); it < emitterInstances.end(); ++it)
	{
		(*it)->SetParticleEmission(false);
	}
}

void ComponentParticle::ResumeParticleSpawn()
{
	for (std::vector<EmitterInstance*>::iterator it = emitterInstances.begin(); it < emitterInstances.end(); ++it)
	{
		(*it)->SetParticleEmission(true);
	}
}