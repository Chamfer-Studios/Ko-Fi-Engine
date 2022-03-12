#include "ComponentParticle.h"
#include "ParticleModule.h"
#include "Renderer3D.h"
#include "Engine.h"

#include "MathGeoLib/Math/float4x4.h"

ComponentParticle::ComponentParticle(GameObject* parent) : Component(parent)
{
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
	EmitterInstance* eI = new EmitterInstance(e, this);
	emitterInstances.push_back(eI);
	colorToAdd = FadeColor(Color(0.0f, 0.0f, 0.0f, 255.0f), 0.5f);
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

	if (ImGui::CollapsingHeader("Particle System"))
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
					if (ImGui::CollapsingHeader("Default"))
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
					if (ImGui::CollapsingHeader("Movement"))
					{
						EmitterMovement* e = (EmitterMovement*)module;
						float initialIntensity = e->initialIntensity;
						if (ImGui::InputFloat("InitialIntensity: ", &initialIntensity, 0.0f, 100.0f))
						{
							e->initialIntensity = initialIntensity;
						}
						float finalIntensity = e->finalIntensity;
						if (ImGui::InputFloat("FinalIntensity: ", &finalIntensity, 0.0f, 25.0f))
						{
							e->finalIntensity = finalIntensity;
						}

						ImGui::Separator();

						float initialDirection[4] = { e->initialDirection.x,e->initialDirection.y,e->initialDirection.z,1.0f };
						if (ImGui::DragFloat3("initialDirection", initialDirection, 0.1f, -10000.0f, 10000.0f))
						{
							e->initialDirection = { initialDirection[0],initialDirection[1],initialDirection[2] };
						}
						float finalDirection[4] = { e->finalDirection.x,e->finalDirection.y,e->finalDirection.z,1.0f };
						if (ImGui::DragFloat3("finalDirection", finalDirection, 0.1f, -10000.0f, 10000.0f))
						{
							e->finalDirection = { finalDirection[0],finalDirection[1],finalDirection[2] };
						}

						ImGui::Separator();

						float initialPosition[4] = { e->initialPosition.x,e->initialPosition.y,e->initialPosition.z,1.0f };
						if (ImGui::DragFloat3("initialPosition", initialPosition, 0.1f, -10000.0f, 10000.0f))
						{
							e->initialPosition = { initialPosition[0],initialPosition[1],initialPosition[2] };
						}
						float finalPosition[4] = { e->finalPosition.x,e->finalPosition.y,e->finalPosition.z,1.0f };
						if (ImGui::DragFloat3("finalPosition", finalPosition, 0.1f, -10000.0f, 10000.0f))
						{
							e->finalPosition = { finalPosition[0],finalPosition[1],finalPosition[2] };
						}

						ImGui::Separator();

						float initialAcceleration[4] = { e->initialAcceleration.x,e->initialAcceleration.y,e->initialAcceleration.z,1.0f };
						if (ImGui::DragFloat3("initialAcceleration", initialAcceleration, 0.1f, -10000.0f, 10000.0f))
						{
							e->initialAcceleration = { initialAcceleration[0],initialAcceleration[1],initialAcceleration[2] };
						}
						float finalAcceleration[4] = { e->finalAcceleration.x,e->finalAcceleration.y,e->finalAcceleration.z,1.0f };
						if (ImGui::DragFloat3("finalAcceleration", finalAcceleration, 0.1f, -10000.0f, 10000.0f))
						{
							e->finalAcceleration = { finalAcceleration[0],finalAcceleration[1],finalAcceleration[2] };
						}
					}
					break;
				case ParticleModuleType::COLOR:
					if (ImGui::CollapsingHeader("Color"))
					{
						EmitterColor* e = (EmitterColor*)module;
						uint posList = 0u;
						for (auto color : e->colorOverTime)
						{
							ImVec4 vecColor = ImVec4(color.color.r, color.color.g, color.color.b, color.color.a);
							//if (ImGui::ColorButton("Color", vecColor, ImGuiColorEditFlags_None, ImVec2(100, 20)));


							//ImGui::SameLine();
							//ImGui::TextUnformatted("Color");
							//if (color.pos > 0)
							//{
								std::string colorStr = "Remove Color ";
								colorStr.append(std::to_string(color.pos));
								if (ImGui::Button(colorStr.c_str(), ImVec2(125, 25))) ret = false;
							//}

							ImGui::ColorEdit4("Color", &color.color.a, ImGuiColorEditFlags_AlphaBar);
							//e->EditColor((*color));
							//if (color == e->colorOverTime.begin())
							//{
							//	if (!e->EditColor((*color))) break;
							//}
							//else
							//{
							//	if (!e->EditColor((*color), posList)) e->colorOverTime.erase(++color);
							//}
							++posList;
						}
						if (ImGui::CollapsingHeader("Add Color"))
						{
							float color[4] = { colorToAdd.color.r,colorToAdd.color.g,colorToAdd.color.b,colorToAdd.color.a };
							if (ImGui::DragFloat4("color", color, 0.1f, -10000.0f, 10000.0f))
							{
								colorToAdd.color = Color(color[0], color[1], color[2], color[3]);
							}
							float pos = colorToAdd.pos;
							if (ImGui::InputFloat("position: ", &pos, 0.0f, 25.0f))
							{
								colorToAdd.pos = pos;
							}
							if (ImGui::Button("Add"))
							{
								e->colorOverTime.push_back(colorToAdd);
								colorToAdd = FadeColor(Color(0.0f, 0.0f, 0.0f, 255.0f), 0.5f);
							}
						}
					}
					break;
				case ParticleModuleType::SIZE:
					if (ImGui::CollapsingHeader("Size"))
					{
						EmitterSize* e = (EmitterSize*)module;
						float initialSize[4] = { e->initialSize.x,e->initialSize.y,e->initialSize.z,1.0f };
						if (ImGui::DragFloat3("initialSize", initialSize, 0.1f, -10000.0f, 10000.0f))
						{
							e->initialSize = { initialSize[0],initialSize[1],initialSize[2] };
						}
						float finalSize[4] = { e->finalSize.x,e->finalSize.y,e->finalSize.z,1.0f };
						if (ImGui::DragFloat3("finalSize", finalSize, 0.1f, -10000.0f, 10000.0f))
						{
							e->finalSize = { finalSize[0],finalSize[1],finalSize[2] };
						}
					}
					break;
				default:
					break;
				}

			}
			ImGui::Combo("##combo", &moduleToAdd, "Add Module\0Movement\0Color\0Size");

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