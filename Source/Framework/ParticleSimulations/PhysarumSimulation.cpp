#include "Framework/ParticleSimulations/PhysarumSimulation.h"

#include "Graphics/Texture.h"
#include "Graphics/DXAccess.h"

#include "Graphics/RenderStages/PhysarumDiffuseStage.h"
#include "Graphics/RenderStages/PhysarumComputeStage.h"
#include "Graphics/RenderStages/PhysarumScreenStage.h"

#include <imgui.h>

PhysarumSimulation::PhysarumSimulation(unsigned int particleCount) : ParticleSimulation(particleCount)
{
	settings.particleCount = particleCount;

	unsigned int* textureBuffer = new unsigned int[1024 * 1024];
	for(int i = 0; i < 1024 * 1024; i++)
	{
		textureBuffer[i] = 0;
	}

	renderBuffer = new Texture(textureBuffer, 1024, 1024, DXGI_FORMAT_R8G8B8A8_UNORM, sizeof(unsigned int));
	
	// TODO: Since every render stage needs window anyway, moving it out of the parameters
	// into the base class consutrctor is probably the way to go
	Window* window = DXAccess::GetWindow();

	agentStage = new PhysarumComputeStage(window, renderBuffer, &settings);
	diffuseStage = new PhysarumDiffuseStage(window, renderBuffer, &settings);
	screenStage = new PhysarumScreenStage(window, renderBuffer);
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

	ImGui::ColorPicker3("Trail Color", &settings.agentColor[0]);
	ImGui::DragFloat("Trail Strength Per Frame", &settings.agentStrengthPerFrame, 0.001f, 0.0f, 1.0f);

	ImGui::Separator();
	ImGui::Text("Particle Count: %i", settings.particleCount);

	ImGui::End();
}

void PhysarumSimulation::Render(ComPtr<ID3D12GraphicsCommandList2> commandList)
{
	agentStage->RecordStage(commandList);
	diffuseStage->RecordStage(commandList);
	screenStage->RecordStage(commandList);
}