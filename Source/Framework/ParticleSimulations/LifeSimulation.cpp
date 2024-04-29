#include "Framework/ParticleSimulations/LifeSimulation.h"
#include "Framework/ParticleSimulation.h"

#include "Graphics/Texture.h"
#include "Graphics/DXAccess.h"

LifeSimulation::LifeSimulation(unsigned int particleCount)
	: ParticleSimulation(particleCount)
{
	settings.particleCount = particleCount;

	unsigned int* textureBuffer = new unsigned int[1024 * 1024];
	for (int i = 0; i < 1024 * 1024; i++)
	{
		textureBuffer[i] = 0;
	}

	renderBuffer = new Texture(textureBuffer, 1024, 1024, DXGI_FORMAT_R8G8B8A8_UNORM, sizeof(unsigned int));
}

void LifeSimulation::Update(float deltaTime)
{
}

void LifeSimulation::Render(ComPtr<ID3D12GraphicsCommandList2> commandList)
{
}
