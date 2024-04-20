#pragma once

#include "Graphics/RenderStage.h"

class Texture;
class DXStructuredBuffer;

struct Particle
{
	float position[2];
	float velocity[2];
	float mass;
};

struct SimulationSettings
{
	float deltaTime = 0.016f;
	float G = 1.0f;
	float maxVelocity = 50.0f;
	float mouseMass = 5000.0f;
	float positionX = 512;
	float positionY = 512;
};

class ParticleComputeStage : public RenderStage
{
public:
	ParticleComputeStage(Window* window, Texture* backBuffer);

	void Update(float deltaTime);
	void RecordStage(ComPtr<ID3D12GraphicsCommandList2> commandList) override;

private:
	void InitializeParticles();
	void CreatePipeline();

private:
	unsigned int particleCount;

	Texture* backBuffer;
	SimulationSettings settings;

	DXStructuredBuffer* particleBuffer;
	DXStructuredBuffer* settingsBuffer;
};