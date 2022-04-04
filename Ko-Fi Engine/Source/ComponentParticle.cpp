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
	//resource = new ParticleResource();
	resource = nullptr;
	emitterInstances.clear();
}

ComponentParticle::~ComponentParticle()
{
	CleanUp();
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
		
			owner->GetEngine()->GetRenderer()->AddParticle(it->emitter->texture, particle->CurrentColor,
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

	for (std::vector<EmitterInstance*>::iterator it = emitterInstances.begin(); it != emitterInstances.end(); ++it)
	{
		emitterInstances.erase(it);
		if (emitterInstances.empty())
			break;
	}
	emitterInstances.clear();
	emitterInstances.shrink_to_fit();

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
			if ((ImGui::Button(addEmitterName.c_str())))
			{
				std::string name = "Emitter";
				NewEmitterName(name);
				Emitter* e = new Emitter(name.c_str());
				resource->emitters.push_back(e);
				EmitterInstance* ei = new EmitterInstance(e, this);
				ei->Init();
				emitterInstances.push_back(ei);
			}

			ImGui::Text("Emitter Instances: %d", emitterInstances.size());

			if(resource->emitters.size() > 0)
				ImGui::Separator();

			int emitterAt = 1;
			for (Emitter* emitter : resource->emitters)
			{
				std::string emitterName = "##Emitter" + std::to_string(emitterAt);
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
							emitter->texture.textureID = TEXTUREID_DEFAULT;
							emitter->texture.SetTexturePath(nullptr);

							Texture tex;
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
						chooser->OpenPanel(changeTexture.c_str(), "png", { "png","jpg","jpeg"});
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
						chooser->OpenPanel(changeTexture.c_str(), "png", { "png","jpg","jpeg" });
						currentTextureId = emitter->texture.textureID;
					}
				}

				emitterName.append("Modules");
				std::string addName = "Add Module to " + emitter->name;
				ImGui::Text("Modules: %d", emitter->modules.size());

				ImGui::Combo(emitterName.c_str(), &moduleToAdd, "Add Module\0Movement\0Color\0Size\0Billboarding");
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
							float spawnTimer = e->spawnTimer;
							std::string spawnTimerName = emitter->name + " - SpawnTimer";
							if (ImGui::InputFloat(spawnTimerName.c_str(), &spawnTimer, 0.0f, 100.0f))
							{
								e->spawnTimer = spawnTimer;
							}
							float spawnTime = e->spawnTime;
							std::string spawnTimeName = emitter->name + " - SpawnTime";
							if (ImGui::InputFloat(spawnTimeName.c_str(), &spawnTime, 0.0f, 25.0f))
							{
								e->spawnTime = spawnTime;
							}

							bool randomParticleLife = e->randomParticleLife;
							if (ImGui::Checkbox("Random Particle Life", &randomParticleLife))
							{
								e->randomParticleLife = randomParticleLife;
							}
							if (randomParticleLife)
							{
								float particleLife[2] = { e->minParticleLife,e->maxParticleLife };
								std::string particleLifeName = emitter->name + " - ParticleLife";
								if (ImGui::DragFloat2(particleLifeName.c_str(), particleLife, 0.1f, 0.0f, 1000.0f, "%.1f"))
								{
									e->minParticleLife = particleLife[0];
									e->maxParticleLife = particleLife[1];
								}
							}
							else
							{
								float particleLife = e->minParticleLife;
								std::string particleLifeName = emitter->name + " - ParticleLife";
								if (ImGui::DragFloat(particleLifeName.c_str(), &particleLife, 0.1f, 0.0f, 1000.0f, "%.1f"))
								{
									e->minParticleLife = particleLife;
								}
							}
						}
						break;
					case ParticleModuleType::MOVEMENT:
						moduleName.append(" Movement");
						if (ImGui::CollapsingHeader(moduleName.c_str()))
						{
							EmitterMovement* e = (EmitterMovement*)module;

							bool randomPosition = e->randomPosition;
							if (ImGui::Checkbox("Random Position", &randomPosition))
							{
								e->randomPosition = randomPosition;
							}
							if (randomPosition)
							{
								float minPosition[3] = { e->minPosition.x,e->minPosition.y,e->minPosition.z };
								std::string minPositionName = emitter->name + " - minPosition";
								if (ImGui::DragFloat3(minPositionName.c_str(), minPosition, 0.1f, -10000.0f, 10000.0f, "%.1f"))
								{
									e->minPosition = { minPosition[0],minPosition[1],minPosition[2] };
								}
								float maxPosition[3] = { e->maxPosition.x,e->maxPosition.y,e->maxPosition.z };
								std::string maxPositionName = emitter->name + " - maxPosition";
								if (ImGui::DragFloat3(maxPositionName.c_str(), maxPosition, 0.1f, -10000.0f, 10000.0f, "%.1f"))
								{
									e->maxPosition = { maxPosition[0],maxPosition[1],maxPosition[2] };
								}
							}
							else
							{
								float position[3] = { e->minPosition.x,e->minPosition.y,e->minPosition.z };
								std::string positionName = emitter->name + " - Position";
								if (ImGui::DragFloat3(positionName.c_str(), position, 0.1f, -10000.0f, 10000.0f, "%.1f"))
								{
									e->minPosition = { position[0],position[1],position[2] };
								}
							}

							ImGui::Spacing();

							bool randomDirection = e->randomDirection;
							if (ImGui::Checkbox("Random Direction", &randomDirection))
							{
								e->randomDirection = randomDirection;
							}
							if (randomDirection)
							{
								float minDirection[3] = { e->minDirection.x,e->minDirection.y,e->minDirection.z };
								std::string minDirectionName = emitter->name + " - minDirection";
								if (ImGui::DragFloat3(minDirectionName.c_str(), minDirection, 0.005f, -1.0f, 1.0f, "%.3f"))
								{
									e->minDirection = { minDirection[0],minDirection[1],minDirection[2] };
								}
								float maxDirection[3] = { e->maxDirection.x,e->maxDirection.y,e->maxDirection.z };
								std::string maxDirectionName = emitter->name + " - maxDirection";
								if (ImGui::DragFloat3(maxDirectionName.c_str(), maxDirection, 0.005f, -1.0f, 1.0f, "%.3f"))
								{
									e->maxDirection = { maxDirection[0],maxDirection[1],maxDirection[2] };
								}
							}
							else
							{
								float direction[3] = { e->minDirection.x,e->minDirection.y,e->minDirection.z };
								std::string directionName = emitter->name + " - Direction";
								if (ImGui::DragFloat3(directionName.c_str(), direction, 0.005f, -1.0f, 1.0f, "%.3f"))
								{
									e->minDirection = { direction[0],direction[1],direction[2] };
								}
							}

							ImGui::Spacing();

							bool randomVelocity = e->randomVelocity;
							if (ImGui::Checkbox("Random Velocity", &randomVelocity))
							{
								e->randomVelocity = randomVelocity;
							}
							if (randomVelocity)
							{
								float velocity[2] = { e->minVelocity,e->maxVelocity };
								std::string velocityName = emitter->name + " - Velocity";
								if (ImGui::DragFloat2(velocityName.c_str(), velocity, 0.1f, 0.0f, 1000.0f, "%.1f"))
								{
									e->minVelocity = velocity[0];
									e->maxVelocity = velocity[1];
								}
							}
							else
							{
								float velocity = e->minVelocity;
								std::string velocityName = emitter->name + " - Velocity";
								if (ImGui::DragFloat(velocityName.c_str(), &velocity, 0.1f, 0.0f, 1000.0f, "%.1f"))
								{
									e->minVelocity = velocity;
								}
							}

							ImGui::Spacing();

							bool randomAcceleration = e->randomAcceleration;
							if (ImGui::Checkbox("Random Acceleration", &randomAcceleration))
							{
								e->randomAcceleration = randomAcceleration;
							}
							if (randomAcceleration)
							{
								float minAcceleration[3] = { e->minAcceleration.x,e->minAcceleration.y,e->minAcceleration.z };
								std::string minAccelerationName = emitter->name + " - minAcceleration";
								if (ImGui::DragFloat3(minAccelerationName.c_str(), minAcceleration, 0.1f, -10000.0f, 10000.0f, "%.1f"))
								{
									e->minAcceleration = { minAcceleration[0],minAcceleration[1],minAcceleration[2] };
								}
								float maxAcceleration[3] = { e->maxAcceleration.x,e->maxAcceleration.y,e->maxAcceleration.z };
								std::string maxAccelerationName = emitter->name + " - maxAcceleration";
								if (ImGui::DragFloat3(maxAccelerationName.c_str(), maxAcceleration, 0.1f, -10000.0f, 10000.0f, "%.1f"))
								{
									e->maxAcceleration = { maxAcceleration[0],maxAcceleration[1],maxAcceleration[2] };
								}
							}
							else
							{
								float acceleration[3] = { e->minAcceleration.x,e->minAcceleration.y,e->minAcceleration.z };
								std::string accelerationName = emitter->name + " - Acceleration";
								if (ImGui::DragFloat3(accelerationName.c_str(), acceleration, 0.1f, -10000.0f, 10000.0f, "%.1f"))
								{
									e->minAcceleration = { acceleration[0],acceleration[1],acceleration[2] };
								}
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
							float minSize[3] = { e->minSize.x,e->minSize.y,e->minSize.z };
							std::string minSizeName = emitter->name + " - minSize";
							if (ImGui::DragFloat3(minSizeName.c_str(), minSize, 0.1f, -10000.0f, 10000.0f, "%.1f"))
							{
								e->minSize = { minSize[0],minSize[1],minSize[2] };
							}

							float maxSize[3] = { e->maxSize.x,e->maxSize.y,e->maxSize.z };
							std::string maxSizeName = emitter->name + " - maxSize";
							if (ImGui::DragFloat3(maxSizeName.c_str(), maxSize, 0.1f, -10000.0f, 10000.0f, "%.1f"))
							{
								e->maxSize = { maxSize[0],maxSize[1],maxSize[2] };
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
					case ParticleModuleType::BILLBOARDING:
						moduleName.append(" Billboarding");
						if (ImGui::CollapsingHeader(moduleName.c_str()))
						{
							ParticleBillboarding* particleBillboarding = (ParticleBillboarding*)module;

							ImGui::Combo("Billboarding##", &billboardingType, "Screen Aligned\0World Aligned\0X-Axis Aligned\0Y-Axis Aligned\0Z-Axis Aligned");
							ImGui::SameLine();
							if (ImGui::Button("SELECT"))
							{
								particleBillboarding->billboardingType = (ParticleBillboarding::BillboardingType)billboardingType;
							}

							bool hideModule = particleBillboarding->hideBillboarding;
							if (ImGui::Checkbox("Hide Billboarding", &hideModule))
							{
								particleBillboarding->hideBillboarding = hideModule;
							}

							std::string deleteEmitter = emitter->name + " - Delete Module Billboarding";
							if (ImGui::Button(deleteEmitter.c_str()))
							{
								for (std::vector<ParticleModule*>::iterator it = emitter->modules.begin(); it < emitter->modules.end(); ++it)
								{
									if ((*it)->type == ParticleModuleType::BILLBOARDING)
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

				ImGui::Spacing();
				ImGui::Spacing();

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
									break;
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
			std::string addResourceName = "Add Resource";
			ImGui::Combo("##Resource Combo", &resourceToAdd, "Add Resource\0New Resource");
			ImGui::SameLine();
			if ((ImGui::Button(addResourceName.c_str())))
			{
				if (resourceToAdd == 1)
				{
					resource = new ParticleResource();
					Emitter* e = new Emitter();
					resource->emitters.push_back(e);
					EmitterInstance* eI = new EmitterInstance(e, this);
					emitterInstances.push_back(eI);
					eI->Init();
				}
			}
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
					jsonModule["spawnTimer"] = mDefault->spawnTimer;
					jsonModule["spawnTime"] = mDefault->spawnTime;
					jsonModule["randomParticleLife"] = mDefault->randomParticleLife;
					jsonModule["minParticleLife"] = mDefault->minParticleLife;
					jsonModule["maxParticleLife"] = mDefault->maxParticleLife;
					break;
				}
				case ParticleModuleType::MOVEMENT:
				{
					EmitterMovement* mMovement = (EmitterMovement*)m;
					jsonModule["type"] = (int)mMovement->type;
					jsonModule["randPosition"] = mMovement->randomPosition;
					jsonModule["minPosition"] = { mMovement->minPosition.x,mMovement->minPosition.y,mMovement->minPosition.z };
					jsonModule["maxPosition"] = { mMovement->maxPosition.x,mMovement->maxPosition.y,mMovement->maxPosition.z };
					jsonModule["randVelocity"] = mMovement->randomVelocity;
					jsonModule["minVelocity"] = mMovement->minVelocity;
					jsonModule["maxVelocity"] = mMovement->maxVelocity;
					jsonModule["randDirection"] = mMovement->randomDirection;
					jsonModule["minDirection"] = { mMovement->minDirection.x,mMovement->minDirection.y,mMovement->minDirection.z };
					jsonModule["maxDirection"] = { mMovement->maxDirection.x,mMovement->maxDirection.y,mMovement->maxDirection.z };
					jsonModule["randAcceleration"] = mMovement->randomAcceleration;
					jsonModule["minAcceleration"] = { mMovement->minAcceleration.x,mMovement->minAcceleration.y,mMovement->minAcceleration.z };
					jsonModule["maxAcceleration"] = { mMovement->maxAcceleration.x,mMovement->maxAcceleration.y,mMovement->maxAcceleration.z };
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
					jsonModule["minSize"] = { mSize->minSize.x,mSize->minSize.y,mSize->minSize.z };
					jsonModule["maxSize"] = { mSize->maxSize.x,mSize->maxSize.y,mSize->maxSize.z };
					break;
				}
				case ParticleModuleType::BILLBOARDING:
				{
					ParticleBillboarding* mBillboarding = (ParticleBillboarding*)m;
					jsonModule["type"] = (int)mBillboarding->type;
					jsonModule["billboardingType"] = (int)mBillboarding->billboardingType;
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
						mDefault->spawnTimer = pModule.value().at("spawnTimer");
						mDefault->spawnTime = pModule.value().at("spawnTime");
						mDefault->randomParticleLife = pModule.value().at("randomParticleLife");
						mDefault->minParticleLife = pModule.value().at("minParticleLife");
						mDefault->maxParticleLife = pModule.value().at("maxParticleLife");
						m = mDefault;
						break;
					}
					case 2:
					{
						EmitterMovement* mMovement = new EmitterMovement();
						mMovement->randomPosition = pModule.value().at("randPosition");
						std::vector<float> values = pModule.value().at("minPosition").get<std::vector<float>>();
						mMovement->minPosition = { values[0],values[1],values[2] };
						values.clear();
						values = pModule.value().at("maxPosition").get<std::vector<float>>();
						mMovement->maxPosition = { values[0],values[1],values[2] };
						values.clear();

						mMovement->randomDirection = pModule.value().at("randDirection");
						values = pModule.value().at("minDirection").get<std::vector<float>>();
						mMovement->minDirection = { values[0],values[1],values[2] };
						values.clear();
						values = pModule.value().at("maxDirection").get<std::vector<float>>();
						mMovement->maxDirection = { values[0],values[1],values[2] };
						values.clear();

						mMovement->randomVelocity = pModule.value().at("randVelocity");
						mMovement->minVelocity = pModule.value().at("minVelocity");
						mMovement->maxVelocity = pModule.value().at("maxVelocity");

						mMovement->randomAcceleration = pModule.value().at("randAcceleration");
						values = pModule.value().at("minAcceleration").get<std::vector<float>>();
						mMovement->minAcceleration = { values[0],values[1],values[2] };
						values.clear();
						values = pModule.value().at("maxAcceleration").get<std::vector<float>>();
						mMovement->maxAcceleration = { values[0],values[1],values[2] };
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
						std::vector<float> values = pModule.value().at("minSize").get<std::vector<float>>();
						mSize->minSize = { values[0],values[1],values[2] };
						values.clear();
						values = pModule.value().at("maxSize").get<std::vector<float>>();
						mSize->maxSize = { values[0],values[1],values[2] };
						values.clear();
						m = mSize;
						break;
					}
					case 5:
					{
						int typeB = pModule.value().at("billboardingType");
						ParticleBillboarding* mBillboarding = new ParticleBillboarding((ParticleBillboarding::BillboardingType)typeB);
						m = mBillboarding;
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