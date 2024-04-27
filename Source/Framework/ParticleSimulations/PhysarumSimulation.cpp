#include "Framework/ParticleSimulations/PhysarumSimulation.h"

#include "Graphics/Texture.h"
#include "Graphics/DXAccess.h"

#include "Graphics/RenderStages/PhysarumScreenStage.h"

PhysarumSimulation::PhysarumSimulation(unsigned int particleCount) : ParticleSimulation(particleCount)
{
	settings.particleCount = particleCount;

	unsigned int* textureBuffer = new unsigned int[1024 * 1024];
	renderBuffer = new Texture(textureBuffer, 1024, 1024, DXGI_FORMAT_R8G8B8A8_UNORM, sizeof(unsigned int));
}

void PhysarumSimulation::Update(float deltaTime)
{

}

void PhysarumSimulation::Render(ComPtr<ID3D12GraphicsCommandList2> commandList)
{

}