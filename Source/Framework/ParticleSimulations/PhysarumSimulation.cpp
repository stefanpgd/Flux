#include "Framework/ParticleSimulations/PhysarumSimulation.h"

#include "Graphics/Texture.h"
#include "Graphics/DXAccess.h"

#include "Graphics/RenderStages/PhysarumDiffuseStage.h"
#include "Graphics/RenderStages/PhysarumComputeStage.h"
#include "Graphics/RenderStages/PhysarumScreenStage.h"

PhysarumSimulation::PhysarumSimulation(unsigned int particleCount) : ParticleSimulation(particleCount)
{
	settings.particleCount = particleCount;

	unsigned int* textureBuffer = new unsigned int[1024 * 1024];
	for(int i = 0; i < 1024 * 1024; i++)
	{
		textureBuffer[i] = 0;
	}

	renderBuffer = new Texture(textureBuffer, 1024, 1024, DXGI_FORMAT_R8G8B8A8_UNORM, sizeof(unsigned int));
	
	// TODO: Since every render stage needs window anyway, moving it out of the parameters
	// into the base class consutrctor is probably the way to go
	Window* window = DXAccess::GetWindow();

	agentStage = new PhysarumComputeStage(window, renderBuffer, &settings);
	diffuseStage = new PhysarumDiffuseStage(window, renderBuffer, &settings);
	screenStage = new PhysarumScreenStage(window, renderBuffer);
}

void PhysarumSimulation::Update(float deltaTime)
{
	settings.deltaTime = deltaTime;
}

void PhysarumSimulation::Render(ComPtr<ID3D12GraphicsCommandList2> commandList)
{
	agentStage->RecordStage(commandList);
	diffuseStage->RecordStage(commandList);
	screenStage->RecordStage(commandList);
}