#pragma once

#include "Framework/ParticleSimulation.h"

class Texture;
class Window;

class PhysarumComputeStage;
class PhysarumDiffuseStage;
class PhysarumScreenStage;

struct PhysarumSettings
{
	unsigned int particleCount;
	float deltaTime = 1.0f;

	float trailDissolveSpeed = 0.35f;
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
	PhysarumComputeStage* agentStage;
	PhysarumDiffuseStage* diffuseStage;
	PhysarumScreenStage* screenStage;

	PhysarumSettings settings;
};