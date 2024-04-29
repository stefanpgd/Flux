#pragma once

#include "Graphics/RenderStage.h"
#include <string>

class Mesh;
class Texture;

class ScreenStage : public RenderStage
{
public:
	ScreenStage(const std::string& pixelShaderPath, Window* window, Texture* backBuffer);

	void RecordStage(ComPtr<ID3D12GraphicsCommandList2> commandList) override;

private:
	void CreateScreenMesh();
	void CreatePipeline(const std::string& pixelShaderPath);

private:
	Mesh* screenMesh;
	Texture* backBuffer;
};