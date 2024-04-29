#pragma once

#include "Graphics/RenderStage.h"

class Texture;
struct SimpleNBodySettings;

class NBodyClearStage : public RenderStage
{
public:
	NBodyClearStage(Texture* backBuffer, float* trailStrength, float* trailCutoffOpacity);

	void RecordStage(ComPtr<ID3D12GraphicsCommandList2> commandList) override;

private:
	void CreatePipeline();

private:
	Texture* backBuffer;

	// Editable settings //
	float* trailStrength;
	float* trailCutoffOpacity;
};