#include "C_Particle.h"

// Modules
#include "Engine.h"
#include "M_Renderer3D.h"
#include "M_Editor.h"
#include "M_ResourceManager.h"
#include "M_FileSystem.h"
#include "ParticleModule.h"

// GameObject
#include "GameObject.h"

// Resources
#include "R_Texture.h"
#include "R_Particle.h"

#include "PanelChooser.h"

#include "FSDefs.h"
#include "Log.h"
#include "imgui_stdlib.h"

#include "MathGeoLib/Math/float4x4.h"

C_Particle::C_Particle(GameObject* parent) : Component(parent)
{
	type = ComponentType::PARTICLE;
	//resource = new R_Particle();
	resource = nullptr;
	emitterInstances.clear();
	emitterInstances.shrink_to_fit();
}

C_Particle::~C_Particle()
{

}

bool C_Particle::Start()
{
	std::vector<std::string> tmp;
	std::vector<std::string> tmpFiltered;
	owner->GetEngine()->GetFileSystem()->DiscoverAllFilesFiltered(ASSETS_PARTICLES_DIR, tmp, tmpFiltered, PARTICLES_EXTENSION);

	for (const auto& file : tmpFiltered)
	{
		std::filesystem::path filename = file;
		resourcesList.push_back(filename.stem().string());
	}

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

bool C_Particle::Update(float dt)
{
	for (auto it : emitterInstances)
	{
		it->Update(dt); //kill inactive and update emitter instances
		
		for (unsigned int i = 0; i < it->activeParticles; i++)
		{
			unsigned int particleIndex = it->particleIndices[i];
			Particle* particle = &it->particles[particleIndex];
		
			owner->GetEngine()->GetRenderer()->AddParticle(*it->emitter->texture, particle->CurrentColor,
				float4x4::FromTRS(particle->position, particle->rotation, particle->scale), 
				particle->distanceToCamera);
		}
	}

	return true;
}

bool C_Particle::PostUpdate(float dt)
{
	return true;
}

bool C_Particle::CleanUp()
{
	for (std::vector<EmitterInstance*>::const_iterator it = emitterInstances.begin(); it != emitterInstances.end();++it)
	{
		emitterInstances.erase(it);
		if (emitterInstances.empty())
			break;
	}
	emitterInstances.clear();
	emitterInstances.shrink_to_fit();

	if (resource != nullptr)
		owner->GetEngine()->GetResourceManager()->FreeResource(resource->GetUID());
	resource = nullptr;
	return true;
}

bool C_Particle::InspectorDraw(PanelChooser* chooser)
{
	bool ret = true;

	if (ImGui::CollapsingHeader("Particle System", ImGuiTreeNodeFlags_DefaultOpen))
	{
		if (DrawDeleteButton(owner, this))
			return true;

		//DEBUG BUTTON
		//if ((ImGui::Button("DEBUG: load resource")))
		//{
		//	if (resource == nullptr)
		//		resource = new R_Particle();
		//	Importer::GetInstance()->particleImporter->Load(resource, "New Particle Effect");
		//	for (const auto& emitter : resource->emitters)
		//	{
		//		EmitterInstance* ei = new EmitterInstance(emitter, this);
		//		ei->Init();
		//		emitterInstances.push_back(ei);
		//	}
		//}

		if (resource != nullptr)
		{
			ImGui::PushItemWidth(12.5f * ImGui::GetFontSize());

			ImGui::Text("Resource Name:");
			ImGui::SameLine();
			ImGui::InputText("##Resource",&(resource->name));
			ImGui::PopItemWidth();
			ImGui::SameLine();
			//DEBUG BUTTON
			if ((ImGui::Button("Save Resource")))
			{
				Importer::GetInstance()->particleImporter->Create(resource);
			}

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

				//std::string changeTexture = "Change Texture to " + emitter->name;
				//if (chooser->IsReadyToClose(changeTexture.c_str()))
				//{
				//	if (!chooser->OnChooserClosed().empty())
				//	{
				//		std::string path = chooser->OnChooserClosed();
				//		if (emitter->texture->GetTextureId() == currentTextureId)
				//		{
				//			emitter->texture->SetTextureId(TEXTUREID_DEFAULT);
				//			emitter->texture->SetTexturePath(nullptr);

				//			R_Texture* tex = new R_Texture();
				//			Importer::GetInstance()->textureImporter->Import(path.c_str(), tex);
				//			emitter->texture = tex;
				//		}
				//	}
				//}

				ImGui::Text("Texture:");

				std::string changeTexture = "Change Texture to " + emitter->name;
				if (chooser->IsReadyToClose(changeTexture))
				{
					if (!chooser->OnChooserClosed().empty())
					{
						std::string path = chooser->OnChooserClosed();
						//for (R_Texture& tex : textures)
						//{
						//	if (tex.textureID == currentTextureId)
						//	{
						//		R_Texture tex;
						//		Importer::GetInstance()->textureImporter->Import(path.c_str(), &tex);
						//		textures.push_back(texture);
						//	}
						//}
						if (!path.empty() || path != "")
						{
							if (emitter->texture != nullptr)
								owner->GetEngine()->GetResourceManager()->FreeResource(emitter->texture->GetUID());

							emitter->texture = nullptr;

							emitter->texture = (R_Texture*)owner->GetEngine()->GetResourceManager()->GetResourceFromLibrary(path.c_str());

							emitter->checkerTexture = false;
						}
						else
						{
							if (emitter->texture != nullptr)
								owner->GetEngine()->GetResourceManager()->FreeResource(emitter->texture->GetUID());

							emitter->checkerTexture = true;
							emitter->texture = Importer::GetInstance()->textureImporter->GetCheckerTexture();
						}
					}
				}

				if (emitter->texture != nullptr)
				{
					if (emitter->texture->GetTextureId() != TEXTUREID_DEFAULT)
					{
						ImGui::Image((ImTextureID)emitter->texture->GetTextureId(), ImVec2(85, 85));
						ImGui::SameLine();
						ImGui::BeginGroup();

						if (emitter->checkerTexture)
						{
							ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1, 1, 0, 1));
							if (ImGui::Selectable("Checker Texture")) {}
							ImGui::PopStyleColor();
						}
						else
						{
							ImGui::Text("Texture Path: ");
							ImGui::SameLine();
							ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1, 1, 0, 1));
							if (ImGui::Selectable(emitter->texture->GetLibraryPath())) {}
							ImGui::PopStyleColor();
						}

						ImGui::PushID(owner->GetEngine()->GetEditor()->idTracker++);

						if (ImGui::Button(changeTexture.c_str()))
						{
							chooser->OpenPanel(changeTexture, "png", { "png" });
							currentTextureId = emitter->texture->GetTextureId();
						}

						ImGui::PopID();

						ImGui::PushID(owner->GetEngine()->GetEditor()->idTracker++);
						if (!emitter->checkerTexture)
						{
							std::string deleteTexture = "Delete Texture to " + emitter->name;
							if (ImGui::Button(deleteTexture.c_str()))
							{
								if (emitter->texture != nullptr && emitter->texture->textureID != TEXTUREID_DEFAULT)
								{
									owner->GetEngine()->GetResourceManager()->FreeResource(emitter->texture->GetUID());
									emitter->checkerTexture = true;
									emitter->texture = Importer::GetInstance()->textureImporter->GetCheckerTexture();
								}
							}
						}
						ImGui::PopID();
						ImGui::EndGroup();
					}
				}

				ImGui::Text("Modules: %d", emitter->modules.size());

				ImGui::PushItemWidth(12.5f * ImGui::GetFontSize());
				std::string addName = "Add Module to " + emitter->name;
				if (ImGui::BeginCombo("Add Module##", addName.c_str()))
				{
					for (int i = (int)ParticleModuleType::NONE + 1; i != (int)ParticleModuleType::END; ++i)
					{
						std::string componentTypeName = ModuleTypeToString((ParticleModuleType)i);
						if (ImGui::Selectable(componentTypeName.c_str()))
							emitter->AddModuleByType((ParticleModuleType)i);
					}
					ImGui::EndCombo();
				}

				std::string tabBarName = emitter->name + "Modules";
				if (ImGui::BeginTabBar(tabBarName.c_str(), ImGuiTabBarFlags_Reorderable | ImGuiTabBarFlags_AutoSelectNewTabs | ImGuiTabBarFlags_NoCloseWithMiddleMouseButton & ImGuiTabBarFlags_TabListPopupButton))
				{
					for (ParticleModule* module : emitter->modules)
					{
						if (ImGui::BeginTabItem(ModuleTypeToString(module->type)))
						{
							switch (module->type)
							{
							case ParticleModuleType::DEFAULT:
							{
								EmitterDefault* e = (EmitterDefault*)module;
								ImGui::PopItemWidth();
								ImGui::PushItemWidth(7.5f * ImGui::GetFontSize());

								std::string spawnTimerName = emitter->name + " - SpawnTimer: " + std::to_string(e->spawnTimer);
								ImGui::Text(spawnTimerName.c_str());

								float spawnTime = e->spawnTime;
								std::string spawnTimeName = emitter->name + " - SpawnTime";
								if (ImGui::InputFloat(spawnTimeName.c_str(), &spawnTime, 0.0f, 25.0f))
									e->spawnTime = spawnTime;

								bool randomParticleLife = e->randomParticleLife;
								if (ImGui::Checkbox("Random Particle Life", &randomParticleLife))
									e->randomParticleLife = randomParticleLife;

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
										e->minParticleLife = particleLife;
								}

								if (e->instance)
								{
									std::string activeParticlesName = emitter->name + " - ActiveParticles: " + std::to_string(e->instance->activeParticles);
									ImGui::Text(activeParticlesName.c_str());
								}

								int maxParticles = emitter->maxParticles;
								std::string maxParticlesName = emitter->name + " - MaxParticles";
								if (ImGui::DragInt(maxParticlesName.c_str(), &maxParticles, 1, 1, (uint)MAX_PARTICLES))
								{
									if (maxParticles > MAX_PARTICLES)
										maxParticles = MAX_PARTICLES;
									emitter->maxParticles = maxParticles;
								}

								int particlesPerSpawn = e->particlesPerSpawn;
								std::string particlesPerSpawnName = emitter->name + " - ParticlesPerSpawn";
								if (ImGui::DragInt(particlesPerSpawnName.c_str(), &particlesPerSpawn, 1, 1, 50))
								{
									e->particlesPerSpawn = particlesPerSpawn;
								}

								ImGui::PopItemWidth();
								ImGui::PushItemWidth(12.5f * ImGui::GetFontSize());
								break;
							}
							case ParticleModuleType::MOVEMENT:
							{
								EmitterMovement* e = (EmitterMovement*)module;

								bool randomPosition = e->randomPosition;
								if (ImGui::Checkbox("Random Position", &randomPosition))
									e->randomPosition = randomPosition;

								if (randomPosition)
								{
									float minPosition[3] = { e->minPosition.x,e->minPosition.y,e->minPosition.z };
									std::string minPositionName = emitter->name + " - minPosition";
									if (ImGui::DragFloat3(minPositionName.c_str(), minPosition, 0.1f, -10000.0f, 10000.0f, "%.1f"))
										e->minPosition = { minPosition[0],minPosition[1],minPosition[2] };

									float maxPosition[3] = { e->maxPosition.x,e->maxPosition.y,e->maxPosition.z };
									std::string maxPositionName = emitter->name + " - maxPosition";
									if (ImGui::DragFloat3(maxPositionName.c_str(), maxPosition, 0.1f, -10000.0f, 10000.0f, "%.1f"))
										e->maxPosition = { maxPosition[0],maxPosition[1],maxPosition[2] };
								}
								else
								{
									float position[3] = { e->minPosition.x,e->minPosition.y,e->minPosition.z };
									std::string positionName = emitter->name + " - Position";
									if (ImGui::DragFloat3(positionName.c_str(), position, 0.1f, -10000.0f, 10000.0f, "%.1f"))
										e->minPosition = { position[0],position[1],position[2] };
								}

								ImGui::Spacing();

								bool randomDirection = e->randomDirection;
								if (ImGui::Checkbox("Random Direction", &randomDirection))
									e->randomDirection = randomDirection;

								if (randomDirection)
								{
									float minDirection[3] = { e->minDirection.x,e->minDirection.y,e->minDirection.z };
									std::string minDirectionName = emitter->name + " - minDirection";
									if (ImGui::DragFloat3(minDirectionName.c_str(), minDirection, 0.005f, -1.0f, 1.0f, "%.3f"))
										e->minDirection = { minDirection[0],minDirection[1],minDirection[2] };

									float maxDirection[3] = { e->maxDirection.x,e->maxDirection.y,e->maxDirection.z };
									std::string maxDirectionName = emitter->name + " - maxDirection";
									if (ImGui::DragFloat3(maxDirectionName.c_str(), maxDirection, 0.005f, -1.0f, 1.0f, "%.3f"))
										e->maxDirection = { maxDirection[0],maxDirection[1],maxDirection[2] };
								}
								else
								{
									float direction[3] = { e->minDirection.x,e->minDirection.y,e->minDirection.z };
									std::string directionName = emitter->name + " - Direction";
									if (ImGui::DragFloat3(directionName.c_str(), direction, 0.005f, -1.0f, 1.0f, "%.3f"))
										e->minDirection = { direction[0],direction[1],direction[2] };
								}

								ImGui::Spacing();

								bool randomVelocity = e->randomVelocity;
								if (ImGui::Checkbox("Random Velocity", &randomVelocity))
									e->randomVelocity = randomVelocity;

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
										e->minVelocity = velocity;
								}

								ImGui::Spacing();

								bool randomAcceleration = e->randomAcceleration;
								if (ImGui::Checkbox("Random Acceleration", &randomAcceleration))
									e->randomAcceleration = randomAcceleration;

								if (randomAcceleration)
								{
									float minAcceleration[3] = { e->minAcceleration.x,e->minAcceleration.y,e->minAcceleration.z };
									std::string minAccelerationName = emitter->name + " - minAcceleration";
									if (ImGui::DragFloat3(minAccelerationName.c_str(), minAcceleration, 0.001f, -10.0f, 10.0f, "%.3f"))
										e->minAcceleration = { minAcceleration[0],minAcceleration[1],minAcceleration[2] };

									float maxAcceleration[3] = { e->maxAcceleration.x,e->maxAcceleration.y,e->maxAcceleration.z };
									std::string maxAccelerationName = emitter->name + " - maxAcceleration";
									if (ImGui::DragFloat3(maxAccelerationName.c_str(), maxAcceleration, 0.001f, -10.0f, 10.0f, "%.3f"))
										e->maxAcceleration = { maxAcceleration[0],maxAcceleration[1],maxAcceleration[2] };
								}
								else
								{
									float acceleration[3] = { e->minAcceleration.x,e->minAcceleration.y,e->minAcceleration.z };
									std::string accelerationName = emitter->name + " - Acceleration";
									if (ImGui::DragFloat3(accelerationName.c_str(), acceleration, 0.001f, -10.0f, 10.0f, "%.3f"))
										e->minAcceleration = { acceleration[0],acceleration[1],acceleration[2] };
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
								break;
							}
							case ParticleModuleType::COLOR:
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
										ImGui::Spacing();
									++posList;
								}

								std::string addColorName = "Add Color to " + emitter->name;
								if (ImGui::Button(addColorName.c_str()))
									e->colorOverTime.push_back(FadeColor());

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
								break;
							}
							case ParticleModuleType::SIZE:
							{
								EmitterSize* e = (EmitterSize*)module;

								bool constantSize = e->constantSize;
								if (ImGui::Checkbox("Constant Size", &constantSize))
								{
									e->constantSize = constantSize;
									e->randomInitialSize = false;
									e->randomFinalSize = false;
								}

								bool randomInitialSize = e->randomInitialSize;
								if (ImGui::Checkbox("Random Initial Size", &randomInitialSize))
								{
									e->randomInitialSize = randomInitialSize;
									e->constantSize = false;
								}

								bool randomFinalSize = e->randomFinalSize;
								if (ImGui::Checkbox("Random Final Size", &randomFinalSize))
								{
									e->randomFinalSize = randomFinalSize;
									e->constantSize = false;
								}

								if (constantSize)
								{
									float size[3] = { e->minInitialSize.x,e->minInitialSize.y,e->minInitialSize.z };
									std::string sizeInitialName = emitter->name + " - Size";
									if (ImGui::DragFloat3(sizeInitialName.c_str(), size, 0.1f, -10000.0f, 10000.0f, "%.1f"))
										e->minInitialSize = { size[0],size[1],size[2] };
								}
								else
								{
									if (randomInitialSize)
									{
										float minInitialSize[3] = { e->minInitialSize.x,e->minInitialSize.y,e->minInitialSize.z };
										std::string minInitialSizeName = emitter->name + " - minInitialSize";
										if (ImGui::DragFloat3(minInitialSizeName.c_str(), minInitialSize, 0.1f, -10000.0f, 10000.0f, "%.1f"))
											e->minInitialSize = { minInitialSize[0],minInitialSize[1],minInitialSize[2] };

										float maxInitialSize[3] = { e->maxInitialSize.x,e->maxInitialSize.y,e->maxInitialSize.z };
										std::string maxInitialSizeName = emitter->name + " - maxInitialSize";
										if (ImGui::DragFloat3(maxInitialSizeName.c_str(), maxInitialSize, 0.1f, -10000.0f, 10000.0f, "%.1f"))
											e->maxInitialSize = { maxInitialSize[0],maxInitialSize[1],maxInitialSize[2] };
									}
									else
									{
										float initialSize[3] = { e->minInitialSize.x,e->minInitialSize.y,e->minInitialSize.z };
										std::string sizeInitialName = emitter->name + " - InitialSize";
										if (ImGui::DragFloat3(sizeInitialName.c_str(), initialSize, 0.1f, -10000.0f, 10000.0f, "%.1f"))
											e->minInitialSize = { initialSize[0],initialSize[1],initialSize[2] };
									}

									if (randomFinalSize)
									{
										float minFinalSize[3] = { e->minFinalSize.x,e->minFinalSize.y,e->minFinalSize.z };
										std::string minFinalSizeName = emitter->name + " - minFinalSize";
										if (ImGui::DragFloat3(minFinalSizeName.c_str(), minFinalSize, 0.1f, -10000.0f, 10000.0f, "%.1f"))
											e->minFinalSize = { minFinalSize[0],minFinalSize[1],minFinalSize[2] };

										float maxFinalSize[3] = { e->maxFinalSize.x,e->maxFinalSize.y,e->maxFinalSize.z };
										std::string maxFinalSizeName = emitter->name + " - maxFinalSize";
										if (ImGui::DragFloat3(maxFinalSizeName.c_str(), maxFinalSize, 0.1f, -10000.0f, 10000.0f, "%.1f"))
											e->maxFinalSize = { maxFinalSize[0],maxFinalSize[1],maxFinalSize[2] };
									}
									else
									{
										float finalSize[3] = { e->minFinalSize.x,e->minFinalSize.y,e->minFinalSize.z };
										std::string sizeFinalName = emitter->name + " - FinalSize";
										if (ImGui::DragFloat3(sizeFinalName.c_str(), finalSize, 0.1f, -10000.0f, 10000.0f, "%.1f"))
											e->minFinalSize = { finalSize[0],finalSize[1],finalSize[2] };
									}
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
								break;
							}
							case ParticleModuleType::BILLBOARDING:
							{
								ParticleBillboarding* particleBillboarding = (ParticleBillboarding*)module;

								std::string addBillboarding = "Change Type of " + emitter->name;
								if (ImGui::BeginCombo("Billboarding##", addBillboarding.c_str()))
								{
									for (int i = (int)ParticleBillboarding::BillboardingType::NONE + 1; i != (int)ParticleBillboarding::BillboardingType::END; ++i)
									{
										std::string componentTypeName = particleBillboarding->BillboardTypeToString((ParticleBillboarding::BillboardingType)i);
										if (ImGui::Selectable(componentTypeName.c_str()))
											particleBillboarding->billboardingType = (ParticleBillboarding::BillboardingType)i;
									}
									ImGui::EndCombo();
								}

								bool hideModule = particleBillboarding->hideBillboarding;
								if (ImGui::Checkbox("Hide Billboarding", &hideModule))
									particleBillboarding->hideBillboarding = hideModule;

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
								break;
							}
							default:
								break;
							}
							ImGui::EndTabItem();
						}
					}
					ImGui::EndTabBar();
				}
				ImGui::PopItemWidth();

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
			if (ImGui::BeginCombo("Add Resource##", addResourceName.c_str()))
			{
				for (int i = -1; i != (int)resourcesList.size(); ++i)
				{
					if (i == -1)
					{
						if (ImGui::Selectable("New Resource"))
						{
							resource = new R_Particle();
							Emitter* e = new Emitter();
							resource->emitters.push_back(e);
							EmitterInstance* eI = new EmitterInstance(e, this);
							emitterInstances.push_back(eI);
							eI->Init();
						}
					}
					else
					{
						if (ImGui::Selectable(resourcesList.at(i).c_str()))
						{
							if (resource == nullptr)
								resource = new R_Particle();
							Importer::GetInstance()->particleImporter->Load(resource, resourcesList.at(i).c_str());
							for (const auto& emitter : resource->emitters)
							{
								EmitterInstance* ei = new EmitterInstance(emitter, this);
								emitterInstances.push_back(ei);
								ei->Init();
							}
						}
					}
				}
				ImGui::EndCombo();
			}




			//std::string addResourceName = "Add Resource";
			//ImGui::Combo("##Resource Combo", &resourceToAdd, "Add Resource\0New Resource");
			//ImGui::SameLine();
			//if ((ImGui::Button(addResourceName.c_str())))
			//{
			//	if (resourceToAdd == 1)
			//	{
			//		resource = new R_Particle();
			//		Emitter* e = new Emitter();
			//		resource->emitters.push_back(e);
			//		EmitterInstance* eI = new EmitterInstance(e, this);
			//		emitterInstances.push_back(eI);
			//		eI->Init();
			//	}
			//}
		}
	}
	else
		DrawDeleteButton(owner, this);

	return ret;
}

void C_Particle::InspectorDrawColor(std::string emitterName, FadeColor& color, int index)
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

void C_Particle::ClearParticles()
{
	for (std::vector<EmitterInstance*>::iterator it = emitterInstances.begin(); it < emitterInstances.end(); ++it)
	{
		(*it)->KillParticles();
	}
}

void C_Particle::StopParticleSpawn()
{
	for (std::vector<EmitterInstance*>::iterator it = emitterInstances.begin(); it < emitterInstances.end(); ++it)
	{
		(*it)->SetParticleEmission(false);
	}
}

void C_Particle::ResumeParticleSpawn()
{
	for (std::vector<EmitterInstance*>::iterator it = emitterInstances.begin(); it < emitterInstances.end(); ++it)
	{
		(*it)->SetParticleEmission(true);
	}
}

void C_Particle::NewEmitterName(std::string& name, int n)
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

void C_Particle::Save(Json& json) const
{
	json["type"] = (int)type;

	Json jsonResource;
	if (resource != nullptr)
	{
		jsonResource["name"] = resource->name;
		Json jsonEmitter;
		for (auto e : resource->emitters)
		{
			jsonEmitter["maxParticles"] = e->maxParticles;
			jsonEmitter["name"] = e->name;

			if (e->texture != nullptr)
				jsonEmitter["texture_path"] = e->texture->GetTexturePath();

			Json jsonModule;
			for (auto m : e->modules)
			{
				switch (m->type)
				{
				case ParticleModuleType::DEFAULT:
				{
					EmitterDefault* mDefault = (EmitterDefault*)m;
					jsonModule["type"] = (int)mDefault->type;
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

					jsonModule["minInitialSize"] = { mSize->minInitialSize.x,mSize->minInitialSize.y,mSize->minInitialSize.z };
					if (!mSize->constantSize)
					{
						if (mSize->randomInitialSize)
							jsonModule["maxInitialSize"] = { mSize->minInitialSize.x,mSize->minInitialSize.y,mSize->minInitialSize.z };
						jsonModule["minFinalSize"] = { mSize->minFinalSize.x,mSize->minFinalSize.y,mSize->minFinalSize.z };
						if (mSize->randomFinalSize)
							jsonModule["maxFinalSize"] = { mSize->maxFinalSize.x,mSize->maxFinalSize.y,mSize->maxFinalSize.z };
					}
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

void C_Particle::Load(Json& json)
{
	if (!json.empty())
	{
		if (resource == nullptr)
			resource = new R_Particle();

		resource->emitters.clear();
		resource->emitters.shrink_to_fit();
		for (const auto& r : json.at("resource").items())
		{
			resource->name = r.value().at("name").get<std::string>();

			for (const auto& emitter : r.value().at("emitters").items())
			{
				emitterInstances.clear();
				emitterInstances.shrink_to_fit();
				Emitter* e = new Emitter(emitter.value().at("name").get<std::string>().c_str());
				EmitterInstance* ei = new EmitterInstance(e, this);
				emitterInstances.push_back(ei);
				ei->Init();
				e->maxParticles = emitter.value().at("maxParticles");
				e->texture = new R_Texture();
				if(emitter.value().contains("texture_path"))
					e->texture->SetTexturePath(emitter.value().at("texture_path").get<std::string>().c_str());
				if (e->texture->GetTexturePath() != "")
					Importer::GetInstance()->textureImporter->Import(e->texture->GetTexturePath(), e->texture);

				e->modules.clear();
				e->modules.shrink_to_fit();
				for (const auto& pModule : emitter.value().at("modules").items())
				{
					ParticleModule* m = nullptr;
					int type = pModule.value().at("type");
					switch (type)
					{
					case 1:
					{
						EmitterDefault* mDefault = new EmitterDefault();
						mDefault->spawnTimer = 0.0f;
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
						values.shrink_to_fit();

						m = mMovement;
						break;
					}
					case 3:
					{
						EmitterColor* mColor = new EmitterColor();
						mColor->colorOverTime.clear();
						mColor->colorOverTime.shrink_to_fit();
						for (const auto& c : pModule.value().at("colors").items())
						{
							FadeColor fc = FadeColor();
							std::vector<float> values = c.value().at("color").get<std::vector<float>>();
							fc.color = Color(values[0], values[1], values[2], values[3]);
							values.clear();
							values.shrink_to_fit();
							fc.pos = c.value().at("position");
							mColor->colorOverTime.push_back(fc);
						}
						m = mColor;
						break;
					}
					case 4:
					{
						EmitterSize* mSize = new EmitterSize();
						std::vector<float> values = pModule.value().at("minInitialSize").get<std::vector<float>>();
						mSize->minInitialSize = { values[0],values[1],values[2] };
						values.clear();
						if (pModule.value().contains("maxInitialSize"))
						{
							mSize->randomInitialSize = true;
							values = pModule.value().at("maxInitialSize").get<std::vector<float>>();
							mSize->maxInitialSize = { values[0],values[1],values[2] };
							values.clear();
						}
						if (pModule.value().contains("minFinalSize"))
						{
							values = pModule.value().at("minFinalSize").get<std::vector<float>>();
							mSize->minFinalSize = { values[0],values[1],values[2] };
							values.clear();
						}
						if (pModule.value().contains("maxFinalSize"))
						{
							mSize->randomFinalSize = true;
							values = pModule.value().at("maxFinalSize").get<std::vector<float>>();
							mSize->maxFinalSize = { values[0],values[1],values[2] };
							values.clear();
						}

						if (!mSize->randomInitialSize && !mSize->randomFinalSize)
							mSize->constantSize = true;

						values.shrink_to_fit();
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

const char* C_Particle::ModuleTypeToString(ParticleModuleType e)
{

	const std::map<ParticleModuleType, const char*> moduleTypeStrings{
		{ParticleModuleType::DEFAULT, "Default"},
		{ParticleModuleType::MOVEMENT, "Movement"},
		{ParticleModuleType::COLOR, "Color"},
		{ParticleModuleType::SIZE, "Size"},
		{ParticleModuleType::BILLBOARDING, "Billboarding"},
	};
	auto   it = moduleTypeStrings.find(e);
	return it == moduleTypeStrings.end() ? "Out of range" : it->second;

}
