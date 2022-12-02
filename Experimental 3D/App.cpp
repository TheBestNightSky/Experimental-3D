#include "App.h"
//#include "Arrow.h"
#include "Box.h"
#include "Cylinder.h"
#include "Pyramid.h"
#include "SkinnedBox.h"
#include <memory>
#include <algorithm>
#include "SkyeMath.h"
#include <sstream>
#include "SkyeFun.h"
#include <math.h>
#include "WndInfo.h"
#include "Surface.h"
#include "GDIPlusManager.h"
#include "imgui/imgui.h"

GDIPlusManager gdipm;

App::App() : wnd(1280, 720, L"Skynet R00L5-U"), light(wnd.Gfx()) {

	class Factory {
	public:
		Factory(Graphics& gfx)
			:
			gfx(gfx)
		{}
		std::unique_ptr<Drawable> operator()()
		{
			const DirectX::XMFLOAT3 mat = { cdist(rng), cdist(rng), cdist(rng) };
			switch (sdist(rng))
			{
			case 0:
				return std::make_unique<Box>(
					gfx, rng, adist, ddist,
					odist, rdist, bdist, mat
					);
			case 1:
				return std::make_unique<Cylinder>(
					gfx, rng, adist, ddist, odist,
					rdist, bdist, tdist
					);
			case 2:
				return std::make_unique<Pyramid>(
					gfx, rng, adist, ddist, odist,
					rdist, tdist
					);
			case 3:
				return std::make_unique<SkinnedBox>(
					gfx, rng, adist, ddist,
					odist, rdist
					);
			default:
				assert(false && "impossible drawable option in factory");
				return {};
			}
		}
	private:
		Graphics& gfx;
		std::mt19937 rng{ std::random_device{}() };
		std::uniform_int_distribution<int> sdist{ 0,3 };
		std::uniform_real_distribution<float> adist{ 0.0f,PI * 2.0f };
		std::uniform_real_distribution<float> ddist{ 0.0f,PI * 0.5f };
		std::uniform_real_distribution<float> odist{ 0.0f,PI * 0.08f };
		std::uniform_real_distribution<float> rdist{ 6.0f,20.0f };
		std::uniform_real_distribution<float> bdist{ 0.4f,3.0f };
		std::uniform_real_distribution<float> cdist{ 0.0f, 1.0f };
		std::uniform_int_distribution<int> tdist{ 3,30 };
	};

	Factory f(wnd.Gfx());
	drawables.reserve(nDrawables);
	std::generate_n(std::back_inserter(drawables), nDrawables, f);

	//init box pointers for editing instance parameters
	for (auto& pd : drawables) {

		if (auto pb = dynamic_cast<Box*>(pd.get())) {

			boxes.push_back(pb);
		}
	}

	//arrow = std::make_unique<Arrow>(wnd.Gfx(), 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f);
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
			
			data.ms++;			
		}
		else if (e.GetType() == Mouse::Event::Type::WheelDown) {
			
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

	for (auto& d : drawables)
	{
		d->Update(wnd.kbd.KeyIsPressed(VK_SPACE) ? 0.0f : pt);
		d->Draw(wnd.Gfx());
	}
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
	

	// imgui window to open box windows
	if (ImGui::Begin("Boxes"))
	{
		using namespace std::string_literals;
		const auto preview = comboBoxIndex.has_value() ? std::to_string(comboBoxIndex.value()) : "Choose a box..."s;
		if (ImGui::BeginCombo("Box Number", preview.c_str()))
		{
			for (int i = 0; i < boxes.size(); i++)
			{
				const bool selected = *comboBoxIndex == i;
				if (ImGui::Selectable(std::to_string(i).c_str(), selected))
				{
					comboBoxIndex = i;
				}
				if (selected)
				{
					ImGui::SetItemDefaultFocus();
				}
			}
			ImGui::EndCombo();
		}
		if (ImGui::Button("Spawn Control Window") && comboBoxIndex)
		{
			boxControlIds.insert(*comboBoxIndex);
			comboBoxIndex.reset();
		}
	}
	ImGui::End();
	// imgui box attribute control windows
	for (auto id : boxControlIds)
	{
		boxes[id]->SpawnControlWindow(id, wnd.Gfx());
	}


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