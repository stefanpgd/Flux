#include "Framework/ParticleSimulations/LifeSimulation.h"
#include "Framework/ParticleSimulation.h"

#include "Graphics/Texture.h"
#include "Graphics/DXAccess.h"

#include "Graphics/RenderStages/ClearScreenStage.h"
#include "Graphics/RenderStages/LifeParticleStage.h"
#include "Graphics/RenderStages/ScreenStage.h"

#include <imgui.h>

LifeSimulation::LifeSimulation(unsigned int particleCount)
	: ParticleSimulation(particleCount)
{
	simulationName = "Particle Life";
	settings.particleCount = particleCount;

	unsigned int* textureBuffer = new unsigned int[1024 * 1024];
	for(int i = 0; i < 1024 * 1024; i++)
	{
		textureBuffer[i] = 0;
	}

	renderBuffer = new Texture(textureBuffer, 1024, 1024, DXGI_FORMAT_R8G8B8A8_UNORM, sizeof(unsigned int));

	clearScreenStage = new ClearScreenStage(renderBuffer);
	screenStage = new ScreenStage("Source/Shaders/Life/screen.pixel.hlsl", renderBuffer);
	particleStage = new LifeParticleStage(renderBuffer, &settings);

	settings.cellColors[0] = glm::vec4(1.0f, 0.2f, 0.2f, 1.0f);
	settings.cellColors[1] = glm::vec4(0.2f, 1.0f, 0.2f, 1.0f);
	settings.cellColors[2] = glm::vec4(0.2f, 0.2f, 1.0f, 1.0f);
}

void LifeSimulation::Update(float deltaTime)
{
	settings.deltaTime = deltaTime;

	ImGui::Begin("Particle Life Simulation Settings");
	ImGui::DragFloat("Particle Scan Distance", &settings.maxDistance, 0.01f, 0.0f, 50.0f);
	ImGui::DragFloat("Particle Friction", &settings.friction, 0.005f, 0.0f, 10.0f);

	ImGui::ColorEdit3("Cell Color - 1", &settings.cellColors[0].x);
	ImGui::ColorEdit3("Cell Color - 2", &settings.cellColors[1].x);
	ImGui::ColorEdit3("Cell Color - 3", &settings.cellColors[2].x);

	ImGui::SeparatorText("Cell Attractions");

	for(int x = 0; x < 4; x++)
	{
		ImGui::PushID(x);

		if(x > 0)
		{
			ImGui::SameLine();

			ImVec4 gridColor = ImVec4(settings.cellColors[x - 1].x, settings.cellColors[x - 1].y, settings.cellColors[x - 1].z, 1.0f);
			ImGui::PushStyleColor(ImGuiCol_Header, gridColor);
			ImGui::Selectable("###", true, ImGuiSelectableFlags_Disabled, ImVec2(50, 50));
			ImGui::PopStyleColor();
		}
		else
		{
			ImGui::Selectable("###", true, ImGuiSelectableFlags_Disabled, ImVec2(50, 50));
		}

		ImGui::PopID();
	}

	for(int y = 0; y < 3; y++)
	{
		for(int x = 0; x < 4; x++)
		{
			ImGui::PushID(y * 4 + x);

			if(x == 0)
			{
				ImVec4 gridColor = ImVec4(settings.cellColors[y].x, settings.cellColors[y].y, settings.cellColors[y].z, 1.0f);

				ImGui::PushStyleColor(ImGuiCol_Header, gridColor);
				ImGui::Selectable("###", true, ImGuiSelectableFlags_Disabled, ImVec2(50, 50));
				ImGui::PopStyleColor();
			}
			else
			{
				float* value = &settings.attractionMatrix[y][x - 1];

				ImGui::SameLine();

				glm::vec3 on = glm::vec3(0.2f, 1.0f, 0.2);
				glm::vec3 off = glm::vec3(1.0f, 0.2f, 0.2f);

				float a = (*value * 0.5) + 0.5;
				glm::vec3 color = off * (1.0f - a) + on * a;

				ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(color.x, color.y, color.z, 0.5f));
				ImGui::VSliderFloat("", ImVec2(51, 51), value, -1.0f, 1.0f);
				ImGui::PopStyleColor();
			}

			ImGui::PopID();
		}
	}


	ImGui::End();
}

void LifeSimulation::Render(ComPtr<ID3D12GraphicsCommandList2> commandList)
{
	clearScreenStage->RecordStage(commandList);
	particleStage->RecordStage(commandList);
	screenStage->RecordStage(commandList);
}