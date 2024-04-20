#include "Graphics/RenderStages/ClearBufferStage.h"
#include "Graphics/DXRootSignature.h"
#include "Graphics/DXComputePipeline.h"
#include "Graphics/Window.h"
#include "Graphics/Texture.h"

#include <d3dx12.h>

ClearBufferStage::ClearBufferStage(Window* window, Texture* backBuffer) : RenderStage(window), backBuffer(backBuffer)
{
	CreatePipeline();
}

void ClearBufferStage::RecordStage(ComPtr<ID3D12GraphicsCommandList2> commandList)
{
	commandList->SetComputeRootSignature(rootSignature->GetAddress());
	commandList->SetPipelineState(computePipeline->GetAddress());

	int threadWidth = window->GetWindowWidth() / 4;
	int threadHeight = window->GetWindowWidth() / 4;

	commandList->SetComputeRootDescriptorTable(0, backBuffer->GetUAV());

	commandList->Dispatch(threadWidth, threadHeight, 1);
}

void ClearBufferStage::CreatePipeline()
{
	CD3DX12_DESCRIPTOR_RANGE1 textureRange[1];
	textureRange[0].Init(D3D12_DESCRIPTOR_RANGE_TYPE_UAV, 1, 0, 0, D3D12_DESCRIPTOR_RANGE_FLAG_DATA_VOLATILE);

	CD3DX12_ROOT_PARAMETER1 clearRootParameters[1];
	clearRootParameters[0].InitAsDescriptorTable(1, &textureRange[0]);

	rootSignature = new DXRootSignature(clearRootParameters, _countof(clearRootParameters));
	computePipeline = new DXComputePipeline(rootSignature, "Source/Shaders/screenClear.compute.hlsl");
}