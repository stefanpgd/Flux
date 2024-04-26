#pragma once

#include "Framework/ParticleSimulation.h"

class Texture;
class Window;

class ClearBufferStage;
class SimpleNBodyComputeStage;
class ScreenStage;

struct SimpleNBodySettings
{
	unsigned int particleCount;
	float deltaTime = 0.016f;
	float G = 1.0f;
	float maxVelocity = 100.0f;
	float mouseMass = 5000.0f;
	float positionX = 512;
	float positionY = 512;

	float trailStrength = 0.9f;
	float trailCutoffOpacity = 0.1f; 
};

// TODO: Depending if we end up having full Nbody, maybe rename or not 
class SimpleNBodySimulation : public ParticleSimulation
{
public:
	SimpleNBodySimulation(int particleCount);

	void Update(float deltaTime) override;
	void Render(ComPtr<ID3D12GraphicsCommandList2> commandList) override;

	// TODO: Add functionality to be able to restart the simulation

private:
	// Our output will be written to this //
	Texture* renderBuffer;

	// Rendering Stages for this simulation //
	ClearBufferStage* clearBufferStage;
	SimpleNBodyComputeStage* nBodyComputeStage;
	ScreenStage* screenStage;

	SimpleNBodySettings settings;
};