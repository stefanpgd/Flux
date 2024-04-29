#include "Framework/ParticleSimulations/LifeSimulation.h"
#include "Framework/ParticleSimulation.h"

#include "Graphics/Texture.h"
#include "Graphics/DXAccess.h"

#include "Graphics/RenderStages/ClearScreenStage.h"
#include "Graphics/RenderStages/LifeParticleStage.h"
#include "Graphics/RenderStages/ScreenStage.h"

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

	clearScreenStage = new ClearScreenStage(renderBuffer);
	screenStage = new ScreenStage("Source/Shaders/Life/screen.pixel.hlsl", renderBuffer);
	particleStage = new LifeParticleStage(renderBuffer, &settings);
}

void LifeSimulation::Update(float deltaTime)
{
}

void LifeSimulation::Render(ComPtr<ID3D12GraphicsCommandList2> commandList)
{
	clearScreenStage->RecordStage(commandList);
	particleStage->RecordStage(commandList);
	screenStage->RecordStage(commandList);
}