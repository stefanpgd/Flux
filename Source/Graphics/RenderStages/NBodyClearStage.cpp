#include "Graphics/RenderStages/NBodyClearStage.h"
#include "Graphics/DXRootSignature.h"
#include "Graphics/DXComputePipeline.h"
#include "Graphics/Window.h"
#include "Graphics/Texture.h"

#include "Framework/ParticleSimulations/SimpleNBodySimulation.h"

#include <d3dx12.h>

NBodyClearStage::NBodyClearStage(Window* window, Texture* backBuffer, float* trailStrength, float* trailCutoffOpacity)
	: RenderStage(window), backBuffer(backBuffer), trailStrength(trailStrength), trailCutoffOpacity(trailCutoffOpacity)
{
	CreatePipeline();
}

void NBodyClearStage::RecordStage(ComPtr<ID3D12GraphicsCommandList2> commandList)
{
	// 1. Bind pipeline & root signature //
	commandList->SetComputeRootSignature(rootSignature->GetAddress());
	commandList->SetPipelineState(computePipeline->GetAddress());

	// 2. Bind relevant root arguments //
	commandList->SetComputeRootDescriptorTable(0, backBuffer->GetUAV());
	commandList->SetComputeRoot32BitConstants(1, 1, trailStrength, 0);
	commandList->SetComputeRoot32BitConstants(1, 1, trailCutoffOpacity, 1);

	// 3. Clear (back buffer) texture // 
	int threads = window->GetWindowWidth() / 4;
	commandList->Dispatch(threads, threads, 1);
}

void NBodyClearStage::CreatePipeline()
{
	CD3DX12_DESCRIPTOR_RANGE1 textureRange[1];
	textureRange[0].Init(D3D12_DESCRIPTOR_RANGE_TYPE_UAV, 1, 0, 0, D3D12_DESCRIPTOR_RANGE_FLAG_DATA_VOLATILE);

	CD3DX12_ROOT_PARAMETER1 clearRootParameters[2];
	clearRootParameters[0].InitAsDescriptorTable(1, &textureRange[0]);
	clearRootParameters[1].InitAsConstants(2, 0, 0);

	rootSignature = new DXRootSignature(clearRootParameters, _countof(clearRootParameters));
	computePipeline = new DXComputePipeline(rootSignature, "Source/Shaders/NBody/screenClear.compute.hlsl");
}