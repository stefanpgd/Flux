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

	float agentSpeed = 125.0f;
	float agentTurnSpeed = 72.0f;

	float agentSensorAngle = 0.2;
	float agentSensorDistance = 12.0;
	int agentSensorSize = 1;

	float trailDissolveSpeed = 0.53f;
	float trailDiffuseSpeed = 12.5f;
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