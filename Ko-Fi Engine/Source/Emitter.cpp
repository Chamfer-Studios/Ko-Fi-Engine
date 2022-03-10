#include "Emitter.h"

Emitter::Emitter()
{
	texture = new Texture();
	path = "Assets\Textures\firework_projectile.png";
	Importer::GetInstance()->textureImporter->Import(path.c_str(), texture);
	CreateModule<EmitterDefault>();
}

Emitter::~Emitter()
{
	texture = nullptr;
}

bool Emitter::Update(float dt)
{
	return true;
}

bool Emitter::Cleanup()
{
	return true;
}

void Emitter::AddModule(ParticleModule* m)
{
	modules.push_back(m);
}

void Emitter::DeleteModule(ParticleModule* m)
{
	for (std::vector<ParticleModule*>::iterator it = modules.begin(); it < modules.end(); ++it)
	{
		if ((*it) == m)
		{
			modules.erase(it);
			modules.shrink_to_fit();
		}
	}
}