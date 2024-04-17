#include "Flux.h"
#include "Renderer.h"

#define WIN32_LEAN_AND_MEAN 
#include <Windows.h>
#include <cassert>

Flux::Flux()
{
	RegisterWindowClass();

	renderer = new Renderer(applicationName, 1080, 720);
}

void Flux::Run()
{
	MSG msg = {};
	while(runApplication && msg.message != WM_QUIT)
	{
		// Window's Callback //
		if(::PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			::TranslateMessage(&msg);
			::DispatchMessage(&msg);
		}

		// Engine Loop //
		Start();
		Update();
		Render();
	}

	// Destroy systems //
}

void Flux::Start()
{

}

void Flux::Update()
{

}

void Flux::Render()
{

}

void Flux::RegisterWindowClass()
{
	WNDCLASSEXW windowClassDescription = {};
	HINSTANCE hInstance = GetModuleHandle(NULL);

	windowClassDescription.cbSize = sizeof(WNDCLASSEX);
	windowClassDescription.style = CS_HREDRAW | CS_VREDRAW;
	windowClassDescription.lpfnWndProc = &WindowsCallback;
	windowClassDescription.cbClsExtra = 0;
	windowClassDescription.cbWndExtra = 0;
	windowClassDescription.hInstance = hInstance;
	windowClassDescription.hIcon = ::LoadIcon(hInstance, NULL);
	windowClassDescription.hCursor = ::LoadCursor(NULL, IDC_ARROW);
	windowClassDescription.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	windowClassDescription.lpszMenuName = NULL;
	windowClassDescription.lpszClassName = applicationName.c_str();
	windowClassDescription.hIconSm = ::LoadIcon(hInstance, NULL);

	static ATOM atom = ::RegisterClassExW(&windowClassDescription);
	assert(atom > 0);
}

LRESULT Flux::WindowsCallback(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch(message)
	{
	case WM_DESTROY:
		::PostQuitMessage(0);
		break;
	}

	// ImGui Windows callback //
	//ImGui_ImplWin32_WndProcHandler(hwnd, message, wParam, lParam);

	return ::DefWindowProcW(hwnd, message, wParam, lParam);
}