#include "Graphics/RenderStages/PhysarumDiffuseStage.h"
#include "Framework/ParticleSimulations/PhysarumSimulation.h"

#include "Graphics/DXRootSignature.h"
#include "Graphics/DXComputePipeline.h"
#include "Graphics/Texture.h"
#include "Graphics/Window.h"

PhysarumDiffuseStage::PhysarumDiffuseStage(Window* window, Texture* backBuffer, PhysarumSettings* settings)
	: RenderStage(window), backBuffer(backBuffer), settings(settings)
{
	CreatePipeline();
}

void PhysarumDiffuseStage::RecordStage(ComPtr<ID3D12GraphicsCommandList2> commandList)
{
	// 1. Bind pipeline & root signature //
	commandList->SetComputeRootSignature(rootSignature->GetAddress());
	commandList->SetPipelineState(computePipeline->GetAddress());

	// 2. Bind relevant root arguments //
	commandList->SetComputeRootDescriptorTable(0, backBuffer->GetUAV());
	commandList->SetComputeRoot32BitConstants(1, 3, &settings->deltaTime, 0);

	commandList->Dispatch(window->GetWindowWidth(), window->GetWindowHeight(), 1);
}

void PhysarumDiffuseStage::CreatePipeline()
{
	CD3DX12_DESCRIPTOR_RANGE1 backBufferRange[1];
	backBufferRange[0].Init(D3D12_DESCRIPTOR_RANGE_TYPE_UAV, 1, 0, 0, D3D12_DESCRIPTOR_RANGE_FLAG_DATA_VOLATILE);

	CD3DX12_ROOT_PARAMETER1 computeParameters[2];
	computeParameters[0].InitAsDescriptorTable(1, &backBufferRange[0]);
	computeParameters[1].InitAsConstants(3, 0, 0);

	rootSignature = new DXRootSignature(computeParameters, _countof(computeParameters), D3D12_ROOT_SIGNATURE_FLAG_NONE);
	computePipeline = new DXComputePipeline(rootSignature, "Source/Shaders/Physarum/PhysarumDiffuse.compute.hlsl");
}