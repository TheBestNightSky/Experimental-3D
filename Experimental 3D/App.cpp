#include "App.h"
#include <memory>
#include <algorithm>
#include "SkyeMath.h"
#include <sstream>
#include <math.h>
#include "WndInfo.h"
#include "Surface.h"
#include "imgui/imgui.h"
#include "VertexBuffer.h"
#include <dxtex/DirectXTex.h>
#include "Testing.h"
#include "PerfLog.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "Mesh.h"
#include "DynamicConstant.h"
#include "ModelProbe.h"
#include "Node.h"
#include "SkyeXM.h"

App::App()
	:
	wnd(1920, 1000, L"Skynet R00L5-U"),
	light1(wnd.Gfx()),
	light2(wnd.Gfx()),
	light3(wnd.Gfx()),
	light4(wnd.Gfx()),
	light5(wnd.Gfx()),
	light6(wnd.Gfx()),
	light7(wnd.Gfx()),
	light8(wnd.Gfx())
	{
	//TestDynamicConstant();
	//TestDynamicMeshLoading();

	/*wall.SetRootTransform(DirectX::XMMatrixTranslation(-12.0f, 0.0f, 0.0f));
	tp.SetPos({ 12.0f,0.0f,-1.0f });
	gobber.SetRootTransform(DirectX::XMMatrixTranslation(0.0f, 0.0f, -4.0f));
	nano.SetRootTransform(DirectX::XMMatrixTranslation(0.0f, -7.0f, 6.0f));
	tp.SetRotation(0.0f, 0.0f, 90.0f);*/

	//bluePlane.SetPos(cam.GetPos());
	//redPlane.SetPos(cam.GetPos());

	light1.SetPos({-31.0f, 7.5f, 11.0f});
	light2.SetPos({ 24.4f, 7.5f, 11.0f });
	light3.SetPos({ -31.0f, 7.5f, -7.2f });
	light4.SetPos({ 24.4f, 7.5f, -7.2f });

	//vase lights
	light5.SetPos({56.0f, 8.0f, 22.5f});
	light6.SetPos({56.0f, 8.0f, -20.1f});
	light7.SetPos({-60.0f, 8.0f, -20.1f});
	light8.SetPos({-60.0f, 8.0f, 22.5f});

	light5.SetColor({1.0f, 0.69f, 0.41f});
	light6.SetColor({ 1.0f, 0.69f, 0.41f });
	light7.SetColor({ 1.0f, 0.69f, 0.41f });
	light8.SetColor({ 1.0f, 0.69f, 0.41f });

	light5.SetIntensity(2.0f);
	light6.SetIntensity(2.0f);
	light7.SetIntensity(2.0f);
	light8.SetIntensity(2.0f);
	//cube.SetPos({ 4.0f, 0.0f, 0.0f });
	//cube.SetPos({ 0.0f, 4.0f, 0.0f });

	WndInfo winfo = wnd.GetWndInfo();
	wnd.Gfx().SetProjection(DirectX::XMMatrixPerspectiveLH(1.0f, (float)winfo.CRegion.height / (float) winfo.CRegion.width, 0.5f,600.0f));
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
	//
	light1.Bind(wnd.Gfx(), cam.GetMatrix());

	while (!wnd.mouse.IsEmpty()) {
		const auto e = *wnd.mouse.Read();

		if (e.GetType() == Mouse::Event::Type::Move) {
			
			data.mx = e.GetPosX();
			data.my = e.GetPosY();
		}
	}

	while (!wnd.kbd.KeyIsEmpty()) {
		const auto e = wnd.kbd.ReadKey();

		if (e.IsPress()) {
			data.lk = e.GetCode();
			if (data.lk == 'V') {
				data.vsync = !data.vsync;
			}
			if (data.lk == VK_TAB) {
				switch (wnd.CursorEnabled()) {
				case true:
					wnd.DisableCursor();
					wnd.mouse.EnableRaw(true);
					break;
				case false:
					wnd.EnableCursor();
					wnd.mouse.EnableRaw(false);
					break;
				}
			}
		}
	}

	if (!wnd.CursorEnabled())
	{
		if (wnd.kbd.KeyIsPressed('W'))
		{
			cam.Translate({ 0.0f,0.0f,pt });
		}
		if (wnd.kbd.KeyIsPressed('A'))
		{
			cam.Translate({ -pt,0.0f,0.0f });
		}
		if (wnd.kbd.KeyIsPressed('S'))
		{
			cam.Translate({ 0.0f,0.0f,-pt });
		}
		if (wnd.kbd.KeyIsPressed('D'))
		{
			cam.Translate({ pt,0.0f,0.0f });
		}
		if (wnd.kbd.KeyIsPressed('R'))
		{
			cam.Translate({ 0.0f,pt,0.0f });
		}
		if (wnd.kbd.KeyIsPressed('F'))
		{
			cam.Translate({ 0.0f,-pt,0.0f });
		}
		if (wnd.kbd.KeyIsPressed(VK_SHIFT))
		{
			cam.SetSpeed(36.0f);
		}
		else {
			cam.SetSpeed(12.0f);
		}
	}

	while (const auto delta = wnd.mouse.ReadRawDelta())
	{
		if (!wnd.CursorEnabled())
		{
			cam.Rotate((float)delta->x, (float)delta->y);
		}
	}

	data.t = timer.Check();
	//wall.Draw(wnd.Gfx());
	//tp.Draw(wnd.Gfx());
	//nano.Draw(wnd.Gfx());
	//gobber.Submit(fc);
	//cube.Submit(fc);

	sponza.Submit(fc);
	
	light1.Submit(fc);
	light2.Submit(fc);
	light3.Submit(fc);
	light4.Submit(fc);
	light5.Submit(fc);
	light6.Submit(fc);
	light7.Submit(fc);
	light8.Submit(fc);

	//cube2.Submit(fc);

	fc.Execute(wnd.Gfx());

	//bluePlane.Draw(wnd.Gfx());
	//redPlane.Draw(wnd.Gfx());

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

	// Mesh techniques window
	class TP : public TechniqueProbe
	{
	public:
		void OnSetTechnique() override
		{
			using namespace std::string_literals;
			ImGui::TextColored({ 0.4f,1.0f,0.6f,1.0f }, pTech->GetName().c_str());
			bool active = pTech->IsActive();
			ImGui::Checkbox(("Tech Active##"s + std::to_string(techIdx)).c_str(), &active);
			pTech->SetActiveState(active);
		}
		bool OnVisitBuffer(Dcb::Buffer& buf) override
		{
			namespace dx = DirectX;
			float dirty = false;
			const auto dcheck = [&dirty](bool changed) {dirty = dirty || changed; };
			auto tag = [tagScratch = std::string{}, tagString = "##" + std::to_string(bufIdx)]
			(const char* label) mutable
			{
				tagScratch = label + tagString;
				return tagScratch.c_str();
			};

			if (auto v = buf["scale"]; v.Exists())
			{
				dcheck(ImGui::SliderFloat(tag("Scale"), &v, 1.0f, 2.0f, "%.3f"));
			}
			if (auto v = buf["offset"]; v.Exists())
			{
				dcheck(ImGui::SliderFloat(tag("offset"), &v, 0.0f, 1.0f, "%.3f"));
			}
			if (auto v = buf["materialColor"]; v.Exists())
			{
				dcheck(ImGui::ColorPicker3(tag("Color"), reinterpret_cast<float*>(&static_cast<dx::XMFLOAT3&>(v))));
			}
			if (auto v = buf["specularColor"]; v.Exists())
			{
				dcheck(ImGui::ColorPicker3(tag("Spec. Color"), reinterpret_cast<float*>(&static_cast<dx::XMFLOAT3&>(v))));
			}
			if (auto v = buf["specularGloss"]; v.Exists())
			{
				dcheck(ImGui::SliderFloat(tag("Glossiness"), &v, 1.0f, 100.0f, "%.1f"));
			}
			if (auto v = buf["specularWeight"]; v.Exists())
			{
				dcheck(ImGui::SliderFloat(tag("Spec. Weight"), &v, 0.0f, 2.0f));
			}
			if (auto v = buf["useSpecularMap"]; v.Exists())
			{
				dcheck(ImGui::Checkbox(tag("Spec. Map Enable"), &v));
			}
			if (auto v = buf["useNormalMap"]; v.Exists())
			{
				dcheck(ImGui::Checkbox(tag("Normal Map Enable"), &v));
			}
			if (auto v = buf["normalMapWeight"]; v.Exists())
			{
				dcheck(ImGui::SliderFloat(tag("Normal Map Weight"), &v, 0.0f, 2.0f));
			}
			return dirty;
		}
	};

	class MP : ModelProbe
	{
	public:
		void SpawnWindow(Model& model)
		{
			ImGui::Begin("Model");
			ImGui::Columns(2, nullptr, true);
			model.Accept(*this);

			ImGui::NextColumn();
			if (pSelectedNode != nullptr)
			{
				bool dirty = false;
				const auto dcheck = [&dirty](bool changed) {dirty = dirty || changed; };
				auto& tf = ResolveTransform();
				ImGui::TextColored({ 0.4f,1.0f,0.6f,1.0f }, "Translation");
				dcheck(ImGui::SliderFloat("X", &tf.x, -60.f, 60.f));
				dcheck(ImGui::SliderFloat("Y", &tf.y, -60.f, 60.f));
				dcheck(ImGui::SliderFloat("Z", &tf.z, -60.f, 60.f));
				ImGui::TextColored({ 0.4f,1.0f,0.6f,1.0f }, "Orientation");
				dcheck(ImGui::SliderAngle("X-rotation", &tf.xRot, -180.0f, 180.0f));
				dcheck(ImGui::SliderAngle("Y-rotation", &tf.yRot, -180.0f, 180.0f));
				dcheck(ImGui::SliderAngle("Z-rotation", &tf.zRot, -180.0f, 180.0f));
				if (dirty)
				{
					pSelectedNode->SetAppliedTransform(
						DirectX::XMMatrixRotationX(tf.xRot) *
						DirectX::XMMatrixRotationY(tf.yRot) *
						DirectX::XMMatrixRotationZ(tf.zRot) *
						DirectX::XMMatrixTranslation(tf.x, tf.y, tf.z)
					);
				}
			}
			if (pSelectedNode != nullptr)
			{
				TP probe;
				pSelectedNode->Accept(probe);
			}
			ImGui::End();
		}
	protected:
		bool PushNode(Node& node) override
		{
			// if there is no selected node, set selectedId to an impossible value
			const int selectedId = (pSelectedNode == nullptr) ? -1 : pSelectedNode->GetId();
			// build up flags for current node
			const auto node_flags = ImGuiTreeNodeFlags_OpenOnArrow
				| ((node.GetId() == selectedId) ? ImGuiTreeNodeFlags_Selected : 0)
				| (node.HasChildren() ? 0 : ImGuiTreeNodeFlags_Leaf);
			// render this node
			const auto expanded = ImGui::TreeNodeEx(
				(void*)(intptr_t)node.GetId(),
				node_flags, node.GetName().c_str()
			);
			// processing for selecting node
			if (ImGui::IsItemClicked())
			{
				// used to change the highlighted node on selection change
				struct Probe : public TechniqueProbe
				{
					virtual void OnSetTechnique()
					{
						if (pTech->GetName() == "Outline")
						{
							pTech->SetActiveState(highlighted);
						}
					}
					bool highlighted = false;
				} probe;

				// remove highlight on prev-selected node
				if (pSelectedNode != nullptr)
				{
					pSelectedNode->Accept(probe);
				}
				// add highlight to newly-selected node
				probe.highlighted = true;
				node.Accept(probe);

				pSelectedNode = &node;
			}
			// signal if children should also be recursed
			return expanded;
		}
		void PopNode(Node& node) override
		{
			ImGui::TreePop();
		}
	private:
		Node* pSelectedNode = nullptr;
		struct TransformParameters
		{
			float xRot = 0.0f;
			float yRot = 0.0f;
			float zRot = 0.0f;
			float x = 0.0f;
			float y = 0.0f;
			float z = 0.0f;
		};
		std::unordered_map<int, TransformParameters> transformParams;
	private:
		TransformParameters& ResolveTransform() noexcept
		{
			const auto id = pSelectedNode->GetId();
			auto i = transformParams.find(id);
			if (i == transformParams.end())
			{
				return LoadTransform(id);
			}
			return i->second;
		}
		TransformParameters& LoadTransform(int id) noexcept
		{
			const auto& applied = pSelectedNode->GetAppliedTransform();
			const auto angles = ExtractEulerAngles(applied);
			const auto translation = ExtractTranslation(applied);
			TransformParameters tp;
			tp.zRot = angles.z;
			tp.xRot = angles.x;
			tp.yRot = angles.y;
			tp.x = translation.x;
			tp.y = translation.y;
			tp.z = translation.z;
			return transformParams.insert({ id,{ tp } }).first->second;
		}
	};
	static MP modelProbe;

	//imgui windows
	modelProbe.SpawnWindow(sponza);
	//cam.SpawnControlWindow();
	//light1.SpawnControlWindow();
	//cube.SpawnControlWindow(wnd.Gfx(), "Cube 1");
	//cube2.SpawnControlWindow(wnd.Gfx(), "Cube 2");
	//gobber.ShowWindow(wnd.Gfx(), "gobber");
	//wall.ShowWindow(wnd.Gfx(), "Wall");
	//tp.SpawnControlWindow(wnd.Gfx());
	//nano.ShowWindow(wnd.Gfx(), "Model 1");
	//sponza.ShowWindow(wnd.Gfx(), "Sponza");
	//bluePlane.SpawnControlWindow(wnd.Gfx(), "Blue Plane");
	//redPlane.SpawnControlWindow(wnd.Gfx(), "Red Plane");

	//present
	switch (data.vsync) {
	case true:
		wnd.Gfx().PushFrame(); //flips the back buffer to the front where it will be drawn on screen (With VSync)
		break;
	case false:
		wnd.Gfx().PushFrame_NO_VSYNC(); //flips the back buffer to the front where it will be drawn on screen (No VSync)
		break;
	};
	fc.Reset();
}