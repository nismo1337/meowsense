#pragma once
#include "Target.hpp"

namespace OW {
	inline void scan_matrix_ex();
	inline void entity_thread() {
		while (true) {
			std::vector<DWORD64>temp_entity = Entity_t;
			if (temp_entity.size() > 0)
			{
				for (int i = 0; i < temp_entity.size(); i++)
				{
					Entitys[i].head = SDK->RPM<Vector3>(temp_entity[i] - 0x9C);
					Entitys[i].Loc = Vector3(Entitys[i].head.X + 0.5f, Entitys[i].head.Y + 0.6f, Entitys[i].head.Z + 0.5f);
					Entitys[i].foot = Vector3(Entitys[i].head.X, Entitys[i].head.Y - 2.f, Entitys[i].head.Z);
					Entitys[i].Live = SDK->RPM<BYTE>(temp_entity[i] - 0x1) == 0x14;

					if (!Config::is_team)
						Entitys[i].Team = SDK->RPM<BYTE>(temp_entity[i] - 0x4) != 0x8;
					else
						Entitys[i].Team = SDK->RPM<BYTE>(temp_entity[i] - 0x4) != 0x10;
				}
				viewMatrix = SDK->RPM<Matrix>(viewMatrixPtr);
				//scan_matrix_ex();
				Sleep(1);
			}
			else
			{
				Sleep(5);
			}
		}
	}

	inline void esp() {
		__try {
			ImDrawList* Draw = ImGui::GetBackgroundDrawList();
			ImVec2 CrossHair = ImVec2(GetSystemMetrics(SM_CXSCREEN) / 2.0f, GetSystemMetrics(SM_CYSCREEN) / 2.0f);
			if (Config::draw_edge) {
				for (int i = 0; i < Entity_t.size(); i++) {
					if (Entitys[i].Team && Entitys[i].Live) {
						Vector3 Vec3 = Entitys[i].Loc;
						Vector2 Vec2_A{}, Vec2_B{};
						if (!viewMatrix.WorldToScreen(Vector3(Vec3.X, Vec3.Y - 1.5f, Vec3.Z), &Vec2_A))
							return;
						if (!viewMatrix.WorldToScreen(Vector3(Vec3.X, Vec3.Y + 0.8f, Vec3.Z), &Vec2_B))
							return;

						float Size = abs(Vec2_A.Y - Vec2_B.Y) / 2.0f;
						float Size2 = abs(Vec2_A.Y - Vec2_B.Y) / 20.0f;
						float Height2 = abs(Vec2_A.Y - Vec2_B.Y);
						int num7 = (float)(1500 / (int)Height2);
						float xpos = (Vec2_A.X + Vec2_B.X) / 2.f;
						float ypos = Vec2_A.Y + Size / 5;

						Render::DrawEdges(ImVec2(xpos - Size / 1.5f, Vec2_A.Y), ImVec2(xpos + Size / 1.5f, Vec2_B.Y), ImVec2(xpos - Size2 / 1.5f, Vec2_A.Y), ImGui::GetColorU32(Config::EdgeESPColor));
					}
				}
			}
			if (Config::draw_fov) {
				Draw->AddCircle(CrossHair, Config::Fov, 0xFFFFFFFF, 100);
			}
		}
		__except (0) {

		}
	}

	inline void overlay_thread() {
		std::chrono::system_clock::time_point a = std::chrono::system_clock::now();
		std::chrono::system_clock::time_point b = std::chrono::system_clock::now();

		HWND tWnd = FindWindowA("TankWindowClass", NULL);

		WNDCLASSEX wc = { sizeof(WNDCLASSEX), CS_CLASSDC, WndProc, 0L, 0L, GetModuleHandle(NULL), NULL, NULL, NULL, NULL, "OVERWATCH", NULL };
		RegisterClassEx(&wc);
		HWND hwnd = CreateWindow(wc.lpszClassName, "OVERWATCH", WS_POPUP, 0, 0, 0, 0, NULL, NULL, wc.hInstance, NULL);


		if (!CreateDeviceD3D(hwnd))
		{
			CleanupDeviceD3D();
			UnregisterClass(wc.lpszClassName, wc.hInstance);
		}

		MARGINS margins = { -1 };
		DwmExtendFrameIntoClientArea(hwnd, &margins);

		SetWindowPos(hwnd, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW);
		SetWindowLong(hwnd, GWL_EXSTYLE, WS_EX_LAYERED | WS_EX_TRANSPARENT | WS_EX_NOACTIVATE);



		ShowWindow(hwnd, SW_SHOWDEFAULT);
		UpdateWindow(hwnd);


		IMGUI_CHECKVERSION();
		ImGui::CreateContext();

		ImGuiIO& io = ImGui::GetIO(); (void)io;

		ImGui::StyleColorsDark();

		ImGui_ImplWin32_Init(tWnd);
		ImGui_ImplDX11_Init(g_pd3dDevice, g_pd3dDeviceContext);

		MSG msg;
		ZeroMemory(&msg, sizeof(msg));
		int FPS;
		while (FindWindowA("TankWindowClass", NULL))
		{

			DEVMODE dm;
			dm.dmSize = sizeof(DEVMODE);

			EnumDisplaySettings(NULL, ENUM_CURRENT_SETTINGS, &dm);

			FPS = dm.dmDisplayFrequency;

			a = std::chrono::system_clock::now();
			std::chrono::duration<double, std::milli> work_time = a - b;
			if (work_time.count() < 1000 / FPS)
			{
				std::chrono::duration<double, std::milli> delta_ms(1000 / FPS - work_time.count());
				auto delta_ms_duration = std::chrono::duration_cast<std::chrono::milliseconds>(delta_ms);
				std::this_thread::sleep_for(std::chrono::milliseconds(delta_ms_duration.count()));
			}

			b = std::chrono::system_clock::now();
			std::chrono::duration<double, std::milli> sleep_time = b - a;

			DWORD Style = GetWindowLong(tWnd, GWL_STYLE);
			RECT rect;
			GetWindowRect(tWnd, &rect);

			SetWindowPos(hwnd, HWND_TOPMOST, rect.left, rect.top, rect.right, rect.bottom, SWP_NOZORDER);

			SetWindowPos(hwnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW);

			if (::PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE))
			{
				::TranslateMessage(&msg);
				::DispatchMessage(&msg);
				continue;
			}

			if (GetAsyncKeyState(VK_LBUTTON))
			{
				ImGui::GetIO().MouseDown[0] = true;
			}
			else
			{
				ImGui::GetIO().MouseDown[0] = false;
			}

			// Start the Dear ImGui frame
			ImGui_ImplDX11_NewFrame();
			ImGui_ImplWin32_NewFrame();
			ImGui::NewFrame();

			if (Config::Menu)
			{
				ImGui::SetNextWindowPos(ImVec2{ 50, 30 }, ImGuiCond_Once);
				ImGui::SetNextWindowSize(ImVec2{ 430, 470 }, ImGuiCond_Once);
				bool _visible = true;
				if (ImGui::Begin(u8"meowsense", &_visible, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoTitleBar)) {
					ImGui::BulletText(u8"meowsense");

					ImGui::Separator();

					ImGui::Checkbox("Team Change", &Config::is_team);

					ImGui::Separator();

					ImGui::Checkbox("Edge esp", &Config::draw_edge);
					ImGui::Checkbox("Fov", &Config::draw_fov);

					ImGui::Separator();

					ImGui::Checkbox("Tracking", &Config::Tracking);
					ImGui::Checkbox("Silent", &Config::Flick);

					ImGui::BulletText(u8"Fov");
					ImGui::SliderFloat("Fov", &Config::Fov, 20.0f, 1000.0f, "%.3f");

					ImGui::BulletText(u8"Smooth");
					ImGui::SliderFloat("Smooth1", &Config::Tracking_smooth, 0.f, 5.f, "%.1f");
					//ImGui::SliderFloat("Smooth2", &Config::Flick_smooth, 0.f, 5.f, "%.1f");

					ImGui::Separator();
				}
				ImGui::End();
			}

			esp();
			ImGui::EndFrame();
			ImGui::Render();
			g_pd3dDeviceContext->OMSetRenderTargets(1, &g_mainRenderTargetView, NULL);
			auto ss = ImVec4(0, 0, 0, 0);
			g_pd3dDeviceContext->ClearRenderTargetView(g_mainRenderTargetView, (float*)&ss);
			ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

			g_pSwapChain->Present(1, 0);
		}

		// Cleanup
		ImGui_ImplDX11_Shutdown();
		ImGui_ImplWin32_Shutdown();
		ImGui::DestroyContext();

		CleanupDeviceD3D();
		::DestroyWindow(hwnd);
		::UnregisterClass(wc.lpszClassName, wc.hInstance);
	}

	inline void aimbot_thread() {
		__try {
			timeBeginPeriod(1);
			Vector2 CrossHair = Vector2(GetSystemMetrics(SM_CXSCREEN) / 2.0f, GetSystemMetrics(SM_CYSCREEN) / 2.0f);
			while (true) {
				//Tracking
				if (Config::Tracking) {
					while (GetAsyncKeyState(VK_LBUTTON))
					{
						Vector2 TarGet = GetAimEnemy();
						if (TarGet.X != 0 && TarGet.Y != 0)
						{
							DD_movR(TarGet.X * Config::Tracking_smooth / (3.f * 8.f), TarGet.Y * Config::Tracking_smooth / (3.f * 8.f));
						}
						Sleep(1);
					}
				}
				//Flick
				else if (Config::Flick) {
					Vector2 TarGets = GetAimEnemy();
					while (GetAsyncKeyState(VK_XBUTTON2))
					{
						Vector2 TarGet = GetAimEnemy();
						if (TarGet.X != 0 && TarGet.Y != 0)
						{
							//DD_movR(TarGet.X * Config::Flick_smooth / 10.f / 1.0f, TarGet.Y * Config::Flick_smooth / 10.f / 1.0f);
							DD_movR(TarGet.X, TarGet.Y);
							DD_btn(1); // 마우스 클릭
							DD_btn(2); // 마우스 뗀다
							Sleep(50);
							DD_movR(-TarGet.X, -TarGet.Y);
							//if (std::abs(TarGet.X - CrossHair.X) > 0.5f && std::abs(TarGet.Y - CrossHair.Y) > 0.5f) {
							//	DD_btn(1); // 마우스 클릭
							//	DD_btn(2); // 마우스 뗀다
							//}
						}
						Sleep(250);
					}
					Sleep(1);
				}
			}
			timeBeginPeriod(1);
		}
		__except (1) {
			Sleep(1);
		}
	}

	inline bool compress_mbis = false;
	inline std::vector<MEMORY_BASIC_INFORMATION64> mbis = {};

	inline bool update_memory_query()
	{
		MEMORY_BASIC_INFORMATION64 mbi = { 0, };
		MEMORY_BASIC_INFORMATION64 old = { 0, };
		uintptr_t current_address = 0x7ffe0000;
		std::vector<MEMORY_BASIC_INFORMATION64> addresses;
		while (true)
		{
			if (!VirtualQueryEx(SDK->hProcess, (LPVOID)current_address, (PMEMORY_BASIC_INFORMATION)&mbi, sizeof(MEMORY_BASIC_INFORMATION64)))
				break;
			if ((mbi.State & 0x1000) != 0 && (mbi.Protect & 0x100) == 0)
			{
				if (old.BaseAddress + old.RegionSize == mbi.BaseAddress && compress_mbis)
					old.RegionSize += mbi.RegionSize;
				else
					addresses.push_back(mbi);

				old = mbi;
			}
			current_address = mbi.BaseAddress + mbi.RegionSize;
		}

		mbis = addresses;

		return (mbis.size() > 0);
	}

	inline void scan_matrix_ex()
	{
		if (!update_memory_query())
			return;

		Matrix view_matrix{};
		Vector3 local_pos = Vector3(viewMatrix.get_location().x, viewMatrix.get_location().y, viewMatrix.get_location().z);

		for (size_t i = 0; i < mbis.size(); ++i)
		{
			MEMORY_BASIC_INFORMATION64 mbi = mbis[i];

			char* buffer = new char[mbi.RegionSize];
			SDK->read_buf(mbi.BaseAddress, buffer, mbi.RegionSize);

			for (int j = 0; j < mbi.RegionSize; j++)
			{
				memcpy(&view_matrix, &buffer[j], sizeof(Matrix));
				auto local_camera = view_matrix.GetCameraVec();

				if (local_pos.DistTo(local_camera) < 2.0f)
				{
					printf("vm found! %p %.2f %.2f %.2f | %.2f %.2f %.2f\n", mbi.BaseAddress + j, local_pos.X, local_pos.Y, local_pos.Z, local_camera.X, local_camera.Y, local_camera.Z);
				}
			}

			delete[]buffer;
		}

		printf("finished\n");
	}
}