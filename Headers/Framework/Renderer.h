#pragma once
#include <string>
#include <vector>
#include <d3d12.h>
#include <wrl.h>
using namespace Microsoft::WRL;

class ClearBufferStage;
class ParticleComputeStage;
class ScreenStage;
class Texture;

class Renderer
{
public:
	Renderer(const std::wstring& applicationName, unsigned int windowWidth, unsigned int windowHeight);

	void Update(float deltaTime);
	void Render();

	void Resize();

private:
	void InitializeImGui();

private:
	ClearBufferStage* clearBufferStage;
	ParticleComputeStage* particleComputeStage;
	ScreenStage* screenStage;

	Texture* computeBackBuffer;
};