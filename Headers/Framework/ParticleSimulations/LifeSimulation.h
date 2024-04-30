#pragma once

#include "Framework/ParticleSimulation.h"
#include <glm.hpp>

class Texture;

class ClearScreenStage;
class LifeParticleStage;
class ScreenStage;

struct LifeSettings
{
	unsigned int particleCount;
	float deltaTime = 1.0f;

	float maxDistance = 10.0f;
	float friction = 2.5f;

	glm::vec4 attractionMatrix[2][2];
	glm::vec4 cellColors[2];
};

class LifeSimulation : public ParticleSimulation
{
public:
	LifeSimulation(unsigned int particleCount);

	void Update(float deltaTime) override;
	void Render(ComPtr<ID3D12GraphicsCommandList2> commandList) override;

private:
	// Output Buffer //
	Texture* renderBuffer;

	ClearScreenStage* clearScreenStage;
	LifeParticleStage* particleStage;
	ScreenStage* screenStage;

	LifeSettings settings;
};