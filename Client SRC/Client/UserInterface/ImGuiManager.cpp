#include "StdAfx.h"

#if defined(__IMGUI__)
#include "ImGuiManager.h"

#include "../EterLib/StateManager.h"
#include "../EterLib/ImGui/imgui.h"
#include "../EterLib/ImGui/imgui_impl_dx8.h"
#include "../EterLib/ImGui/imgui_impl_win32.h"

#include "../EterLib/Camera.h"
#include "../EterPack/EterPackManager.h"

#include "PythonApplication.h"
#include "PythonSystem.h"
#include "PythonNetworkStream.h"
#include "InstanceBase.h"

#include <commdlg.h>

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
extern bool PERF_CHECKER_RENDER_GAME;

std::string FormatMemorySize(DWORD bytes)
{
	const double kb = 1024.0;
	const double mb = kb * 1024.0;
	const double gb = mb * 1024.0;

	std::ostringstream oss;

	if (bytes >= gb)
		oss << std::fixed << std::setprecision(2) << (bytes / gb) << " GB";
	else if (bytes >= mb)
		oss << std::fixed << std::setprecision(2) << (bytes / mb) << " MB";
	else if (bytes >= kb)
		oss << std::fixed << std::setprecision(2) << (bytes / kb) << " KB";
	else
		oss << bytes << " bytes";

	return oss.str();
}

void ImGuiManager::Create(HWND hWnd, LPDIRECT3DDEVICE8 pDevice)
{
	IMGUI_CHECKVERSION();

	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
	io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
	io.ConfigFlags |= ImGuiConfigFlags_NoMouseCursorChange;

	//ImGui::StyleColorsClassic();
	ImGui::StyleColorsDark();

	ImGuiStyle& style = ImGui::GetStyle();
	if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		style.WindowRounding = 0.0f;
		style.Colors[ImGuiCol_WindowBg].w = 1.0f;
	}

	ImGui_ImplWin32_Init(hWnd);
	ImGui_ImplDX8_Init(pDevice);
}

void ImGuiManager::Destroy()
{
	ImGui_ImplDX8_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
}

LRESULT ImGuiManager::WndProcHandler(HWND hWnd, UINT uiMsg, WPARAM wParam, LPARAM lParam)
{
	return ImGui_ImplWin32_WndProcHandler(hWnd, uiMsg, wParam, lParam);
}

void ImGuiManager::Start()
{
	ImGui_ImplDX8_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
}

void ImGuiManager::Render()
{
	CPythonApplication& rkApp = CPythonApplication::Instance();

	if (ImGui::BeginMainMenuBar())
	{
		if (ImGui::BeginMenu("Options"))
		{
			ImGui::MenuItem("Performance Check", "perf_game_render.txt", &PERF_CHECKER_RENDER_GAME);
			if (ImGui::MenuItem("Load Script", "Loads Python script file."))
			{
				char szFileName[MAX_PATH] = "";
				OPENFILENAME OpenFileName;
				ZeroMemory(&OpenFileName, sizeof(OpenFileName));

				OpenFileName.lStructSize = sizeof(OpenFileName);
				OpenFileName.hwndOwner = NULL;
				OpenFileName.lpstrFilter = "Python Files\0*.py\0All Files\0*.*\0";
				OpenFileName.lpstrFile = szFileName;
				OpenFileName.nMaxFile = MAX_PATH;
				OpenFileName.Flags = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST;
				OpenFileName.lpstrDefExt = "py";

				if (GetOpenFileName(&OpenFileName))
					CPythonLauncher::Instance().RunFile(szFileName);
			}
			ImGui::EndMenu();
		}
		ImGui::Separator();

		char szTextureMemoryInfo[1024 + 1];
		snprintf(szTextureMemoryInfo, sizeof(szTextureMemoryInfo), "Available Texture Memory: %s",
			FormatMemorySize(CGraphicBase::GetAvailableTextureMemory()).c_str());

		ImGui::Text(szTextureMemoryInfo);
		ImGui::Separator();

		std::string strActorInfo;
		rkApp.GetInfo(CPythonApplication::INFO_ACTOR, &strActorInfo);
		ImGui::Text("%s", strActorInfo.c_str());
	}
	ImGui::EndMainMenuBar();
}

void ImGuiManager::End()
{
	ImGui::EndFrame();

	const DWORD state_D3DRS_ZENABLE = STATEMANAGER.GetRenderState(D3DRS_ZENABLE);
	const DWORD state_D3DRS_ALPHABLENDENABLE = STATEMANAGER.GetRenderState(D3DRS_ALPHABLENDENABLE);
	//const DWORD state_D3DRS_SCISSORTESTENABLE = STATEMANAGER.GetRenderState(D3DRS_SCISSORTESTENABLE);

	STATEMANAGER.SetRenderState(D3DRS_ZENABLE, FALSE);
	STATEMANAGER.SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
	//STATEMANAGER.SetRenderState(D3DRS_SCISSORTESTENABLE, FALSE);

	ImGui::Render();
	ImGui_ImplDX8_RenderDrawData(ImGui::GetDrawData());

	const ImGuiIO& io = ImGui::GetIO();
	if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		ImGui::UpdatePlatformWindows();
		ImGui::RenderPlatformWindowsDefault();
	}

	STATEMANAGER.SetRenderState(D3DRS_ZENABLE, state_D3DRS_ZENABLE);
	STATEMANAGER.SetRenderState(D3DRS_ALPHABLENDENABLE, state_D3DRS_ALPHABLENDENABLE);
	//STATEMANAGER.SetRenderState(D3DRS_SCISSORTESTENABLE, state_D3DRS_SCISSORTESTENABLE);
}
#endif
