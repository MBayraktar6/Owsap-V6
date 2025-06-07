#pragma once

#if defined(__IMGUI__)
namespace ImGuiManager
{
	void Create(HWND hWnd, LPDIRECT3DDEVICE8 pDevice);
	void Destroy();

	LRESULT WndProcHandler(HWND hWnd, UINT uiMsg, WPARAM wParam, LPARAM lParam);

	// Render Stage
	void Start();
	void Render();
	void End();
}
#endif
