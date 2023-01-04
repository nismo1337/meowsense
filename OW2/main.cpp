#pragma once
#include "includes.hpp"
#include "SDK.hpp"
#include "Overwatch.hpp"

int main() {
	using namespace OW;

	static char CurrDir[MAX_PATH] = "";
	char MyDir[MAX_PATH];
	GetCurrentDirectory(MAX_PATH, MyDir);

	strcpy_s(MyDir, "lib.dll");
	HINSTANCE hInst = LoadLibrary(MyDir);

	DD_btn = (pDD_btn)GetProcAddress(hInst, "DD_btn");
	DD_key = (pDD_key)GetProcAddress(hInst, "DD_key");
	DD_movR = (pDD_movR)GetProcAddress(hInst, "DD_movR");

	if (!SDK->Initialize() || !hInst)
		exit(0);

	printf("hProcess: %p\n", SDK->hProcess);
	printf("BaseAddress: %p\n", SDK->dwGameBase);

	_beginthread((_beginthread_proc_type)entity_thread, 0, 0);
	_beginthread((_beginthread_proc_type)aimbot_thread, 0, 0);
	_beginthread((_beginthread_proc_type)overlay_thread, 0, 0);

	while (FindWindowA("TankWindowClass", NULL))
	{
		BYTE AOB[] = "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\xFF\xFF\x02\x00\x01\x00\x00\x00\x00\x00\x00\x00\x00";
		std::string MASK = "??xx??xx?xxx?xxxxxxxxxxxxxxx???";

		Entity_t = SDK->FindPatternsExRegs(AOB, MASK, 0x180000);

		uint64_t viewMatrixVal = SDK->RPM<uint64_t>(SDK->dwGameBase + offset::Address_viewmatrix_base) ^ offset::offset_viewmatrix_xor_key;
		viewMatrixVal = SDK->RPM<uint64_t>(viewMatrixVal + 0x5A0);
		viewMatrixVal = SDK->RPM<uint64_t>(viewMatrixVal + 0x118);
		auto view = SDK->RPM<uint64_t>(SDK->dwGameBase + offset::Address_viewmatrix_base_test) + offset::offset_viewmatrix_ptr;
		viewMatrixPtr = view;
		viewMatrix = SDK->RPM<Matrix>(viewMatrixPtr);

		if (GetAsyncKeyState(VK_INSERT)) {
			Config::Menu = !Config::Menu;
			Sleep(250);
		}

		Sleep(150);
	}

	return 1;
}