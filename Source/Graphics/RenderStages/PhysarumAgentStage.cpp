#include "Graphics/RenderStages/PhysarumAgentStage.h"
#include "Framework/ParticleSimulations/PhysarumSimulation.h"
#include "Framework/Mathematics.h"

#include "Graphics/DXStructuredBuffer.h"
#include "Graphics/DXRootSignature.h"
#include "Graphics/DXComputePipeline.h"
#include "Graphics/Texture.h"

#include "Utilities/Random.h"

PhysarumAgentStage::PhysarumAgentStage(Texture* backBuffer, PhysarumSettings* settings)
	: backBuffer(backBuffer), settings(settings)
{
	InitializeParticles();
	CreatePipeline();
}

void PhysarumAgentStage::RecordStage(ComPtr<ID3D12GraphicsCommandList2> commandList)
{
	// 1. Bind root signature & pipeline state //
	commandList->SetComputeRootSignature(rootSignature->GetAddress());
	commandList->SetPipelineState(computePipeline->GetAddress());

	// 2. Bind root arguments //
	commandList->SetComputeRootDescriptorTable(0, backBuffer->GetUAV());
	commandList->SetComputeRootDescriptorTable(1, agentBuffer->GetUAV());
	commandList->SetComputeRoot32BitConstants(2, 7, settings, 0);

	// 3. Execute particle compute //
	unsigned int dispatchSize = settings->particleCount / 64;
	commandList->Dispatch(dispatchSize, 1, 1);
}

void PhysarumAgentStage::InitializeParticles()
{
	Agent* agents = new Agent[settings->particleCount];

	for(int i = 0; i < settings->particleCount; i++)
	{
		float r = RandomInRange(5.0f, 50.0f);
		float theta = RandomInRange(0.0f, 3.14159265 * 2.0);

		agents[i].position[0] = 512;
		agents[i].position[1] = 512;

		agents[i].angle = r;
	}

	agentBuffer = new DXStructuredBuffer(agents, settings->particleCount, sizeof(Agent));
}

void PhysarumAgentStage::CreatePipeline()
{
	CD3DX12_DESCRIPTOR_RANGE1 backBufferRange[1];
	backBufferRange[0].Init(D3D12_DESCRIPTOR_RANGE_TYPE_UAV, 1, 0, 0, D3D12_DESCRIPTOR_RANGE_FLAG_DATA_VOLATILE);

	CD3DX12_DESCRIPTOR_RANGE1 agentRange[1];
	agentRange[0].Init(D3D12_DESCRIPTOR_RANGE_TYPE_UAV, 1, 1, 0, D3D12_DESCRIPTOR_RANGE_FLAG_DATA_VOLATILE);

	CD3DX12_DESCRIPTOR_RANGE1 settingsRange[1];
	settingsRange[0].Init(D3D12_DESCRIPTOR_RANGE_TYPE_UAV, 1, 2, 0, D3D12_DESCRIPTOR_RANGE_FLAG_DATA_VOLATILE);

	CD3DX12_ROOT_PARAMETER1 computeParameters[3];
	computeParameters[0].InitAsDescriptorTable(1, &backBufferRange[0]);
	computeParameters[1].InitAsDescriptorTable(1, &agentRange[0]);
	computeParameters[2].InitAsConstants(7, 0, 0);

	rootSignature = new DXRootSignature(computeParameters, _countof(computeParameters), D3D12_ROOT_SIGNATURE_FLAG_NONE);
	computePipeline = new DXComputePipeline(rootSignature, "Source/Shaders/Physarum/PhysarumAgents.compute.hlsl");
}