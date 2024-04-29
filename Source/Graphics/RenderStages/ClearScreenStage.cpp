#include "Graphics/RenderStages/ClearScreenStage.h"
#include "Graphics/DXRootSignature.h"
#include "Graphics/DXComputePipeline.h"
#include "Graphics/Window.h"
#include "Graphics/Texture.h"

ClearScreenStage::ClearScreenStage(Texture* targetTexture) : targetTexture(targetTexture)
{
	CreatePipeline();
}

void ClearScreenStage::RecordStage(ComPtr<ID3D12GraphicsCommandList2> commandList)
{
	// 1. Bind pipeline & root signature //
	commandList->SetComputeRootSignature(rootSignature->GetAddress());
	commandList->SetPipelineState(computePipeline->GetAddress());

	// 2. Bind relevant root arguments //
	commandList->SetComputeRootDescriptorTable(0, targetTexture->GetUAV());

	// 3. Clear (back buffer) texture // 
	int threads = window->GetWindowWidth() / 64;
	commandList->Dispatch(threads, threads, 1);
}

void ClearScreenStage::CreatePipeline()
{
	CD3DX12_DESCRIPTOR_RANGE1 textureRange[1];
	textureRange[0].Init(D3D12_DESCRIPTOR_RANGE_TYPE_UAV, 1, 0, 0, D3D12_DESCRIPTOR_RANGE_FLAG_DATA_VOLATILE);

	CD3DX12_ROOT_PARAMETER1 clearRootParameters[1];
	clearRootParameters[0].InitAsDescriptorTable(1, &textureRange[0]);

	rootSignature = new DXRootSignature(clearRootParameters, _countof(clearRootParameters));
	computePipeline = new DXComputePipeline(rootSignature, "Source/Shaders/clearScreen.compute.hlsl");
}