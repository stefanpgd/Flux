#pragma once

#include "Graphics/RenderStage.h"

class Texture;
class DXStructuredBuffer;
struct LifeSettings;

struct Particle
{
	float position[2];
	float velocity[2];
	int color;
};

class LifeParticleStage : public RenderStage
{
public:
	LifeParticleStage(Window* window, Texture* backBuffer, LifeSettings* settings);

	void RecordStage(ComPtr<ID3D12GraphicsCommandList2> commandList) override;

private:
	void InitializeParticles();
	void CreatePipeline();

private:
	LifeSettings* settings;

	Texture* backBuffer;
	DXStructuredBuffer* particleBuffer;
};