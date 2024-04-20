#pragma once

#include "Graphics/RenderStage.h"

class Texture;
class DXStructuredBuffer;

struct Particle
{
	float position[2];
};

class ParticleComputeStage : public RenderStage
{
public:
	ParticleComputeStage(Window* window, Texture* backBuffer, unsigned int particleCount);

	void RecordStage(ComPtr<ID3D12GraphicsCommandList2> commandList) override;

private:
	void InitializeParticles();
	void CreatePipeline();

private:
	unsigned int particleCount;

	Texture* backBuffer;
	DXStructuredBuffer* particleBuffer;
};