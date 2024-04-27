#pragma once

#include "Graphics/RenderStage.h"

class Mesh;
class Texture;

class NBodyScreenStage : public RenderStage
{
public:
	NBodyScreenStage(Window* window, Texture* backBuffer);

	void RecordStage(ComPtr<ID3D12GraphicsCommandList2> commandList) override;

private:
	void CreateScreenMesh();
	void CreatePipeline();

private:
	Mesh* screenMesh;
	Texture* backBuffer;
};

