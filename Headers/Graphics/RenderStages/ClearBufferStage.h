#pragma once

#include "Graphics/RenderStage.h"

class Texture;

class ClearBufferStage : public RenderStage
{
public:
	ClearBufferStage(Window* window, Texture* backBuffer);

	void RecordStage(ComPtr<ID3D12GraphicsCommandList2> commandList) override;

private:
	void CreatePipeline();

private:
	Texture* backBuffer;
};