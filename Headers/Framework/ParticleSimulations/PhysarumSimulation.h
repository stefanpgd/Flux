#pragma once

#include "Framework/ParticleSimulation.h"

class Texture;
class Window;

class PhysarumScreenStage;
class PhysarumComputeStage;

struct PhysarumSettings
{
	unsigned int particleCount;
	float deltaTime = 1.0f;
};

class PhysarumSimulation : public ParticleSimulation
{
public:
	PhysarumSimulation(unsigned int particleCount);

	void Update(float deltaTime) override;
	void Render(ComPtr<ID3D12GraphicsCommandList2> commandList) override;

private:
	// Output buffer //
	Texture* renderBuffer;

	// Render stages //
	PhysarumScreenStage* screenStage;
	PhysarumComputeStage* agentStage;

	PhysarumSettings settings;
};