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

class PhysarumAgentStage : public RenderStage
{
public:
	PhysarumAgentStage(Texture* backBuffer, PhysarumSettings* settings);

	void RecordStage(ComPtr<ID3D12GraphicsCommandList2> commandList) override;

private:
	void InitializeParticles();
	void CreatePipeline();

private:
	Texture* backBuffer;
	PhysarumSettings* settings;

	DXStructuredBuffer* agentBuffer;
};