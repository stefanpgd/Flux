#pragma once

#include "Framework/ParticleSimulation.h"

class Texture;

struct LifeSimulationSettings
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

	LifeSimulationSettings settings;
};