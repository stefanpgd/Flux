#pragma once

#include "Graphics/RenderStage.h"
#include <string>

class Mesh;
class Texture;

class PresentScreenStage : public RenderStage
{
public:
	PresentScreenStage(const std::string& pixelShaderPath, Texture* backBuffer);

	void RecordStage(ComPtr<ID3D12GraphicsCommandList2> commandList) override;

private:
	void CreateScreenMesh();
	void CreatePipeline(const std::string& pixelShaderPath);

private:
	Mesh* screenMesh;
	Texture* backBuffer;
};