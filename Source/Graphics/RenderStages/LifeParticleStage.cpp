#include "Graphics/RenderStages/LifeParticleStage.h"
#include "Framework/ParticleSimulations/LifeSimulation.h"

#include "Graphics/DXComputePipeline.h"
#include "Graphics/DXStructuredBuffer.h"
#include "Graphics/DXRootSignature.h"
#include "Graphics/Texture.h"

#include "Utilities/Random.h"
#include "Framework/Mathematics.h"

LifeParticleStage::LifeParticleStage(Texture* backBuffer, LifeSettings* settings)
	: backBuffer(backBuffer), settings(settings)
{
	InitializeParticles();
	CreatePipeline();
}

void LifeParticleStage::RecordStage(ComPtr<ID3D12GraphicsCommandList2> commandList)
{
	// 1. Bind root signature & pipeline state //
	commandList->SetComputeRootSignature(rootSignature->GetAddress());
	commandList->SetPipelineState(computePipeline->GetAddress());

	// 2. Bind root arguments //
	commandList->SetComputeRootDescriptorTable(0, backBuffer->GetUAV());
	commandList->SetComputeRootDescriptorTable(1, particleBuffer->GetUAV());
	commandList->SetComputeRoot32BitConstants(2, 28, settings, 0);

	// 3. Execute particle compute //
	unsigned int dispatchSize = settings->particleCount / 64;
	commandList->Dispatch(dispatchSize, 1, 1);
}

void LifeParticleStage::InitializeParticles()
{
	Particle* particles = new Particle[settings->particleCount];
	for(int i = 0; i < settings->particleCount; i++)
	{
		float r = RandomInRange(5.0f, 500.0f);
		float theta = RandomInRange(0.0f, 3.14159265 * 2.0);

		particles[i].position[0] = 512 + cosf(theta) * r;
		particles[i].position[1] = 512 + sinf(theta) * r;

		glm::vec2 velocity = glm::vec2(RandomInRange(-1.0f, 1.0f), RandomInRange(-1.0f, 1.0f));
		velocity = glm::normalize(velocity);

		particles[i].velocity[0] = velocity.x;
		particles[i].velocity[1] = velocity.y;

		float roll = Random01();

		particles[i].color = 0;

		if(roll < 0.1) // 10% chance for 'Green'
		{
			particles[i].color = 1;
		}
		else if(roll < 0.3)  // 20% chance for 'Blue'
		{
			particles[i].color = 2;
		}
	}

	particleBuffer = new DXStructuredBuffer(particles, settings->particleCount, sizeof(Particle));
}

void LifeParticleStage::CreatePipeline()
{
	CD3DX12_DESCRIPTOR_RANGE1 backBufferRange[1];
	backBufferRange[0].Init(D3D12_DESCRIPTOR_RANGE_TYPE_UAV, 1, 0, 0, D3D12_DESCRIPTOR_RANGE_FLAG_DATA_VOLATILE);

	CD3DX12_DESCRIPTOR_RANGE1 particleRange[1];
	particleRange[0].Init(D3D12_DESCRIPTOR_RANGE_TYPE_UAV, 1, 1, 0, D3D12_DESCRIPTOR_RANGE_FLAG_DATA_VOLATILE);

	CD3DX12_DESCRIPTOR_RANGE1 settingsRange[1];
	settingsRange[0].Init(D3D12_DESCRIPTOR_RANGE_TYPE_UAV, 1, 2, 0, D3D12_DESCRIPTOR_RANGE_FLAG_DATA_VOLATILE);

	CD3DX12_ROOT_PARAMETER1 computeParameters[3];
	computeParameters[0].InitAsDescriptorTable(1, &backBufferRange[0]);
	computeParameters[1].InitAsDescriptorTable(1, &particleRange[0]);
	computeParameters[2].InitAsConstants(28, 0, 0);

	rootSignature = new DXRootSignature(computeParameters, _countof(computeParameters), D3D12_ROOT_SIGNATURE_FLAG_NONE);
	computePipeline = new DXComputePipeline(rootSignature, "Source/Shaders/Life/lifeParticle.compute.hlsl");
}