#pragma once

#include <vector>
#include <array>

class Flux;
class ParticleSimulation;

class Editor
{
public:
	Editor(Flux* application, const std::vector<ParticleSimulation*>& simulations);

	void Update(float deltaTime);

private:
	void ImGuiStyleSettings();

private:
	float deltaTime;
	std::array<int, 60> averageFPS;
	unsigned int frameCount = 0;

	struct ImFont* baseFont;
	struct ImFont* boldFont;

	Flux* application;
	const std::vector<ParticleSimulation*>& simulations;
};