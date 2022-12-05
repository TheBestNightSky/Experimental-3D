#include "App.h"
#include "AssTest.h"
#include <memory>
#include <algorithm>
#include "SkyeMath.h"
#include <sstream>
#include <math.h>
#include "WndInfo.h"
#include "Surface.h"
#include "GDIPlusManager.h"
#include "imgui/imgui.h"

GDIPlusManager gdipm;

App::App() : wnd(1280, 720, L"Skynet R00L5-U"), light(wnd.Gfx()) {

	WndInfo winfo = wnd.GetWndInfo();
	wnd.Gfx().SetProjection(DirectX::XMMatrixPerspectiveLH(1.0f, (float)winfo.CRegion.height / (float)winfo.CRegion.width, 0.5f, 100.0f));
}

int App::Start() {
	while (true) {
		if (const auto ecode = Window::ProcessMessages()) {
			//if optional returned has value it means we are quiting so break out of loop and return the exit code 
			return *ecode;
		}

		DoFrame(ftimer.Check());
	}
}

struct Data {
	bool vsync = true;
	int mx = 0;
	int my = 0;
	int ms = 0;
	float t = 0;
	unsigned char lk = '_';
};

Data data;
void App::DoFrame(float dt) {
	std::ostringstream oss;
	const auto pt = ptimer.Mark() * speed_factor;

	////calculate BG color
	const float c = 0.0f;//sin(timer.Check()) / 2.0f + 0.5f;
	const float d = 0.0f;//sin(timer.Check() + 1.0f) / 2.0f + 0.5f;
	const float f = 0.01f;//sin(timer.Check() + 2.0f) / 2.0f + 0.5f;

	wnd.Gfx().BeginFrame(c, d, f);
	wnd.Gfx().SetCamera(cam.GetMatrix());
	light.Bind(wnd.Gfx(), cam.GetMatrix());

	while (!wnd.mouse.IsEmpty()) {
		const auto e = *wnd.mouse.Read();

		if (e.GetType() == Mouse::Event::Type::Move) {
			
			data.mx = e.GetPosX();
			data.my = e.GetPosY();
		}
		else if (e.GetType() == Mouse::Event::Type::WheelUp) {
			cam.ModR(-1);
			data.ms++;			
		}
		else if (e.GetType() == Mouse::Event::Type::WheelDown) {
			cam.ModR(1);
			data.ms--;
		}
	}

	while (!wnd.kbd.KeyIsEmpty()) {
		const auto e = wnd.kbd.ReadKey();

		if (e.IsPress()) {
			data.lk = e.GetCode();
			if (data.lk == 'V') {
				data.vsync = !data.vsync;
			}
		}
	}

	data.t = timer.Check();

	const auto transform = DirectX::XMMatrixRotationRollPitchYaw(pos.pitch, pos.yaw, pos.roll) *
		DirectX::XMMatrixTranslation(pos.x, pos.y, pos.z);
	nano.Draw(wnd.Gfx(), transform);
	light.Draw(wnd.Gfx());


	if (ImGui::Begin("Simulation Speed"))
	{
		ImGui::SliderFloat("Speed Factor", &speed_factor, 0.0f, 6.0f, "%.4f");
		ImGui::Checkbox(" - Vsync Enabled", &data.vsync);
		ImGui::Text("%.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
		ImGui::Text("Mouse Pos: (%i , %i)", data.mx, data.my);
		ImGui::Text("Mouse Wheel Delta: (%i)", data.ms);
		ImGui::Text("Last Key Press: (%c)", data.lk);
		ImGui::Text("Time Elapsed: (%.1f)", data.t);
		ImGui::Text("Status: %s", wnd.kbd.KeyIsPressed(VK_SPACE) ? "PAUSED" : "RUNNING (hold spacebar to pause)");
	}
	ImGui::End();
	
	cam.SpawnControlWindow();
	light.SpawnControlWindow();
	ShowModelWindow();

	//present
	switch (data.vsync) {
	case true:
		wnd.Gfx().PushFrame(); //flips the back buffer to the front where it will be drawn on screen (With VSync)
		break;
	case false:
		wnd.Gfx().PushFrame_NO_VSYNC(); //flips the back buffer to the front where it will be drawn on screen (No VSync)
		break;
	};
}

void App::ShowModelWindow() {

	if (ImGui::Begin("Model"))
	{
		using namespace std::string_literals;

		ImGui::Text("Orientation");
		ImGui::SliderAngle("Roll", &pos.roll, -180.0f, 180.0f);
		ImGui::SliderAngle("Pitch", &pos.pitch, -180.0f, 180.0f);
		ImGui::SliderAngle("Yaw", &pos.yaw, -180.0f, 180.0f);

		ImGui::Text("Position");
		ImGui::SliderFloat("X", &pos.x, -20.0f, 20.0f);
		ImGui::SliderFloat("Y", &pos.y, -20.0f, 20.0f);
		ImGui::SliderFloat("Z", &pos.z, -20.0f, 20.0f);
	}
	ImGui::End();
}