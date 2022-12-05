#pragma once
#include "Window.h"
#include "SkyenetTimer.h"
#include "Drawable.h"
#include "ImguiManager.h"
#include "Camera.h"
#include "PointLight.h"
#include "Mesh.h"
#include <set>

class App {
public:
	App();
	// master frame / message loop
	int Start();

private:
	void DoFrame(float dt);
	void ShowModelWindow();

private:
	ImguiManager imgui;
	Window wnd;
	SkyenetTimer timer;
	SkyenetTimer ftimer;
	SkyenetTimer ptimer;
	float speed_factor = 1.0f;
	Camera cam;
	PointLight light;
	Model nano{ wnd.Gfx(), "Models\\nanosuit.obj" };
	struct
	{
		float roll = 0.0f;
		float pitch = 0.0f;
		float yaw = 0.0f;
		float x = 0.0f;
		float y = 0.0f;
		float z = 0.0f;
	} pos;
};