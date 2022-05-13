#include "I_Particles.h"
#include "JsonHandler.h"
#include "R_Particle.h"
#include "Emitter.h"
#include "ParticleModule.h"
#include "Engine.h"
#include "FSDefs.h"
#include "M_FileSystem.h"
#include <filesystem>
#include "Log.h"

I_Particle::I_Particle(KoFiEngine* engine) : engine(engine)
{
}

I_Particle::~I_Particle()
{
}

bool I_Particle::Create(R_Particle* particle)
{
	bool ret = false;

	JsonHandler jsonHandler;
	Json jsonFile;

	const char* name = particle->name.c_str();

	jsonFile[name];
	jsonFile[name]["name"] = name;
	jsonFile[name]["emitters"] = Json::array();
	for (std::vector<Emitter*>::iterator e = particle->emitters.begin(); e != particle->emitters.end(); ++e)
	{
		Json jsonEmitter;

		jsonEmitter["name"] = (*e)->name;
		jsonEmitter["maxParticles"] = (*e)->maxParticles;
		jsonEmitter["texture_path"] = (*e)->texture->GetTexturePath();
		std::vector<ParticleModule*> modules;
		for (std::vector<ParticleModule*>::iterator m = (*e)->modules.begin(); m != (*e)->modules.end(); ++m)
		{
			Json jsonModule;
			switch ((*m)->type)
			{
			case ParticleModuleType::DEFAULT:
			{
				EmitterDefault* mDefault = (EmitterDefault*)(*m);
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
				EmitterMovement* mMovement = (EmitterMovement*)(*m);
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
				EmitterColor* mColor = (EmitterColor*)(*m);
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
				EmitterSize* mSize = (EmitterSize*)(*m);
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
				ParticleBillboarding* mBillboarding = (ParticleBillboarding*)(*m);
				jsonModule["type"] = (int)mBillboarding->type;
				jsonModule["billboardingType"] = (int)mBillboarding->billboardingType;
				break;
			}
			default:
				break;
			}
			jsonEmitter["modules"].push_back(jsonModule);
		}
		jsonFile[name]["emitters"].push_back(jsonEmitter);
	}

	std::string path = ASSETS_PARTICLES_DIR + std::string(name) + PARTICLES_EXTENSION;

	if (engine->GetFileSystem()->CheckDirectory(ASSETS_PARTICLES_DIR))
		ret = jsonHandler.SaveJson(jsonFile, path.c_str());
	else
		ret = false;


	return ret;
}

bool I_Particle::Save(const R_Particle* particle, const char* path)
{
	if (engine->GetFileSystem()->CheckDirectory(PARTICLES_DIR))
	{
		return engine->GetFileSystem()->CopyFileTo(particle->GetAssetPath(), path);
	}
	else
	{
		KOFI_ERROR("Particle Importer: Could not save resource particle, invalid path.");
		return false;
	}
}

bool I_Particle::Load(R_Particle* particle, const char* name)
{
	bool ret = true;

	JsonHandler jsonHandler;
	Json jsonFile;
	Json jsonScene;

	std::string path = ASSETS_PARTICLES_DIR + std::string(name) + PARTICLES_EXTENSION;
	ret = jsonHandler.LoadJson(jsonFile, path.c_str());

	if (ret && !jsonFile.is_null())
	{
		particle->emitters.clear();
		particle->emitters.shrink_to_fit();
		particle->name = jsonFile.at(name).at("name").get<std::string>();

			for (const auto& emitter : jsonFile.at(name).at("emitters").items())
			{
				Emitter* e = new Emitter(emitter.value().at("name").get<std::string>().c_str());
				e->maxParticles = emitter.value().at("maxParticles");
				e->texture = new R_Texture();
				if (emitter.value().contains("texture_path"))
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
						mDefault->spawnTime = pModule.value().at("spawnTime");
						mDefault->spawnTimer = 0.0f;
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
				if (!e->GetModule<ParticleBillboarding>())
					e->AddModuleByType(ParticleModuleType::BILLBOARDING);

				if (!e->GetModule<EmitterDefault>())
					e->AddModuleByType(ParticleModuleType::DEFAULT);

				particle->emitters.push_back(e);
			}
	}

	return false;
}
