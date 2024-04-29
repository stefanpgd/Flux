#pragma once

#include "Framework/ParticleSimulation.h"

class Texture;
class Window;

class NBodyClearStage;
class NBodyComputeStage;
class ScreenStage;

struct NBodySettings
{
	unsigned int particleCount = 0;
	float deltaTime = 0.016f;
	float G = 1.0f;
	float maxVelocity = 125.0f;
	
	float trailStrength = 0.925f;
	float trailCutoffOpacity = 0.1f;
};

class NBodySimulation : public ParticleSimulation
{
public:
	NBodySimulation(int particleCount);

	void Update(float deltaTime) override;
	void Render(ComPtr<ID3D12GraphicsCommandList2> commandList) override;

private:
	Texture* renderBuffer;

	// Rendering Stages for this Simulation //
	NBodyClearStage* clearBufferStage;
	NBodyComputeStage* nBodyComputeStage;
	ScreenStage* screenStage;

	NBodySettings settings;
};