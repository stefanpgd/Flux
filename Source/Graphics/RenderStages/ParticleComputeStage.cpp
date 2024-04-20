#include "Graphics/RenderStages/ParticleComputeStage.h"
#include "Graphics/DXComputePipeline.h"
#include "Graphics/DXStructuredBuffer.h"
#include "Graphics/DXRootSignature.h"
#include "Graphics/Texture.h"

ParticleComputeStage::ParticleComputeStage(Window* window, Texture* backBuffer, 
	unsigned int particleCount) : RenderStage(window), backBuffer(backBuffer), particleCount(particleCount)
{
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
	commandList->Dispatch(64, 1, 1);
}

void ParticleComputeStage::InitializeParticles()
{
	Particle* particles = new Particle[particleCount];
	for(int i = 0; i < particleCount; i++)
	{
		particles[i].position[0] = rand() % 1024;
		particles[i].position[1] = rand() % 1024;
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