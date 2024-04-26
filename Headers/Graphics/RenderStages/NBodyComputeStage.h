#pragma once

#include "Graphics/RenderStage.h"

class Texture;
class DXStructuredBuffer;
struct NBodySettings;

// TODO: particle should likely be part of the simulation itself
// since that defines what the simulation is and what it should act like..
// so move this out soon-ish

struct Particle
{
	float position[2];
	float velocity[2];
	float mass;
};

class NBodyComputeStage : public RenderStage
{
public:
	NBodyComputeStage(Window* window, Texture* backBuffer, NBodySettings* settings);

	void RecordStage(ComPtr<ID3D12GraphicsCommandList2> commandList) override;

private:
	void InitializeParticles();
	void CreatePipeline();

private:
	Texture* backBuffer;
	NBodySettings* settings;

	DXStructuredBuffer* particleBuffer;
};