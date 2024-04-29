#pragma once

#include "Graphics/RenderStage.h"

class Texture;
class DXStructuredBuffer;
struct PhysarumSettings;

struct Agent
{
	float position[2];
	float angle; 
};

// TODO: Rename to PhysarumAgentStage
class PhysarumComputeStage : public RenderStage
{
public:
	PhysarumComputeStage(Texture* backBuffer, PhysarumSettings* settings);

	void RecordStage(ComPtr<ID3D12GraphicsCommandList2> commandList) override;

private:
	void InitializeParticles();
	void CreatePipeline();

private:
	Texture* backBuffer;
	PhysarumSettings* settings;

	DXStructuredBuffer* agentBuffer;
};