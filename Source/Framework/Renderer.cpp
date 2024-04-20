#include "Framework/Renderer.h"

// DirectX Components //
#include "Graphics/DXAccess.h"
#include "Graphics/DXDevice.h"
#include "Graphics/DXCommands.h"
#include "Graphics/DXUtilities.h"

// Renderer Components //
#include "Graphics/Window.h"
#include "Graphics/Texture.h"

// Render Stages //
#include "Graphics/RenderStages/ClearBufferStage.h"
#include "Graphics/RenderStages/ParticleComputeStage.h"
#include "Graphics/RenderStages/ScreenStage.h"

#include <cassert>
#include <imgui.h>
#include <imgui_impl_win32.h>
#include <imgui_impl_dx12.h>

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

	// Variables relevant to the render stages & compute pipeline(s) //
	unsigned int* textureBuffer = new unsigned int[1024 * 1024];
	computeBackBuffer = new Texture(textureBuffer, 1024, 1024, DXGI_FORMAT_R8G8B8A8_UNORM, sizeof(unsigned int));

	clearBufferStage = new ClearBufferStage(window, computeBackBuffer);
	particleComputeStage = new ParticleComputeStage(window, computeBackBuffer, 1024);
	screenStage = new ScreenStage(window, computeBackBuffer);
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

	// 1. Reset & prepare command allocator //
	directCommands->ResetCommandList(backBufferIndex);

	// 2. Bind heap(s) that will be required for the pipelines //
	commandList->SetDescriptorHeaps(1, heaps);

	// 3. Execute rendering passes //
	clearBufferStage->RecordStage(commandList);
	particleComputeStage->RecordStage(commandList);
	screenStage->RecordStage(commandList);

	// 4. Execute List, Present and wait for the next frame to be ready //
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