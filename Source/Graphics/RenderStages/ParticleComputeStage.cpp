#include "Graphics/RenderStages/ParticleComputeStage.h"
#include "Graphics/DXComputePipeline.h"
#include "Graphics/DXStructuredBuffer.h"
#include "Graphics/DXRootSignature.h"
#include "Graphics/Texture.h"

#include <glm.hpp>
#include "Utilities/Random.h"
#include "Utilities/Logger.h"

ParticleComputeStage::ParticleComputeStage(Window* window, Texture* backBuffer) 
	: RenderStage(window), backBuffer(backBuffer)
{
	particleCount = 4096;

	InitializeParticles();
	CreatePipeline();
}

void ParticleComputeStage::RecordStage(ComPtr<ID3D12GraphicsCommandList2> commandList)
{
	// 1. Bind root signature & pipeline state //
	commandList->SetComputeRootSignature(rootSignature->GetAddress());
	commandList->SetPipelineState(computePipeline->GetAddress());

	// 2. Bind root arguments //
	commandList->SetComputeRootDescriptorTable(0, backBuffer->GetUAV());
	commandList->SetComputeRootDescriptorTable(1, particleBuffer->GetUAV());

	// 3. Execute particle compute //
	unsigned int dispatchSize = particleCount / 16;
	if(dispatchSize % 16 != 0)
	{
		LOG(Log::MessageType::Error, "Dispatch size is not divisble by 16");
	}

	commandList->Dispatch(particleCount, 1, 1);
}

void ParticleComputeStage::InitializeParticles()
{
	Particle* particles = new Particle[particleCount];
	for(int i = 0; i < particleCount; i++)
	{
		particles[i].position[0] = rand() % 1024;
		particles[i].position[1] = rand() % 1024;

		glm::vec2 velocity = glm::vec2(RandomInRange(-1.0f, 1.0f), RandomInRange(-1.0f, 1.0f));
		velocity = glm::normalize(velocity);

		particles[i].velocity[0] = velocity.x;
		particles[i].velocity[1] = velocity.y;
	}

	particleBuffer = new DXStructuredBuffer(particles, particleCount, sizeof(Particle));
}

void ParticleComputeStage::CreatePipeline()
{
	CD3DX12_DESCRIPTOR_RANGE1 computeRange[1];
	computeRange[0].Init(D3D12_DESCRIPTOR_RANGE_TYPE_UAV, 1, 0, 0, D3D12_DESCRIPTOR_RANGE_FLAG_DATA_VOLATILE);

	CD3DX12_DESCRIPTOR_RANGE1 dataRange[1];
	dataRange[0].Init(D3D12_DESCRIPTOR_RANGE_TYPE_UAV, 1, 1, 0, D3D12_DESCRIPTOR_RANGE_FLAG_DATA_VOLATILE);

	CD3DX12_ROOT_PARAMETER1 computeParameters[2];
	computeParameters[0].InitAsDescriptorTable(1, &computeRange[0]);
	computeParameters[1].InitAsDescriptorTable(1, &dataRange[0]);

	rootSignature = new DXRootSignature(computeParameters, _countof(computeParameters), D3D12_ROOT_SIGNATURE_FLAG_NONE);
	computePipeline = new DXComputePipeline(rootSignature, "Source/Shaders/particle.compute.hlsl");
}