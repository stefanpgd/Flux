#pragma once
#include <string>
#include <vector>
#include <d3d12.h>
#include <wrl.h>
using namespace Microsoft::WRL;

class ParticleSimulation;

class Renderer
{
public:
	Renderer(const std::wstring& applicationName, unsigned int windowWidth, unsigned int windowHeight);
	
	void Render();
	void Resize();

	void SetParticleSimulation(ParticleSimulation* simulation);

private:
	void InitializeImGui();

private:
	ParticleSimulation* activeSimulation;
};