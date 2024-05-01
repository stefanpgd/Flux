#include "Framework/ParticleSimulations/PhysarumSimulation.h"

#include "Graphics/Texture.h"
#include "Graphics/DXAccess.h"

#include "Graphics/RenderStages/PhysarumDiffuseStage.h"
#include "Graphics/RenderStages/PhysarumAgentStage.h"
#include "Graphics/RenderStages/PresentScreenStage.h"

#include <imgui.h>

PhysarumSimulation::PhysarumSimulation(unsigned int particleCount) : ParticleSimulation(particleCount)
{
	simulationName = "Physarum Polycephalum";
	settings.particleCount = particleCount;

	unsigned int* textureBuffer = new unsigned int[1024 * 1024];
	for(int i = 0; i < 1024 * 1024; i++)
	{
		textureBuffer[i] = 0;
	}

	renderBuffer = new Texture(textureBuffer, 1024, 1024, DXGI_FORMAT_R8G8B8A8_UNORM, sizeof(unsigned int));
	
	agentStage = new PhysarumAgentStage(renderBuffer, &settings);
	diffuseStage = new PhysarumDiffuseStage(renderBuffer, &settings);
	screenStage = new PresentScreenStage("Source/Shaders/Physarum/screen.pixel.hlsl", renderBuffer);
}

void PhysarumSimulation::Update(float deltaTime)
{
	settings.deltaTime = deltaTime;

	ImGui::Begin("Physarum Simulation Settings");
	
	ImGui::Text("Settings related to the agent Movement Speed");
	ImGui::DragFloat("Agent Speed", &settings.agentSpeed, 0.5f, 0.0f, 250.0f);
	ImGui::DragFloat("Agent Turn Speed", &settings.agentTurnSpeed, 0.2f, 0.0f, 150.0f);
	ImGui::Separator();

	ImGui::Text("Settings related to the Trail 'Sensing'");
	ImGui::DragFloat("Sensor Angle", &settings.agentSensorAngle, 0.01f, 0.01f, 3.14159265);
	ImGui::DragFloat("Sensor Distance", &settings.agentSensorDistance, 0.01f, 0.01f, 50.0f);
	ImGui::SliderInt("Sensor Size", &settings.agentSensorSize, 1, 3);
	ImGui::Separator();

	ImGui::Text("Settings related to the Trail");
	ImGui::DragFloat("Trail Dissolve Speed", &settings.trailDissolveSpeed, 0.01f, 0.33f, 10.0f);
	ImGui::DragFloat("Trail Diffuse Speed", &settings.trailDiffuseSpeed, 0.05f, 0.33f, 50.0f);

	ImGui::Separator();
	ImGui::Text("Particle Count: %i", settings.particleCount);

	ImGui::Text("Adjusting the 'Sensor Distance' will result\ninto large or small cell-like structures.", settings.particleCount);


	ImGui::End();
}

void PhysarumSimulation::Render(ComPtr<ID3D12GraphicsCommandList2> commandList)
{
	agentStage->RecordStage(commandList);
	diffuseStage->RecordStage(commandList);
	screenStage->RecordStage(commandList);
}