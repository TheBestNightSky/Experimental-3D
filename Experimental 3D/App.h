#pragma once
#include "Window.h"
#include "SkyenetTimer.h"
#include "Drawable.h"
#include "ImguiManager.h"
#include "Camera.h"
#include "PointLight.h"
#include <set>

class App {
public:
	App();
	// master frame / message loop
	int Start();

private:
	void DoFrame(float dt);

private:
	ImguiManager imgui;
	Window wnd;
	SkyenetTimer timer;
	SkyenetTimer ftimer;
	SkyenetTimer ptimer;
	//std::unique_ptr<class Arrow> arrow;
	std::vector<std::unique_ptr<class Drawable>> drawables;
	std::vector<class Box*> boxes;
	float speed_factor = 1.0f;
	Camera cam;
	PointLight light;
	static constexpr size_t nDrawables = 200;
	std::optional<int> comboBoxIndex;
	std::set<int> boxControlIds;
};