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

class ParticleComputeStage : public RenderStage
{
public:
	ParticleComputeStage(Window* window, Texture* backBuffer);

	void RecordStage(ComPtr<ID3D12GraphicsCommandList2> commandList) override;

private:
	void InitializeParticles();
	void CreatePipeline();

private:
	unsigned int particleCount;

	Texture* backBuffer;
	DXStructuredBuffer* particleBuffer;
};