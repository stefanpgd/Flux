#pragma once

#define WIN32_LEAN_AND_MEAN 
#include <Windows.h>

#include <string>

class Renderer;

class Flux
{
public:
	Flux();

	void Run();

private:
	void RegisterWindowClass();

	void Start();
	void Update();
	void Render();

	static LRESULT CALLBACK WindowsCallback(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

private:
	std::wstring applicationName = L"Flux";
	bool runApplication = true;

	// Systems //
	Renderer* renderer;
};