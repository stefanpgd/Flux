#include "Framework/Renderer.h"

// DirectX Components //
#include "Graphics/DXAccess.h"
#include "Graphics/DXDevice.h"
#include "Graphics/DXCommands.h"
#include "Graphics/DXUtilities.h"
#include "Graphics/DXDescriptorHeap.h"
#include "Graphics/DXRootSignature.h"
#include "Graphics/DXPipeline.h"
#include "Graphics/DXComputePipeline.h"

// Renderer Components //
#include "Graphics/Window.h"
#include "Graphics/Mesh.h"
#include "Graphics/Texture.h"
#include "Graphics/DepthBuffer.h"
#include "Graphics/DXStructuredBuffer.h"

#include <cassert>
#include <imgui.h>
#include <array>
#include <imgui_impl_win32.h>
#include <imgui_impl_dx12.h>
#include <random>

// TEMP //
Mesh* screenMesh;
DXRootSignature* screenRoot;
DXPipeline* screenPipeline;

Texture* computeTest;
DXRootSignature* computeRoot;
DXComputePipeline* computePipeline;
DXStructuredBuffer* particleBuffer;

struct Particle
{
	float position[2];
};

namespace RendererInternal
{
	Window* window = nullptr;
	DXDevice* device = nullptr;

	DXCommands* directCommands = nullptr;
	DXCommands* copyCommands = nullptr;

	DXDescriptorHeap* CBVHeap = nullptr;
	DXDescriptorHeap* DSVHeap = nullptr;
	DXDescriptorHeap* RTVHeap = nullptr;

	Texture* defaultTexture = nullptr;
}
using namespace RendererInternal;


Renderer::Renderer(const std::wstring& applicationName, unsigned int windowWidth,
	unsigned int windowHeight)
{
	// Initialization for all vital components for rendering //
	SetThreadDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);

	device = new DXDevice();
	CBVHeap = new DXDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, 1000, D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE);
	DSVHeap = new DXDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE_DSV, 10);
	RTVHeap = new DXDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE_RTV, 15);

	directCommands = new DXCommands(D3D12_COMMAND_LIST_TYPE_DIRECT, Window::BackBufferCount);
	copyCommands = new DXCommands(D3D12_COMMAND_LIST_TYPE_DIRECT, 1);

	window = new Window(applicationName, windowWidth, windowHeight);

	InitializeImGui();

	// Placeholder / Temp //
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

	CD3DX12_DESCRIPTOR_RANGE1 screenRange[1];
	screenRange[0].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0); // Render Target as Texture

	CD3DX12_ROOT_PARAMETER1 screenRootParameters[1];
	screenRootParameters[0].InitAsDescriptorTable(1, &screenRange[0], D3D12_SHADER_VISIBILITY_PIXEL);

	screenRoot = new DXRootSignature(screenRootParameters, _countof(screenRootParameters),
		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

	DXPipelineDescription description;
	description.VertexPath = "Source/Shaders/screen.vertex.hlsl";
	description.PixelPath = "Source/Shaders/screen.pixel.hlsl";
	description.RootSignature = screenRoot;

	screenPipeline = new DXPipeline(description);

	int width = 1024;
	int height = 1024;
	unsigned int* textureBuffer = new unsigned int[width * height];
	for(int i = 0; i < width * height; i++)
	{
		textureBuffer[i] = 0;
	}

	computeTest = new Texture(textureBuffer, width, height, DXGI_FORMAT_R8G8B8A8_UNORM, sizeof(unsigned int));

	CD3DX12_DESCRIPTOR_RANGE1 computeRange[1];
	computeRange[0].Init(D3D12_DESCRIPTOR_RANGE_TYPE_UAV, 1, 0, 0, D3D12_DESCRIPTOR_RANGE_FLAG_DATA_VOLATILE);

	CD3DX12_DESCRIPTOR_RANGE1 dataRange[1];
	dataRange[0].Init(D3D12_DESCRIPTOR_RANGE_TYPE_UAV, 1, 1, 0, D3D12_DESCRIPTOR_RANGE_FLAG_DATA_VOLATILE);

	CD3DX12_ROOT_PARAMETER1 computeParameters[2];
	computeParameters[0].InitAsDescriptorTable(1, &computeRange[0]);
	computeParameters[1].InitAsDescriptorTable(1, &dataRange[0]);

	computeRoot = new DXRootSignature(computeParameters, _countof(computeParameters), D3D12_ROOT_SIGNATURE_FLAG_NONE);
	computePipeline = new DXComputePipeline(computeRoot, "Source/Shaders/hello.compute.hlsl");

	srand(time(nullptr));

	Particle* particles = new Particle[1024];
	for(int i = 0; i < 1024; i++)
	{
		particles[i].position[0] = rand() % 1024;
		particles[i].position[1] = rand() % 1024;
	}

	particleBuffer = new DXStructuredBuffer(particles, 1024, sizeof(Particle));
}

void Renderer::Update(float deltaTime)
{
	// TODO: Maybe record render times in here, have a proper MS count etc.
}

void Renderer::Render()
{
	// 0. Grab all relevant objects to perform all stages //
	unsigned int backBufferIndex = window->GetCurrentBackBufferIndex();
	ComPtr<ID3D12GraphicsCommandList2> commandList = directCommands->GetGraphicsCommandList();
	ID3D12DescriptorHeap* heaps[] = { CBVHeap->GetAddress() };

	CD3DX12_CPU_DESCRIPTOR_HANDLE backBufferRTV = window->GetCurrentScreenRTV();
	CD3DX12_CPU_DESCRIPTOR_HANDLE depthView = window->GetDepthDSV();

	// 1. Reset & prepare command allocator //
	directCommands->ResetCommandList(backBufferIndex);

	// 2. Prepare back buffer to be used as Render Target
	TransitionResource(window->GetCurrentScreenBuffer().Get(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);
	BindAndClearRenderTarget(window, &backBufferRTV, &depthView);

	// TEMP //
	commandList->SetDescriptorHeaps(1, heaps);
	commandList->SetComputeRootSignature(computeRoot->GetAddress());
	commandList->SetPipelineState(computePipeline->GetAddress());

	CD3DX12_GPU_DESCRIPTOR_HANDLE uavHandle = computeTest->GetUAV();
	commandList->SetComputeRootDescriptorTable(0, uavHandle);
	commandList->SetComputeRootDescriptorTable(1, particleBuffer->GetUAV());

	commandList->Dispatch(64, 1, 1);
	//TransitionResource(computeTest->GetResource().Get(), D3D12_RESOURCE_STATE_UNORDERED_ACCESS, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);

	// 3. Setup pipeline, and prepare settings for it //
	commandList->SetGraphicsRootSignature(screenRoot->GetAddress());
	commandList->SetPipelineState(screenPipeline->GetAddress());
	commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// 4. Draw the screen quad //
	commandList->IASetVertexBuffers(0, 1, &screenMesh->GetVertexBufferView());
	commandList->IASetIndexBuffer(&screenMesh->GetIndexBufferView());

	commandList->SetGraphicsRootDescriptorTable(0, computeTest->GetSRV());
	commandList->DrawIndexedInstanced(screenMesh->GetIndicesCount(), 1, 0, 0, 0);

	// 5. Transition back the backbuffer to be used as display //
	//TransitionResource(computeTest->GetResource().Get(), D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
	TransitionResource(window->GetCurrentScreenBuffer().Get(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);

	// 6. Execute List, Present and wait for the next frame to be ready //
	directCommands->ExecuteCommandList(backBufferIndex);
	window->Present();
	directCommands->WaitForFenceValue(window->GetCurrentBackBufferIndex());
}

void Renderer::Resize()
{
	directCommands->Flush();
	window->Resize();
}

void Renderer::InitializeImGui()
{
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

	ImGui::StyleColorsDark();
	ImGui_ImplWin32_Init(window->GetHWND());

	const unsigned int cbvIndex = CBVHeap->GetNextAvailableIndex();
	ImGui_ImplDX12_Init(device->GetAddress(), Window::BackBufferCount, DXGI_FORMAT_R8G8B8A8_UNORM,
		CBVHeap->GetAddress(), CBVHeap->GetCPUHandleAt(cbvIndex), CBVHeap->GetGPUHandleAt(cbvIndex));
}

#pragma region DXAccess Implementations
ComPtr<ID3D12Device2> DXAccess::GetDevice()
{
	if(!device)
	{
		assert(false && "DXDevice hasn't been initialized yet, call will return nullptr");
	}

	return device->Get();
}

DXCommands* DXAccess::GetCommands(D3D12_COMMAND_LIST_TYPE type)
{
	if(!directCommands || !copyCommands)
	{
		assert(false && "Commands haven't been initialized yet, call will return nullptr");
	}

	switch(type)
	{
	case D3D12_COMMAND_LIST_TYPE_COPY:
		return copyCommands;
		break;

	case D3D12_COMMAND_LIST_TYPE_DIRECT:
		return directCommands;
		break;
	}

	assert(false && "This command type hasn't been added yet!");
	return nullptr;
}

unsigned int DXAccess::GetCurrentBackBufferIndex()
{
	if(!window)
	{
		assert(false && "Window hasn't been initialized yet, can't retrieve back buffer index");
	}

	return window->GetCurrentBackBufferIndex();
}

Texture* DXAccess::GetDefaultTexture()
{
	// Incase an texture isn't present, the 'default' texture gets loaded in
	// Similar to Valve's ERROR 3D model
	return defaultTexture;
}

DXDescriptorHeap* DXAccess::GetDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE type)
{
	switch(type)
	{
	case D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV:
		return CBVHeap;
		break;

	case D3D12_DESCRIPTOR_HEAP_TYPE_DSV:
		return DSVHeap;
		break;

	case D3D12_DESCRIPTOR_HEAP_TYPE_RTV:
		return RTVHeap;
		break;
	}

	// Invalid type passed
	assert(false && "Descriptor type passed isn't a valid or created type!");
	return nullptr;
}

Window* DXAccess::GetWindow()
{
	return window;
}
#pragma endregion