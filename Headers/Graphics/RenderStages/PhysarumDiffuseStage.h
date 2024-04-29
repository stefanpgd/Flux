#pragma once

#include "Graphics/RenderStage.h"

class Texture;
struct PhysarumSettings;

class PhysarumDiffuseStage : public RenderStage
{
public:
	PhysarumDiffuseStage(Texture* backBuffer, PhysarumSettings* settings);

	void RecordStage(ComPtr<ID3D12GraphicsCommandList2> commandList) override;

private:
	void CreatePipeline();

private:
	Texture* backBuffer;
	PhysarumSettings* settings;
};