#include "ComponentParticle.h"
#include "ParticleModule.h"
#include "Renderer3D.h"
#include "Engine.h"
#include "Log.h"
#include "imgui_stdlib.h"
#include "Texture.h"
#include "PanelChooser.h"
#include "ParticleResource.h"

#include "MathGeoLib/Math/float4x4.h"

ComponentParticle::ComponentParticle(GameObject* parent) : Component(parent)
{
	type = ComponentType::PARTICLE;
	resource = new ParticleResource();
	emitterInstances.clear();
}

ComponentParticle::~ComponentParticle()
{

}

bool ComponentParticle::Start()
{
	if (resource != nullptr)
	{
		Emitter* e = new Emitter();
		resource->emitters.push_back(e);
		EmitterInstance* eI = new EmitterInstance(e, this);
		emitterInstances.push_back(eI);
		eI->Init();
	}

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
		it->Update(dt); //kill inactive and update emitter instances
		
		for (unsigned int i = 0; i < it->activeParticles; i++)
		{
			unsigned int particleIndex = it->particleIndices[i];
			Particle* particle = &it->particles[particleIndex];
		
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

	for (std::vector<EmitterInstance*>::iterator it = emitterInstances.begin(); it < emitterInstances.end(); ++it)
	{
		it = emitterInstances.erase(it);
	}

	//TODO: refs -1
	resource->CleanUp();
	RELEASE(resource);

	return true;
}

bool ComponentParticle::InspectorDraw(PanelChooser* chooser)
{
	bool ret = true;

	if (ImGui::CollapsingHeader("Particle System", ImGuiTreeNodeFlags_DefaultOpen))
	{
		if (resource != nullptr)
		{
			ImGui::Text("Resource Name:");
			ImGui::SameLine();
			ImGui::InputText("##Resource",&(resource->name));

			ImGui::Text("Emitters: %d", resource->emitters.size());

			std::string addEmitterName = "Add Emitter to " + resource->name;
			ImGui::Combo("##Emitter Combo", &emitterToAdd, "Add Emitter\0New Emitter");
			ImGui::SameLine();
			if ((ImGui::Button(addEmitterName.c_str())))
			{
				//TODO: Check all emitters that are loaded
				if (emitterToAdd == 1)
				{
					CONSOLE_LOG("%d", emitterToAdd);
					std::string name = "Emitter";
					NewEmitterName(name);
					Emitter* e = new Emitter(name.c_str());
					resource->emitters.push_back(e);
					EmitterInstance* ei = new EmitterInstance(e, this);
					ei->Init();
					emitterInstances.push_back(ei);
				}
			}
			//what we had before, don't delete
			//ImGui::SameLine();

			////Combo to select either an existing resource or create a new one

			//if (ImGui::Button("Add Emitter")) 
			//{
			//	std::string name = "Emitter";
			//	NewEmitterName(name);
			//	Emitter* e = new Emitter(name.c_str());
			//	resource->emitters.push_back(e);
			//	EmitterInstance* ei = new EmitterInstance(e, this);
			//	ei->Init();
			//	emitterInstances.push_back(ei);
			//}

			ImGui::Text("Emitter Instances: %d", emitterInstances.size());

			if(resource->emitters.size() > 0)
				ImGui::Separator();

			int emitterAt = 1;
			for (Emitter* emitter : resource->emitters)
			{
				std::string emitterName = "##Emitter" + std::to_string(emitterAt);
				//std::string emitterNameDots = "Emitter " + std::to_string(emitterAt) + " Name:";
				ImGui::Text("Name:");
				ImGui::SameLine();
				ImGui::InputText(emitterName.c_str(), &(emitter->name));

				std::string changeTexture = "Change Texture to " + emitter->name;
				if (chooser->IsReadyToClose(changeTexture.c_str()))
				{
					if (chooser->OnChooserClosed() != nullptr)
					{
						std::string path = chooser->OnChooserClosed();
						if (emitter->texture.textureID == currentTextureId)
						{
							emitter->texture.textureID = -1;
							emitter->texture.SetTexturePath(nullptr);

							Texture tex = Texture();
							Importer::GetInstance()->textureImporter->Import(path.c_str(), &tex);
							emitter->texture = tex;
						}
					}
				}

				ImGui::Text("Material Texture:");
				if (emitter->texture.textureID != -1)
				{
					ImGui::Image((ImTextureID)emitter->texture.textureID, ImVec2(85, 85));
					ImGui::SameLine();
					ImGui::BeginGroup();
					ImGui::Text(emitter->texture.GetTexturePath());
					ImGui::PushID(emitter->texture.textureID << 8);

					std::string changeTexture = "Change Texture to " + emitter->name;
					if (ImGui::Button(changeTexture.c_str()))
					{
						chooser->OpenPanel(changeTexture.c_str(), "png");
						currentTextureId = emitter->texture.textureID;
					}

					ImGui::PopID();
					ImGui::PushID(emitter->texture.textureID << 16);

					std::string deleteTexture = "Delete Texture to " + emitter->name;
					if (ImGui::Button(deleteTexture.c_str()))
					{
						emitter->texture.textureID = -1;
						emitter->texture.SetTexturePath(nullptr);
					}
					ImGui::PopID();
					ImGui::EndGroup();
				}
				else
				{
					std::string addTexture = "Add Texture to " + emitter->name;
					if (ImGui::Button(addTexture.c_str()))
					{
						std::string changeTexture = "Change Texture to " + emitter->name;
						chooser->OpenPanel(changeTexture.c_str(), "png");
						currentTextureId = emitter->texture.textureID;
					}
				}

				emitterName.append("Modules");
				std::string addName = "Add Module to " + emitter->name;
				ImGui::Text("Modules: %d", emitter->modules.size());

				ImGui::Combo(emitterName.c_str(), &moduleToAdd, "Add Module\0Movement\0Color\0Size");
				ImGui::SameLine();
				if ((ImGui::Button(addName.c_str())))
				{
					++moduleToAdd;
					if (moduleToAdd != (int)ComponentType::NONE)
						emitter->AddModuleByType((ParticleModuleType)moduleToAdd);
				}

				for (ParticleModule* module : emitter->modules)
				{
					std::string moduleName = emitter->name;
					switch (module->type)
					{
					case ParticleModuleType::DEFAULT:
						moduleName.append(" Default");
						if (ImGui::CollapsingHeader(moduleName.c_str()))
						{
							EmitterDefault* e = (EmitterDefault*)module;
							float timer = e->timer;
							std::string timerName = emitter->name + " - Timer";
							if (ImGui::InputFloat(timerName.c_str(), &timer, 0.0f, 100.0f))
							{
								e->timer = timer;
							}
							float spawnTime = e->spawnTime;
							std::string spawnName = emitter->name + " - SpawnTime";
							if (ImGui::InputFloat(spawnName.c_str(), &spawnTime, 0.0f, 25.0f))
							{
								e->spawnTime = spawnTime;
							}
							float initialLifetime = e->initialLifetime;
							std::string lifetimeName = emitter->name + " - InitialLifetime";
							if (ImGui::InputFloat(lifetimeName.c_str(), &initialLifetime, 0.0f, 60.0f))
							{
								e->initialLifetime = initialLifetime;
							}
						}
						break;
					case ParticleModuleType::MOVEMENT:
						moduleName.append(" Movement");
						if (ImGui::CollapsingHeader(moduleName.c_str()))
						{
							EmitterMovement* e = (EmitterMovement*)module;
							float intensity[2] = { e->initialIntensity,e->finalIntensity };
							std::string intensityName = emitter->name + " - Intensity";
							if (ImGui::DragFloat2(intensityName.c_str(), intensity, 0.1f, 0.0f, 100.0f, "%.1f"))
							{
								e->initialIntensity = intensity[0];
								e->finalIntensity = intensity[1];
							}

							ImGui::Spacing();

							float initialDirection[3] = { e->initialDirection.x,e->initialDirection.y,e->initialDirection.z };
							std::string iDirectionName = emitter->name + " - InitialDirection";
							if (ImGui::DragFloat3(iDirectionName.c_str(), initialDirection, 0.1f, -10000.0f, 10000.0f, "%.1f"))
							{
								e->initialDirection = { initialDirection[0],initialDirection[1],initialDirection[2] };
							}
							float finalDirection[3] = { e->finalDirection.x,e->finalDirection.y,e->finalDirection.z };
							std::string fDirectionName = emitter->name + " - FinalDirection";
							if (ImGui::DragFloat3(fDirectionName.c_str(), finalDirection, 0.1f, -10000.0f, 10000.0f, "%.1f"))
							{
								e->finalDirection = { finalDirection[0],finalDirection[1],finalDirection[2] };
							}

							ImGui::Spacing();

							float initialPosition[3] = { e->initialPosition.x,e->initialPosition.y,e->initialPosition.z };
							std::string iPositionName = emitter->name + " - InitialPosition";
							if (ImGui::DragFloat3(iPositionName.c_str(), initialPosition, 0.1f, -10000.0f, 10000.0f, "%.1f"))
							{
								e->initialPosition = { initialPosition[0],initialPosition[1],initialPosition[2] };
							}
							float finalPosition[3] = { e->finalPosition.x,e->finalPosition.y,e->finalPosition.z };
							std::string fPositionName = emitter->name + " - FinalPosition";
							if (ImGui::DragFloat3(fPositionName.c_str(), finalPosition, 0.1f, -10000.0f, 10000.0f, "%.1f"))
							{
								e->finalPosition = { finalPosition[0],finalPosition[1],finalPosition[2] };
							}

							ImGui::Spacing();

							float initialAcceleration[3] = { e->initialAcceleration.x,e->initialAcceleration.y,e->initialAcceleration.z };
							std::string iAccelerationName = emitter->name + " - InitialAcceleration";
							if (ImGui::DragFloat3(iAccelerationName.c_str(), initialAcceleration, 0.1f, -10000.0f, 10000.0f, "%.1f"))
							{
								e->initialAcceleration = { initialAcceleration[0],initialAcceleration[1],initialAcceleration[2] };
							}
							float finalAcceleration[3] = { e->finalAcceleration.x,e->finalAcceleration.y,e->finalAcceleration.z };
							std::string fAccelerationName = emitter->name + " - FinalAcceleration";
							if (ImGui::DragFloat3(fAccelerationName.c_str(), finalAcceleration, 0.1f, -10000.0f, 10000.0f, "%.1f"))
							{
								e->finalAcceleration = { finalAcceleration[0],finalAcceleration[1],finalAcceleration[2] };
							}

							std::string deleteEmitter = emitter->name + " - Delete Module Movement";
							if (ImGui::Button(deleteEmitter.c_str()))
							{
								for (std::vector<ParticleModule*>::iterator it = emitter->modules.begin(); it < emitter->modules.end(); ++it)
								{
									if ((*it)->type == ParticleModuleType::MOVEMENT)
									{
										emitter->modules.erase(it);
										break;
									}
								}
							}
						}
						break;
					case ParticleModuleType::COLOR:
						moduleName.append(" Color");
						if (ImGui::CollapsingHeader(moduleName.c_str()))
						{
							EmitterColor* e = (EmitterColor*)module;
							ImGui::Text("Colors: %d", e->colorOverTime.size());

							ImGui::Spacing();

							int posList = 0;
							for (std::vector<FadeColor>::iterator color = e->colorOverTime.begin(); color < e->colorOverTime.end(); ++color)
							{
								InspectorDrawColor(emitter->name, (*color), posList);

								std::string deleteColor = emitter->name + " - Delete Color " + std::to_string(posList + 1);
								if (ImGui::Button(deleteColor.c_str()))
								{
									e->colorOverTime.erase(color);
									break;
								}

								if (color != e->colorOverTime.end())
								{
									ImGui::Spacing();
								}
								++posList;
							}

							std::string addColorName = "Add Color to " + emitter->name;
							if (ImGui::Button(addColorName.c_str()))
							{
								e->colorOverTime.push_back(FadeColor());
							}

							std::string deleteEmitter = emitter->name + " - Delete Module Color";
							if (ImGui::Button(deleteEmitter.c_str()))
							{
								for (std::vector<ParticleModule*>::iterator it = emitter->modules.begin(); it < emitter->modules.end(); ++it)
								{
									if ((*it)->type == ParticleModuleType::COLOR)
									{
										emitter->modules.erase(it);
										break;
									}
								}
							}
						}
						break;
					case ParticleModuleType::SIZE:
						moduleName.append(" Size");
						if (ImGui::CollapsingHeader(moduleName.c_str()))
						{
							EmitterSize* e = (EmitterSize*)module;
							float initialSize[3] = { e->initialSize.x,e->initialSize.y,e->initialSize.z };
							std::string iSizeName = emitter->name + " - InitialSize";
							if (ImGui::DragFloat3(iSizeName.c_str(), initialSize, 0.1f, -10000.0f, 10000.0f, "%.1f"))
							{
								e->initialSize = { initialSize[0],initialSize[1],initialSize[2] };
							}

							float finalSize[3] = { e->finalSize.x,e->finalSize.y,e->finalSize.z };
							std::string fSizeName = emitter->name + " - FinalSize";
							if (ImGui::DragFloat3(fSizeName.c_str(), finalSize, 0.1f, -10000.0f, 10000.0f, "%.1f"))
							{
								e->finalSize = { finalSize[0],finalSize[1],finalSize[2] };
							}

							std::string deleteEmitter = emitter->name + " - Delete Module Size";
							if (ImGui::Button(deleteEmitter.c_str()))
							{
								for (std::vector<ParticleModule*>::iterator it = emitter->modules.begin(); it < emitter->modules.end(); ++it)
								{
									if ((*it)->type == ParticleModuleType::SIZE)
									{
										emitter->modules.erase(it);
										break;
									}
								}
							}
						}
						break;
					default:
						break;
					}
				}

				std::string deleteEmitter = "Delete " + emitter->name;
				if (ImGui::Button(deleteEmitter.c_str()))
				{
					for (std::vector<Emitter*>::iterator it = resource->emitters.begin(); it < resource->emitters.end(); ++it)
					{
						if ((*it) == emitter)
						{
							for (std::vector<EmitterInstance*>::iterator yt = emitterInstances.begin(); yt < emitterInstances.end(); ++yt)
							{
								if ((*yt)->emitter == emitter)
								{
									emitterInstances.erase(yt);
								}
									
							}
							resource->emitters.erase(it);
							break;
						}
					}
				}

				if (resource->emitters.size() > 0 && emitter != resource->emitters.back())
					ImGui::Separator();

				++emitterAt;
			}
		}
		else
		{
			ImGui::Text("Resource is null!");
		}
	}
	return ret;
}

void ComponentParticle::InspectorDrawColor(std::string emitterName, FadeColor& color, int index)
{
	float c[4] = { color.color.r,color.color.g,color.color.b,color.color.a };
	std::string colorName = emitterName + " - Color " + std::to_string(index + 1);
	if (ImGui::ColorEdit4(colorName.c_str(), c))
	{
		color.color.r = c[0];
		color.color.g = c[1];
		color.color.b = c[2];
		color.color.a = c[3];
	}
	float p = color.pos;
	std::string positionName = emitterName + " - Position " + std::to_string(index + 1);
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

void ComponentParticle::NewEmitterName(std::string& name, int n)
{
	for (auto emitter : resource->emitters)
	{
		if (emitter->name == name)
		{
			++n;
			name = "Emitter " + std::to_string(n);
			NewEmitterName(name, n);
		}
	}
}

void ComponentParticle::Save(Json& json) const
{
	json["type"] = "particle";
	Json jsonResource;
	if (resource != nullptr)
	{
		jsonResource["name"] = resource->name;
		Json jsonEmitter;
		for (auto e : resource->emitters)
		{
			jsonEmitter["maxParticles"] = e->maxParticles;
			jsonEmitter["name"] = e->name;
			jsonEmitter["texture_path"] = e->texture.path;
			Json jsonModule;
			for (auto m : e->modules)
			{
				switch (m->type)
				{
				case ParticleModuleType::DEFAULT:
				{
					EmitterDefault* mDefault = (EmitterDefault*)m;
					jsonModule["type"] = (int)mDefault->type;
					jsonModule["timer"] = mDefault->timer;
					jsonModule["spawnTime"] = mDefault->spawnTime;
					jsonModule["initialLifetime"] = mDefault->initialLifetime;
					break;
				}
				case ParticleModuleType::MOVEMENT:
				{
					EmitterMovement* mMovement = (EmitterMovement*)m;
					jsonModule["type"] = (int)mMovement->type;
					jsonModule["initialIntensity"] = mMovement->initialIntensity;
					jsonModule["finalIntensity"] = mMovement->finalIntensity;
					jsonModule["initialDirection"] = { mMovement->initialDirection.x,mMovement->initialDirection.y,mMovement->initialDirection.z };
					jsonModule["finalDirection"] = { mMovement->finalDirection.x,mMovement->finalDirection.y,mMovement->finalDirection.z };
					jsonModule["initialPosition"] = { mMovement->initialPosition.x,mMovement->initialPosition.y,mMovement->initialPosition.z };
					jsonModule["finalPosition"] = { mMovement->finalPosition.x,mMovement->finalPosition.y,mMovement->finalPosition.z };
					jsonModule["initialAcceleration"] = { mMovement->initialAcceleration.x,mMovement->initialAcceleration.y,mMovement->initialAcceleration.z };
					jsonModule["finalAcceleration"] = { mMovement->finalAcceleration.x,mMovement->finalAcceleration.y,mMovement->finalAcceleration.z };
					break;
				}
				case ParticleModuleType::COLOR:
				{
					EmitterColor* mColor = (EmitterColor*)m;
					jsonModule["type"] = (int)mColor->type;
					Json jsonColor;
					for (auto c : mColor->colorOverTime)
					{
						jsonColor["color"] = { c.color.r,c.color.g,c.color.b,c.color.a };
						jsonColor["position"] = c.pos;
						jsonModule["colors"].push_back(jsonColor);
					}
					break;
				}
				case ParticleModuleType::SIZE:
				{
					EmitterSize* mSize = (EmitterSize*)m;
					jsonModule["type"] = (int)mSize->type;
					jsonModule["initialSize"] = { mSize->initialSize.x,mSize->initialSize.y,mSize->initialSize.z };
					jsonModule["finalSize"] = { mSize->finalSize.x,mSize->finalSize.y,mSize->finalSize.z };
					break;
				}
				default:
					break;
				}
				jsonEmitter["modules"].push_back(jsonModule);
				jsonModule.clear();
			}
			jsonResource["emitters"].push_back(jsonEmitter);
		}
		json["resource"].push_back(jsonResource);
	}
}

void ComponentParticle::Load(Json& json)
{
	if (!json.empty())
	{
		if (resource == nullptr)
			resource = new ParticleResource();

		resource->emitters.clear();
		for (const auto& r : json.at("resource").items())
		{
			//emitterInstances.clear();
			//Emitter* e = new Emitter(emitter.value().at("name").get<std::string>().c_str());
			//EmitterInstance* ei = new EmitterInstance(e, this);
			//emitterInstances.push_back(ei);
			//ei->Init();
			//e->maxParticles = emitter.value().at("maxParticles");
			//e->texture = Texture();
			//e->texture.path = emitter.value().at("texture_path");
			//if (e->texture.path != "")
			//	Importer::GetInstance()->textureImporter->Import(json.at("texture_path").get<std::string>().c_str(), &e->texture);

			//e->modules.clear();
			//for (const auto& pModule : emitter.value().at("modules").items())
			resource->name = r.value().at("name").get<std::string>();

			for (const auto& emitter : r.value().at("emitters").items())
			{
				emitterInstances.clear();
				Emitter* e = new Emitter(emitter.value().at("name").get<std::string>().c_str());
				EmitterInstance* ei = new EmitterInstance(e, this);
				emitterInstances.push_back(ei);
				ei->Init();
				e->maxParticles = emitter.value().at("maxParticles");
				e->texture = Texture();
				e->texture.path = emitter.value().at("texture_path").get<std::string>();
				if (e->texture.path != "")
					Importer::GetInstance()->textureImporter->Import(e->texture.path.c_str(), &e->texture);

				e->modules.clear();
				for (const auto& pModule : emitter.value().at("modules").items())
				{
					ParticleModule* m = nullptr;
					int type = pModule.value().at("type");
					switch (type)
					{
					case 1:
					{
						EmitterDefault* mDefault = new EmitterDefault();
						mDefault->timer = pModule.value().at("timer");
						mDefault->spawnTime = pModule.value().at("spawnTime");
						mDefault->initialLifetime = pModule.value().at("initialLifetime");
						m = mDefault;
						break;
					}
					case 2:
					{
						EmitterMovement* mMovement = new EmitterMovement();
						mMovement->initialIntensity = pModule.value().at("initialIntensity");
						mMovement->finalIntensity = pModule.value().at("finalIntensity");
						std::vector<float> values = pModule.value().at("initialDirection").get<std::vector<float>>();
						mMovement->initialDirection = { values[0],values[1],values[2] };
						values.clear();
						values = pModule.value().at("finalDirection").get<std::vector<float>>();
						mMovement->finalDirection = { values[0],values[1],values[2] };
						values.clear();
						values = pModule.value().at("initialPosition").get<std::vector<float>>();
						mMovement->initialPosition = { values[0],values[1],values[2] };
						values.clear();
						values = pModule.value().at("finalPosition").get<std::vector<float>>();
						mMovement->finalPosition = { values[0],values[1],values[2] };
						values.clear();
						values = pModule.value().at("initialAcceleration").get<std::vector<float>>();
						mMovement->initialAcceleration = { values[0],values[1],values[2] };
						values.clear();
						values = pModule.value().at("finalAcceleration").get<std::vector<float>>();
						mMovement->finalAcceleration = { values[0],values[1],values[2] };
						values.clear();
						m = mMovement;
						break;
					}
					case 3:
					{
						EmitterColor* mColor = new EmitterColor();
						mColor->colorOverTime.clear();
						for (const auto& c : pModule.value().at("colors").items())
						{
							FadeColor fc = FadeColor();
							std::vector<float> values = c.value().at("color").get<std::vector<float>>();
							fc.color = Color(values[0], values[1], values[2], values[3]);
							values.clear();
							fc.pos = c.value().at("position");
							mColor->colorOverTime.push_back(fc);
						}
						m = mColor;
						break;
					}
					case 4:
					{
						EmitterSize* mSize = new EmitterSize();
						std::vector<float> values = pModule.value().at("initialSize").get<std::vector<float>>();
						mSize->initialSize = { values[0],values[1],values[2] };
						values.clear();
						values = pModule.value().at("finalSize").get<std::vector<float>>();
						mSize->finalSize = { values[0],values[1],values[2] };
						values.clear();
						m = mSize;
						break;
					}
					default:
						break;
					}
					if (m != nullptr)
						e->modules.push_back(m);
				}
				resource->emitters.push_back(e);
			}
		}
	}
}