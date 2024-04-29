#pragma once

#include "Framework/ParticleSimulation.h"

class Texture;

class ClearScreenStage;
class LifeParticleStage;
class ScreenStage;

struct LifeSettings
{
	unsigned int particleCount;
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