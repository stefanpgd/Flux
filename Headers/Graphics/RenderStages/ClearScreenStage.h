#pragma once

#include "Graphics/RenderStage.h"

class Texture;

class ClearScreenStage : public RenderStage
{
public:
	ClearScreenStage(Window* window, Texture* targetTexture);

	void RecordStage(ComPtr<ID3D12GraphicsCommandList2> commandList) override;

private:
	void CreatePipeline();

private:
	Texture* targetTexture;
};