#pragma once
#define DEG2RAD(x) x * M_PI / 180.0
#define M_PI       3.14159265358979323846
#define DIRECTINPUT_VERSION 0x0800

/* WinApi C++ 17 */
#include <Windows.h>
#include <iostream>
#include <filesystem>
#include <fstream>
#include <string>
#include <direct.h>
#include <TlHelp32.h>
#include <vector>
#include <process.h>
#include <thread>
#include <bitset>
#include <mutex>
#include <array>
#include <dwmapi.h>

/* D3D SDK */
#include <d3d11.h>
#include <DirectXMath.h>
#include <d3d9types.h>

/* Overlay & Imgui */
#include "ImGui/imgui.h"
#include "ImGui/imgui_impl_dx11.h"
#include "ImGui/imgui_impl_win32.h"
#include "ImGui/imgui_internal.h""

/* String Encrypted */
#include "Xor.hpp"

/* Offset & Address */
#include "Offsets.hpp"

/* Config */
#include "Config.hpp"

/* Render */
#include "Renderer.hpp"
#include "Vector.hpp"

/* Lib */
#pragma comment(lib, "winmm.lib")
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dwmapi.lib")

#pragma warning(disable : 4996)

namespace fs = std::filesystem;
using namespace DirectX;

typedef int (FAR WINAPI* pDD_btn)(int btn);
typedef int (FAR WINAPI* pDD_key)(int keycode, int flag);
typedef int (FAR WINAPI* pDD_movR)(int dx, int dy);

static pDD_btn  DD_btn{};
static pDD_key  DD_key{};
static pDD_movR DD_movR{};

// Forward declare message handler from imgui_impl_win32.cpp
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

namespace OW {
	struct Entity
	{
		Vector3 head;

		Vector3 Loc;

		Vector3 foot;

		bool Live;

		bool Team;
	};

	inline UINT g_Width{}, g_Height{};
	inline std::vector<uint64_t> Entity_t{};
	inline Entity Entitys[20] = {};
	inline Matrix viewMatrix{};
	inline uint64_t viewMatrixPtr{};

	// Data
	static ID3D11Device* g_pd3dDevice = NULL;
	static ID3D11DeviceContext* g_pd3dDeviceContext = NULL;
	static IDXGISwapChain* g_pSwapChain = NULL;
	static ID3D11RenderTargetView* g_mainRenderTargetView = NULL;

	inline void CleanupRenderTarget()
	{
		if (g_mainRenderTargetView) { g_mainRenderTargetView->Release(); g_mainRenderTargetView = NULL; }
	}

	inline void CleanupDeviceD3D()
	{
		CleanupRenderTarget();
		if (g_pSwapChain) { g_pSwapChain->Release(); g_pSwapChain = NULL; }
		if (g_pd3dDeviceContext) { g_pd3dDeviceContext->Release(); g_pd3dDeviceContext = NULL; }
		if (g_pd3dDevice) { g_pd3dDevice->Release(); g_pd3dDevice = NULL; }
	}

	inline void CreateRenderTarget()
	{
		ID3D11Texture2D* pBackBuffer;
		g_pSwapChain->GetBuffer(0, IID_PPV_ARGS(&pBackBuffer));
		g_pd3dDevice->CreateRenderTargetView(pBackBuffer, NULL, &g_mainRenderTargetView);
		pBackBuffer->Release();
	}

	inline bool CreateDeviceD3D(HWND hWnd)
	{
		// Setup swap chain
		DXGI_SWAP_CHAIN_DESC sd;
		ZeroMemory(&sd, sizeof(sd));
		sd.BufferCount = 2;
		sd.BufferDesc.Width = 0;
		sd.BufferDesc.Height = 0;
		sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		sd.BufferDesc.RefreshRate.Numerator = 60;
		sd.BufferDesc.RefreshRate.Denominator = 1;
		sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
		sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		sd.OutputWindow = hWnd;
		sd.SampleDesc.Count = 1;
		sd.SampleDesc.Quality = 0;
		sd.Windowed = TRUE;
		sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

		UINT createDeviceFlags = 0;
		//createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
		D3D_FEATURE_LEVEL featureLevel;
		const D3D_FEATURE_LEVEL featureLevelArray[2] = { D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_0, };
		if (D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, createDeviceFlags, featureLevelArray, 2, D3D11_SDK_VERSION, &sd, &g_pSwapChain, &g_pd3dDevice, &featureLevel, &g_pd3dDeviceContext) != S_OK)
			return false;

		CreateRenderTarget();
		return true;
	}

	// Win32 message handler
	inline LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
	{
		if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
			return true;

		switch (msg)
		{
		case WM_SIZE:
			if (g_pd3dDevice != NULL && wParam != SIZE_MINIMIZED)
			{
				CleanupRenderTarget();
				g_pSwapChain->ResizeBuffers(0, (UINT)LOWORD(lParam), (UINT)HIWORD(lParam), DXGI_FORMAT_UNKNOWN, 0);
				CreateRenderTarget();
			}
			return 0;
		case WM_SYSCOMMAND:
			if ((wParam & 0xfff0) == SC_KEYMENU) // Disable ALT application menu
				return 0;
			break;
		case WM_DESTROY:
			::PostQuitMessage(0);
			return 0;
		}
		return ::DefWindowProc(hWnd, msg, wParam, lParam);
	}
}