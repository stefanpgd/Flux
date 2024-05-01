#pragma once

#include "Graphics/RenderStage.h"

class Texture;
class DXStructuredBuffer;
struct NBodySettings;

struct Particle
{
	float position[2];
	float velocity[2];
	float mass;
};

class NBodyComputeStage : public RenderStage
{
public:
	NBodyComputeStage(Texture* backBuffer, NBodySettings* settings);

	void RecordStage(ComPtr<ID3D12GraphicsCommandList2> commandList) override;

private:
	void InitializeParticles();
	void CreatePipeline();

private:
	Texture* backBuffer;
	NBodySettings* settings;

	DXStructuredBuffer* particleBuffer;
};