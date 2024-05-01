#pragma once

#define WIN32_LEAN_AND_MEAN 
#include <Windows.h>

#include <string>
#include <vector>

class Renderer;
class Editor;
class ParticleSimulation;

class Flux
{
public:
	Flux();

	void Run();

private:
	void RegisterWindowClass();

	void Start();
	void Update(float deltaTime);
	void Render();

	void SetParticleSimulation(ParticleSimulation* simulation);

	static LRESULT CALLBACK WindowsCallback(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

private:
	std::wstring applicationName = L"Flux";
	bool runApplication = true;

	// Systems //
	Renderer* renderer;
	Editor* editor;

	std::vector<ParticleSimulation*> simulations;
	ParticleSimulation* activeSimulation;

	friend class Editor;
};