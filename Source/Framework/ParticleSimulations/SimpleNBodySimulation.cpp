#include "Framework/ParticleSimulations/SimpleNBodySimulation.h"
#include "Framework/Input.h"

#include "Graphics/Texture.h"
#include "Graphics/DXAccess.h"

// Render Stages //
#include "Graphics/RenderStages/ClearBufferStage.h"
#include "Graphics/RenderStages/SimpleNBodyComputeStage.h"
#include "Graphics/RenderStages/ScreenStage.h"

#include <imgui.h>

// TODO: Be able to pass different sizes instead of hard-coding it
SimpleNBodySimulation::SimpleNBodySimulation(int particleCount) : ParticleSimulation(particleCount)
{
	// Variables relevant to the render stages & compute pipeline(s) //
	Window* window = DXAccess::GetWindow();

	unsigned int* textureBuffer = new unsigned int[1024 * 1024];
	renderBuffer = new Texture(textureBuffer, 1024, 1024, DXGI_FORMAT_R8G8B8A8_UNORM, sizeof(unsigned int));

	clearBufferStage = new ClearBufferStage(window, renderBuffer);
	nBodyComputeStage = new SimpleNBodyComputeStage(window, renderBuffer, &settings);
	screenStage = new ScreenStage(window, renderBuffer);
}

void SimpleNBodySimulation::Update(float deltaTime)
{
	settings.deltaTime = deltaTime;
	settings.positionX = ImGui::GetIO().MousePos.x;
	settings.positionY = ImGui::GetIO().MousePos.y;

	// TODO: Simulation Speed ( deltaTime multiplier )

	if(Input::GetKeyDown(KeyCode::F))
	{
		settings.G *= -1;
	}

	ImGui::Begin("Simplified N-Body Settings");
	ImGui::DragFloat("G", &settings.G, 0.01f, 0.01f, 10.0f);
	ImGui::DragFloat("Max Velocity", &settings.maxVelocity, 0.1f, 0.01f, 500.0f);
	ImGui::DragFloat("Mouse Mass", &settings.mouseMass, 10.0f);
	ImGui::Separator();
	ImGui::Text("Particle Count: %i", particleCount);
	ImGui::Text("To Flip Gravity - Press F");

	ImGui::End();
}

void SimpleNBodySimulation::Render(ComPtr<ID3D12GraphicsCommandList2> commandList)
{
	// Execute rendering passes //
	clearBufferStage->RecordStage(commandList);
	nBodyComputeStage->RecordStage(commandList);
	screenStage->RecordStage(commandList);
}