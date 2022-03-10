#include "ParticleModule.h"
#include <vector>
#include "MathGeoLib/Algorithm/Random/LCG.h"
#include "MathGeoLib/Math/MathFunc.h"

ParticleModule::ParticleModule()
{
	//if (emitter != nullptr)
	//{
	//	emitter->AddModule(this);
	//}
}

ParticleModule::~ParticleModule()
{}

float ParticleModule::GetPercentage(Particle* p)
{
	return (p->lifeTime / p->maxLifetime);
}

// MODULES---------------------------------------------------------------

EmitterDefault::EmitterDefault()
{
	type = ParticleModuleType::DEFAULT;
}

void EmitterDefault::Spawn(Particle* particle, EmitterInstance* emitter)
{
	particle->maxLifetime = initialLifetime;
	particle->lifeTime = 0.0f;
}

bool EmitterDefault::Update(float dt, EmitterInstance* emitter)
{
	if (disable)
	{
		return true;
	}
	timer += dt;
	if (timer >= spawnTime)
	{
		emitter->SpawnParticle();
		timer = 0;
	}
	for (std::vector<Particle*>::iterator it = emitter->particles.begin(); it < emitter->particles.end(); ++it)
	{
		(*it)->lifeTime += dt;
	}
}

EmitterMovement::EmitterMovement()
{
	type = ParticleModuleType::MOVEMENT;
}

void EmitterMovement::Spawn(Particle* particle, EmitterInstance* emitter)
{
	LCG random;
	particle->direction = particle->direction.Lerp(initialDirection, finalDirection, random.Float());
	particle->intensity = math::Lerp(initialIntensity,finalIntensity,random.Float());
	particle->velocity = particle->direction * particle->intensity;
	particle->acceleration = particle->acceleration.Lerp(initialAcceleration, finalAcceleration, random.Float());
}

bool EmitterMovement::Update(float dt, EmitterInstance* emitter)
{
	if (disable)
	{
		return true;
	}
	for (std::vector<Particle*>::iterator it = emitter->particles.begin(); it < emitter->particles.end(); ++it)
	{
		(*it)->velocity += (*it)->acceleration;
		(*it)->position += (*it)->velocity * dt;
	}
}

EmitterColor::EmitterColor()
{
	type = ParticleModuleType::COLOR;
}

void EmitterColor::Spawn(Particle* particle, EmitterInstance* emitter)
{
	particle->CurrentColor = ColorLerp(GetPercentage(particle));
}

bool EmitterColor::Update(float dt, EmitterInstance* emitter)
{
	if (disable)
	{
		return true;
	}
	for (std::vector<Particle*>::iterator it = emitter->particles.begin(); it < emitter->particles.end(); ++it)
	{
		(*it)->CurrentColor = ColorLerp(GetPercentage((*it)));
	}
}

Color EmitterColor::ColorLerp(float current)
{
	for (int i = 0; i < colorOverTime.size(); ++i)
	{
		if (colorOverTime.at(i).pos == current)
		{
			return colorOverTime.at(i).color;
		}
		else if (i = 0 && colorOverTime.at(i).pos > current)
		{
			return colorOverTime.at(i).color;
		}
		else if (i = colorOverTime.size() && colorOverTime.at(i).pos < current)
		{
			return colorOverTime.at(i).color;
		}
		else if (colorOverTime.at(i).pos > current && colorOverTime.at(i-1).pos < current)
		{
			float r = math::Lerp(colorOverTime.at(i - 1).color.r, colorOverTime.at(i).color.r, current);
			float g = math::Lerp(colorOverTime.at(i - 1).color.r, colorOverTime.at(i).color.r, current);
			float b = math::Lerp(colorOverTime.at(i - 1).color.r, colorOverTime.at(i).color.r, current);
			float a = math::Lerp(colorOverTime.at(i - 1).color.r, colorOverTime.at(i).color.r, current);
			return Color(r, g, b, a);
		}
	}
	return Color(1.0f,1.0f,1.0f,1.0f);
}

bool EmitterColor::EditColor(FadeColor& color, uint pos)
{
	bool ret = true;
	ImVec4 vecColor = ImVec4(color.color.r, color.color.g, color.color.b, color.color.a);
	//if (ImGui::ColorButton("Color", vecColor, ImGuiColorEditFlags_None, ImVec2(100, 20)));


	//ImGui::SameLine();
	//ImGui::TextUnformatted("Color");
	if (pos > 0)
	{
		std::string colorStr = "Remove Color ";
		colorStr.append(std::to_string(pos));
		if (ImGui::Button(colorStr.c_str(), ImVec2(125, 25))) ret = false;
	}

	ImGui::ColorEdit4("Color", &color.color.a, ImGuiColorEditFlags_AlphaBar);

	return ret;
}

EmitterSize::EmitterSize()
{
	type = ParticleModuleType::SIZE;
}

void EmitterSize::Spawn(Particle* particle, EmitterInstance* emitter)
{
	LCG random;
	particle->scale = particle->scale.Lerp(initialSize, finalSize, random.Float());
}

bool EmitterSize::Update(float dt, EmitterInstance* emitter)
{
	if (disable)
	{
		return true;
	}
	for (std::vector<Particle*>::iterator it = emitter->particles.begin(); it < emitter->particles.end(); ++it)
	{
		float p = GetPercentage((*it));
		(*it)->scale = (*it)->scale.Lerp(initialSize, finalSize, p);
	}
}