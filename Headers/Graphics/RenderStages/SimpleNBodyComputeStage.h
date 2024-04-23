#pragma once

#include "Graphics/RenderStage.h"

class Texture;
class DXStructuredBuffer;
struct SimpleNBodySettings;

struct Particle
{
	float position[2];
	float velocity[2];
	float mass;
};

class SimpleNBodyComputeStage : public RenderStage
{
public:
	SimpleNBodyComputeStage(Window* window, Texture* backBuffer, SimpleNBodySettings* settings);

	void RecordStage(ComPtr<ID3D12GraphicsCommandList2> commandList) override;

private:
	void InitializeParticles();
	void CreatePipeline();

private:
	unsigned int particleCount;

	Texture* backBuffer;
	SimpleNBodySettings* settings;

	DXStructuredBuffer* particleBuffer;
};