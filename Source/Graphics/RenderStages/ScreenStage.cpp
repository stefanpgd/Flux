#include "Graphics/RenderStages/ScreenStage.h"

#include "Graphics/Mesh.h"
#include "Graphics/Texture.h"
#include "Graphics/DXPipeline.h"
#include "Graphics/DXRootSignature.h"
#include "Graphics/DXUtilities.h"

ScreenStage::ScreenStage(const std::string& pixelShaderPath, Window* window, Texture* backBuffer)
	: RenderStage(window), backBuffer(backBuffer)
{
	CreateScreenMesh();
	CreatePipeline(pixelShaderPath);
}

// Todo: Consider maybe directly accessing the backbuffers from the swap-chain
// So the need for a graphics/rasterization pipeline might falter
void ScreenStage::RecordStage(ComPtr<ID3D12GraphicsCommandList2> commandList)
{
	// 0. Grab relevant resources for render call //
	CD3DX12_CPU_DESCRIPTOR_HANDLE backBufferRTV = window->GetCurrentScreenRTV();
	CD3DX12_CPU_DESCRIPTOR_HANDLE depthView = window->GetDepthDSV();

	// 1. Prepare & Bind the render target //
	TransitionResource(window->GetCurrentScreenBuffer().Get(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);
	BindAndClearRenderTarget(window, &backBufferRTV, &depthView);

	// 2. Bind root signature & pipeline //
	commandList->SetGraphicsRootSignature(rootSignature->GetAddress());
	commandList->SetPipelineState(pipeline->GetAddress());

	// 3. Define relevant buffers & settings for the Input Assembler //
	commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	commandList->IASetVertexBuffers(0, 1, &screenMesh->GetVertexBufferView());
	commandList->IASetIndexBuffer(&screenMesh->GetIndexBufferView());

	// 4. Set the root arguments //
	commandList->SetGraphicsRootDescriptorTable(0, backBuffer->GetSRV());

	// 5. Draw screen quad //
	commandList->DrawIndexedInstanced(screenMesh->GetIndicesCount(), 1, 0, 0, 0);
}

void ScreenStage::CreateScreenMesh()
{
	Vertex* screenVertices = new Vertex[4];
	screenVertices[0].Position = glm::vec3(-1.0f, -1.0f, 0.0f);
	screenVertices[1].Position = glm::vec3(-1.0f, 1.0f, 0.0f);
	screenVertices[2].Position = glm::vec3(1.0f, 1.0f, 0.0f);
	screenVertices[3].Position = glm::vec3(1.0f, -1.0f, 0.0f);

	screenVertices[0].TexCoord = glm::vec2(0.0f, 1.0f);
	screenVertices[1].TexCoord = glm::vec2(0.0f, 0.0f);
	screenVertices[2].TexCoord = glm::vec2(1.0f, 0.0f);
	screenVertices[3].TexCoord = glm::vec2(1.0f, 1.0f);

	unsigned int* screenIndices = new unsigned int[6]
		{	2, 1, 0, 3, 2, 0 };

	screenMesh = new Mesh(screenVertices, 4, screenIndices, 6);

	delete[] screenVertices;
	delete[] screenIndices;
}

void ScreenStage::CreatePipeline(const std::string& pixelShaderPath)
{
	CD3DX12_DESCRIPTOR_RANGE1 screenRange[1];
	screenRange[0].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0); // back buffer as Texture

	CD3DX12_ROOT_PARAMETER1 screenRootParameters[1];
	screenRootParameters[0].InitAsDescriptorTable(1, &screenRange[0], D3D12_SHADER_VISIBILITY_PIXEL);

	rootSignature = new DXRootSignature(screenRootParameters, _countof(screenRootParameters),
		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

	DXPipelineDescription description;
	description.VertexPath = "Source/Shaders/NBody/screen.vertex.hlsl";
	description.PixelPath = pixelShaderPath;
	description.RootSignature = rootSignature;

	pipeline = new DXPipeline(description);
}