#pragma once

class Editor
{
public:
	Editor();

	void Update(float deltaTime);

private:
	void ImGuiStyleSettings();

private:
	float deltaTime;

	struct ImFont* baseFont;
	struct ImFont* boldFont;
};