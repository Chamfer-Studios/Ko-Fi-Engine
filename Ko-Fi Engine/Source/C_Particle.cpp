#include "C_Particle.h"

// Modules
#include "Engine.h"
#include "M_Renderer3D.h"
#include "M_Editor.h"
#include "M_ResourceManager.h"
#include "M_FileSystem.h"
#include "M_Camera3D.h"
#include "ParticleModule.h"
#include "C_Camera.h"
#include "Emitter.h"

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
	resource = nullptr;
}

C_Particle::~C_Particle()
{
	CleanUp();
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
		//CONSOLE_LOG("active particles: %d", it->activeParticles);

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
	for (auto it : emitterInstances)
	{
		RELEASE(it);
	}
	emitterInstances.clear();
	emitterInstances.shrink_to_fit();

	for (auto it : resourcesList)
	{
		it.clear();
		it.shrink_to_fit();
	}
	resourcesList.clear();
	resourcesList.shrink_to_fit();

	RELEASE(resource);
	return true;
}

bool C_Particle::InspectorDraw(PanelChooser* chooser)
{
	bool ret = true;

	if (ImGui::CollapsingHeader("Particle System", ImGuiTreeNodeFlags_DefaultOpen))
	{
		if (DrawDeleteButton(owner, this))
			return true;

		if (resource != nullptr)
		{
			ImGui::PushItemWidth(12.5f * ImGui::GetFontSize());

			ImGui::Text("Resource Name:");
			ImGui::SameLine();
			ImGui::InputText("##Resource",&(resource->name));
			ImGui::PopItemWidth();
			ImGui::SameLine();

			if ((ImGui::Button("Save Resource")))
				Importer::GetInstance()->particleImporter->Create(resource, emitterInstances[0]->loop);

			if (ImGui::Button("Kill All Particles"))
				ClearParticles();

			bool loop = emitterInstances.front()->loop;
			if (ImGui::Checkbox("Loop", &loop))
			{
				for (auto ei : emitterInstances)
					ei->loop = loop;
			}

			ImGui::Text("Emitter Instances: %d", emitterInstances.size());
			ImGui::Text("Emitters: %d", resource->emitters.size());

			if (ImGui::Button("Add Emitter"))
			{
				std::string name = "Emitter";
				NewEmitterName(name);
				Emitter* e = new Emitter(name.c_str());
				resource->emitters.push_back(e);
				EmitterInstance* ei = new EmitterInstance(e, this);
				ei->Init();
				emitterInstances.push_back(ei);
			}

			if(resource->emitters.size() > 0)
				ImGui::Separator();

			for (Emitter* emitter : resource->emitters)
			{
				ImGui::PushID(owner->GetEngine()->GetEditor()->idTracker++);
				ImGui::Text("Name:");
				ImGui::SameLine();
				ImGui::InputText("##Emitter", &(emitter->name));
				ImGui::Text("Texture:");

				std::string changeTexture = "Change Texture to" + emitter->name;
				if (chooser->IsReadyToClose(changeTexture))
				{
					if (!chooser->OnChooserClosed().empty())
					{
						std::string path = chooser->OnChooserClosed();
						if (emitter->texture->GetTextureId() == currentTextureId)
						{
							emitter->texture->SetTextureId(TEXTUREID_DEFAULT);
							emitter->texture->SetAssetPath(nullptr);
						}
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
							ImGui::Text("Texture Path:");
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
							if (ImGui::Button("Delete Texture"))
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
				EmitterInstance* ei = nullptr;
				for (const auto& eiIt : emitterInstances)
				{
					if (eiIt->emitter == emitter)
						ei = eiIt;
				}

				if (ImGui::Button("Kill All Emitter Particles"))
					ei->KillAllParticles();

				bool stopParticleEmission = ei->GetParticleEmission();
				if (ImGui::Checkbox("Stop Particles Emission", &stopParticleEmission))
					ei->SetParticleEmission(stopParticleEmission);

				ImGui::Text("Modules: %d", emitter->modules.size());
				ImGui::PushItemWidth(12.5f * ImGui::GetFontSize());

				if (ImGui::BeginCombo("Add Module##", "Add Module"))
				{
					for (int i = (int)ParticleModuleType::NONE + 1; i != (int)ParticleModuleType::END; ++i)
					{
						std::string componentTypeName = ModuleTypeToString((ParticleModuleType)i);
						if (ImGui::Selectable(componentTypeName.c_str()))
							emitter->AddModuleByType((ParticleModuleType)i);
					}
					ImGui::EndCombo();
				}

				if (ImGui::BeginTabBar("Modules", ImGuiTabBarFlags_Reorderable | ImGuiTabBarFlags_AutoSelectNewTabs | ImGuiTabBarFlags_NoCloseWithMiddleMouseButton & ImGuiTabBarFlags_TabListPopupButton))
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

								std::string spawnTimerName = "SpawnTimer: " + std::to_string(e->spawnTimer);
								ImGui::Text(spawnTimerName.c_str());

								float spawnTime = e->spawnTime;
								if (ImGui::DragFloat("SpawnTime", &spawnTime, 0.005f, 0.0f, 25.0f, "%.3f"))
									e->spawnTime = spawnTime;

								bool randomParticleLife = e->randomParticleLife;
								if (ImGui::Checkbox("Random Particle Life", &randomParticleLife))
									e->randomParticleLife = randomParticleLife;

								if (randomParticleLife)
								{
									float particleLife[2] = { e->minParticleLife,e->maxParticleLife };
									if (ImGui::DragFloat2("ParticleLife", particleLife, 0.005f, 0.0f, 1000.0f, "%.3f"))
									{
										e->minParticleLife = particleLife[0];
										e->maxParticleLife = particleLife[1];
									}
								}
								else
								{
									float particleLife = e->minParticleLife;
									if (ImGui::DragFloat("ParticleLife", &particleLife, 0.005f, 0.0f, 1000.0f, "%.3f"))
										e->minParticleLife = particleLife;
								}

								if (e->instance)
								{
									std::string activeParticlesName = "ActiveParticles: " + std::to_string(e->instance->activeParticles);
									ImGui::Text(activeParticlesName.c_str());
								}

								int maxParticles = emitter->maxParticles;
								if (ImGui::DragInt("Max Particles", &maxParticles, 1, 1, (uint)MAX_PARTICLES))
								{
									if (maxParticles > MAX_PARTICLES)
										maxParticles = MAX_PARTICLES;

									emitter->maxParticles = maxParticles;
								}

								int particlesPerSpawn = e->particlesPerSpawn;
								if (ImGui::DragInt("Particles Per Spawn", &particlesPerSpawn, 1, 1, 50))
									e->particlesPerSpawn = particlesPerSpawn;

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
									if (ImGui::DragFloat3("Min Position", minPosition, 0.1f, -10000.0f, 10000.0f, "%.1f"))
										e->minPosition = { minPosition[0],minPosition[1],minPosition[2] };

									float maxPosition[3] = { e->maxPosition.x,e->maxPosition.y,e->maxPosition.z };
									if (ImGui::DragFloat3("Max Position", maxPosition, 0.1f, -10000.0f, 10000.0f, "%.1f"))
										e->maxPosition = { maxPosition[0],maxPosition[1],maxPosition[2] };
								}
								else
								{
									float position[3] = { e->minPosition.x,e->minPosition.y,e->minPosition.z };
									if (ImGui::DragFloat3("Position", position, 0.1f, -10000.0f, 10000.0f, "%.1f"))
										e->minPosition = { position[0],position[1],position[2] };
								}
								ImGui::Spacing();

								bool followForward = e->followForward;
								if (ImGui::Checkbox("Follow Forward", &followForward))
									e->followForward = followForward;

								if (!followForward)
								{
									bool randomDirection = e->randomDirection;
									if (ImGui::Checkbox("Random Direction", &randomDirection))
										e->randomDirection = randomDirection;

									if (randomDirection)
									{
										float minDirection[3] = { e->minDirection.x,e->minDirection.y,e->minDirection.z };
										if (ImGui::DragFloat3("Min Direction", minDirection, 0.005f, -1.0f, 1.0f, "%.3f"))
											e->minDirection = { minDirection[0],minDirection[1],minDirection[2] };

										float maxDirection[3] = { e->maxDirection.x,e->maxDirection.y,e->maxDirection.z };
										if (ImGui::DragFloat3("Max Direction", maxDirection, 0.005f, -1.0f, 1.0f, "%.3f"))
											e->maxDirection = { maxDirection[0],maxDirection[1],maxDirection[2] };
									}
									else
									{
										float direction[3] = { e->minDirection.x,e->minDirection.y,e->minDirection.z };
										if (ImGui::DragFloat3("Direction", direction, 0.005f, -1.0f, 1.0f, "%.3f"))
											e->minDirection = { direction[0],direction[1],direction[2] };
									}
								}
								ImGui::Spacing();

								bool randomVelocity = e->randomVelocity;
								if (ImGui::Checkbox("Random Velocity", &randomVelocity))
									e->randomVelocity = randomVelocity;

								if (randomVelocity)
								{
									float velocity[2] = { e->minVelocity,e->maxVelocity };
									if (ImGui::DragFloat2("Velocity", velocity, 0.1f, 0.0f, 1000.0f, "%.1f"))
									{
										e->minVelocity = velocity[0];
										e->maxVelocity = velocity[1];
									}
								}
								else
								{
									float velocity = e->minVelocity;
									if (ImGui::DragFloat("Velocity", &velocity, 0.1f, 0.0f, 1000.0f, "%.1f"))
										e->minVelocity = velocity;
								}
								ImGui::Spacing();

								bool randomAcceleration = e->randomAcceleration;
								if (ImGui::Checkbox("Random Acceleration", &randomAcceleration))
									e->randomAcceleration = randomAcceleration;

								if (randomAcceleration)
								{
									float minAcceleration[3] = { e->minAcceleration.x,e->minAcceleration.y,e->minAcceleration.z };
									if (ImGui::DragFloat3("Min Acceleration", minAcceleration, 0.001f, -10.0f, 10.0f, "%.3f"))
										e->minAcceleration = { minAcceleration[0],minAcceleration[1],minAcceleration[2] };

									float maxAcceleration[3] = { e->maxAcceleration.x,e->maxAcceleration.y,e->maxAcceleration.z };
									if (ImGui::DragFloat3("Max Acceleration", maxAcceleration, 0.001f, -10.0f, 10.0f, "%.3f"))
										e->maxAcceleration = { maxAcceleration[0],maxAcceleration[1],maxAcceleration[2] };
								}
								else
								{
									float acceleration[3] = { e->minAcceleration.x,e->minAcceleration.y,e->minAcceleration.z };
									if (ImGui::DragFloat3("Acceleration", acceleration, 0.001f, -10.0f, 10.0f, "%.3f"))
										e->minAcceleration = { acceleration[0],acceleration[1],acceleration[2] };
								}

								if (ImGui::Button("Delete Module Movement"))
									DeleteModule(emitter, ParticleModuleType::MOVEMENT);

								break;
							}
							case ParticleModuleType::COLOR:
							{
								EmitterColor* e = (EmitterColor*)module;

								ImGui::Text("Colors: %d", e->colorOverTime.size());
								ImGui::Spacing();
								for (std::vector<FadeColor>::iterator color = e->colorOverTime.begin(); color < e->colorOverTime.end(); ++color)
								{
									ImGui::PushID(owner->GetEngine()->GetEditor()->idTracker++);
									FadeColor currentColor = (*color);

									float c[4] = { currentColor.color.r,currentColor.color.g,currentColor.color.b,currentColor.color.a };
									if (ImGui::ColorEdit4("Color", c))
									{
										currentColor.color.r = c[0];
										currentColor.color.g = c[1];
										currentColor.color.b = c[2];
										currentColor.color.a = c[3];
									}

									float p = currentColor.pos;
									if (ImGui::DragFloat("Position", &p, 0.005f, 0.0f, 1.0f, "%.3f"))
										currentColor.pos = p;

									(*color) = currentColor;

									if (ImGui::Button("Delete Color"))
									{
										e->colorOverTime.erase(color);
										ImGui::PopID();
										break;
									}

									if (color != e->colorOverTime.end())
										ImGui::Spacing();

									ImGui::PopID();
								}

								if (ImGui::Button("Add Color"))
									e->colorOverTime.push_back(FadeColor());

								if (ImGui::Button("Delete Module Color"))
									DeleteModule(emitter, ParticleModuleType::COLOR);

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
									if (ImGui::DragFloat3("Size", size, 0.005f, 0.0f, 10000.0f, "%.3f"))
										e->minInitialSize = { size[0],size[1],size[2] };
								}
								else
								{
									if (randomInitialSize)
									{
										float minInitialSize[3] = { e->minInitialSize.x,e->minInitialSize.y,e->minInitialSize.z };
										if (ImGui::DragFloat3("Min Initial Size", minInitialSize, 0.005f, 0.0f, 10000.0f, "%.3f"))
											e->minInitialSize = { minInitialSize[0],minInitialSize[1],minInitialSize[2] };

										float maxInitialSize[3] = { e->maxInitialSize.x,e->maxInitialSize.y,e->maxInitialSize.z };
										if (ImGui::DragFloat3("Max Initial Size", maxInitialSize, 0.005f, 0.0f, 10000.0f, "%.3f"))
											e->maxInitialSize = { maxInitialSize[0],maxInitialSize[1],maxInitialSize[2] };
									}
									else
									{
										float initialSize[3] = { e->minInitialSize.x,e->minInitialSize.y,e->minInitialSize.z };
										if (ImGui::DragFloat3("Initial Size", initialSize, 0.005f, 0.0f, 10000.0f, "%.3f"))
											e->minInitialSize = { initialSize[0],initialSize[1],initialSize[2] };
									}

									if (randomFinalSize)
									{
										float minFinalSize[3] = { e->minFinalSize.x,e->minFinalSize.y,e->minFinalSize.z };
										if (ImGui::DragFloat3("Min Final Size", minFinalSize, 0.005f, 0.0f, 10000.0f, "%.3f"))
											e->minFinalSize = { minFinalSize[0],minFinalSize[1],minFinalSize[2] };

										float maxFinalSize[3] = { e->maxFinalSize.x,e->maxFinalSize.y,e->maxFinalSize.z };
										if (ImGui::DragFloat3("Max Final Size", maxFinalSize, 0.005f, 0.0f, 10000.0f, "%.3f"))
											e->maxFinalSize = { maxFinalSize[0],maxFinalSize[1],maxFinalSize[2] };
									}
									else
									{
										float finalSize[3] = { e->minFinalSize.x,e->minFinalSize.y,e->minFinalSize.z };
										if (ImGui::DragFloat3("Final Size", finalSize, 0.005f, 0.0f, 10000.0f, "%.3f"))
											e->minFinalSize = { finalSize[0],finalSize[1],finalSize[2] };
									}
								}

								if (ImGui::Button("Delete Module Size"))
									DeleteModule(emitter, ParticleModuleType::SIZE);

								break;
							}
							case ParticleModuleType::BILLBOARDING:
							{
								ParticleBillboarding* particleBillboarding = (ParticleBillboarding*)module;

								if (ImGui::BeginCombo("Billboarding##", particleBillboarding->BillboardTypeToString((ParticleBillboarding::BillboardingType)particleBillboarding->billboardingType)))
								{
									for (int i = (int)ParticleBillboarding::BillboardingType::NONE + 1; i != (int)ParticleBillboarding::BillboardingType::END; ++i)
									{
										std::string componentTypeName = particleBillboarding->BillboardTypeToString((ParticleBillboarding::BillboardingType)i);
										if (ImGui::Selectable(componentTypeName.c_str()))
											particleBillboarding->billboardingType = (ParticleBillboarding::BillboardingType)i;
									}
									ImGui::EndCombo();
								}

								bool rangeDegrees = particleBillboarding->rangeDegrees;
								if (ImGui::Checkbox("Range Degrees", &rangeDegrees))
									particleBillboarding->rangeDegrees = rangeDegrees;

								if (particleBillboarding->rangeDegrees)
								{
									int degrees[2] = { particleBillboarding->minDegrees,particleBillboarding->maxDegrees };
									if (ImGui::DragInt2("Degree Range", degrees, 1, 0, 360))
									{
										particleBillboarding->minDegrees = degrees[0];
										particleBillboarding->maxDegrees = degrees[1];
									}
								}
								else
								{
									int degrees = particleBillboarding->minDegrees;
									if (ImGui::DragInt("Degrees", &degrees, 1, 0, 360))
										particleBillboarding->minDegrees = degrees;
								}

								bool frontAxis = particleBillboarding->frontAxis;
								if (ImGui::Checkbox("FrontAxis", &frontAxis))
									particleBillboarding->frontAxis = frontAxis;

								bool topAxis = particleBillboarding->topAxis;
								if (ImGui::Checkbox("TopAxis", &topAxis))
									particleBillboarding->topAxis = topAxis;

								bool sideAxis = particleBillboarding->sideAxis;
								if (ImGui::Checkbox("SideAxis", &sideAxis))
									particleBillboarding->sideAxis = sideAxis;

								bool hideModule = particleBillboarding->hideBillboarding;
								if (ImGui::Checkbox("Hide Billboarding", &hideModule))
									particleBillboarding->hideBillboarding = hideModule;

								if (ImGui::Button("Delete Module Billboarding"))
									DeleteModule(emitter, ParticleModuleType::BILLBOARDING);

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

				ImGui::PopID();
			}
		}
		else
		{
			if (ImGui::BeginCombo("Add Resource##", "Add Resource"))
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
							emitterInstances.clear();
							if (resource == nullptr)
								resource = new R_Particle();
							int loop = 1;
							Importer::GetInstance()->particleImporter->Load(resource, resourcesList.at(i).c_str(),loop);
							for (const auto& emitter : resource->emitters)
							{
								EmitterInstance* ei = new EmitterInstance(emitter, this);
								emitterInstances.push_back(ei);
								ei->Init();
								ei->loop = loop;
							}
						}
					}
				}
				ImGui::EndCombo();
			}
		}
	}
	else
		DrawDeleteButton(owner, this);

	return ret;
}

void C_Particle::ClearParticles()
{
	for (std::vector<EmitterInstance*>::iterator it = emitterInstances.begin(); it < emitterInstances.end(); ++it)
		(*it)->KillAllParticles();
}

void C_Particle::DeleteModule(Emitter* e, ParticleModuleType t)
{
	for (std::vector<ParticleModule*>::iterator it = e->modules.begin(); it < e->modules.end(); ++it)
	{
		if ((*it)->type == t)
		{
			e->modules.erase(it);
			return;
		}
	}
}

void C_Particle::StopParticleSpawn()
{
	for (std::vector<EmitterInstance*>::iterator it = emitterInstances.begin(); it < emitterInstances.end(); ++it)
		(*it)->SetParticleEmission(false);
}

void C_Particle::ResumeParticleSpawn()
{
	for (std::vector<EmitterInstance*>::iterator it = emitterInstances.begin(); it < emitterInstances.end(); ++it)
		(*it)->SetParticleEmission(true);
	ResetTimers();
}

void C_Particle::ResetTimers()
{
	for (auto emitter : resource->emitters)
	{
		for (auto m : emitter->modules)
		{
			if (m->type == ParticleModuleType::DEFAULT)
			{
				EmitterDefault* mDef = (EmitterDefault*)m;
				mDef->spawnTimer = mDef->spawnTime;
				break;
			}
		}
	}
}

void C_Particle::SetLoop(bool v)
{
	for (auto emitter : resource->emitters)
	{
		for (auto m : emitter->modules)
		{
			if (m->type == ParticleModuleType::DEFAULT)
			{
				EmitterDefault* mDef = (EmitterDefault*)m;
				mDef->looping = v;
				break;
			}
		}
	}
}

void C_Particle::SetColor(float r, float g, float b, float a)
{
	for (auto e : resource->emitters)
	{
		for (auto m : e->modules)
		{
			if (m->type == ParticleModuleType::COLOR)
			{
				EmitterColor* eColor = (EmitterColor*)m;
				eColor->colorOverTime[0].color = Color(r, g, b, a);
			}
		}
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
			jsonEmitter["name"] = e->name;
			jsonEmitter["maxParticles"] = e->maxParticles;
			for (auto ei : emitterInstances)
			{
				if (ei->emitter == e)
				{
					jsonEmitter["loop"] = ei->loop;
					break;
				}
			}

			if (e->texture != nullptr)
				jsonEmitter["texture_path"] = e->texture->GetAssetPath();

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
					jsonModule["particlesPerSpawn"] = mDefault->particlesPerSpawn;
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
					jsonModule["followForward"] = mMovement->followForward;
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
						jsonColor.clear();
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
					jsonModule["minDegrees"] = mBillboarding->minDegrees;
					jsonModule["maxDegrees"] = mBillboarding->maxDegrees;
					jsonModule["frontAxis"] = mBillboarding->frontAxis;
					jsonModule["topAxis"] = mBillboarding->topAxis;
					jsonModule["sideAxis"] = mBillboarding->sideAxis;
					break;
				}
				default:
					break;
				}
				jsonEmitter["modules"].push_back(jsonModule);
				jsonModule.clear();
			}
			jsonResource["emitters"].push_back(jsonEmitter);
			jsonEmitter.clear();
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
		emitterInstances.clear();
		emitterInstances.shrink_to_fit();
		for (const auto& r : json.at("resource").items())
		{
			resource->name = r.value().at("name").get<std::string>();

			for (const auto& emitter : r.value().at("emitters").items())
			{
				Emitter* e = new Emitter(emitter.value().at("name").get<std::string>().c_str());
				EmitterInstance* ei = new EmitterInstance(e, this);
				emitterInstances.push_back(ei);
				ei->Init();
				ei->loop = emitter.value().at("loop");
				e->maxParticles = emitter.value().at("maxParticles");
				e->texture = nullptr;
				if (emitter.value().contains("texture_path"))
					e->texture = (R_Texture*)owner->GetEngine()->GetResourceManager()->GetResourceFromLibrary(emitter.value().at("texture_path").get<std::string>().c_str());
				else
					e->texture = Importer::GetInstance()->textureImporter->GetCheckerTexture();

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
						mDefault->particlesPerSpawn = pModule.value().at("particlesPerSpawn");
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

						mMovement->followForward = pModule.value().at("followForward");

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
						bool notConstant = false;
						std::vector<float> values = pModule.value().at("minInitialSize").get<std::vector<float>>();
						mSize->minInitialSize = { values[0],values[1],values[2] };
						values.clear();
						if (pModule.value().contains("maxInitialSize"))
						{
							notConstant = true;
							mSize->randomInitialSize = true;
							values = pModule.value().at("maxInitialSize").get<std::vector<float>>();
							mSize->maxInitialSize = { values[0],values[1],values[2] };
							values.clear();
						}
						if (pModule.value().contains("minFinalSize"))
						{
							notConstant = true;
							values = pModule.value().at("minFinalSize").get<std::vector<float>>();
							mSize->minFinalSize = { values[0],values[1],values[2] };
							values.clear();
						}
						if (pModule.value().contains("maxFinalSize"))
						{
							notConstant = true;
							mSize->randomFinalSize = true;
							values = pModule.value().at("maxFinalSize").get<std::vector<float>>();
							mSize->maxFinalSize = { values[0],values[1],values[2] };
							values.clear();
						}

						mSize->constantSize = !notConstant;

						values.shrink_to_fit();
						m = mSize;
						break;
					}
					case 5:
					{
						int typeB = pModule.value().at("billboardingType");
						ParticleBillboarding* mBillboarding = new ParticleBillboarding((ParticleBillboarding::BillboardingType)typeB);
						mBillboarding->minDegrees = pModule.value().at("minDegrees");
						mBillboarding->maxDegrees = pModule.value().at("maxDegrees");
						mBillboarding->frontAxis = pModule.value().at("frontAxis");
						mBillboarding->topAxis = pModule.value().at("topAxis");
						mBillboarding->sideAxis = pModule.value().at("sideAxis");
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
