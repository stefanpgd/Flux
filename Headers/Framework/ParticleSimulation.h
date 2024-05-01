#pragma once

#include <string>

#include <d3d12.h>
#include <wrl.h>
using namespace Microsoft::WRL;

class Window;

/// <summary>
/// Base class for any Particle Simulation. It allows to encapsulate the behaviour
/// for both the CPU and GPU in a single class where we can extra data from it that might be
/// relevant to the rendering, editor or other systems without much duplicate code/
/// </summary>
class ParticleSimulation
{
public:
	ParticleSimulation(int particleCount);

	virtual void Start();
	virtual void Update(float deltaTime);
	virtual void Render(ComPtr<ID3D12GraphicsCommandList2> commandList) = 0;

public:
	std::string simulationName;
	unsigned int particleCount;
};