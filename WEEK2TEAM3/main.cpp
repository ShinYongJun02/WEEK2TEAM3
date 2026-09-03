#pragma comment(lib, "user32")			
#pragma comment(lib, "d3d11")			
#pragma comment(lib, "d3dcompiler")		

#include <windows.h>					
#include <d3d11.h>						
#include <d3dcompiler.h>			

#include "ImGui/imgui.h"				
#include "ImGui/imgui_internal.h"		
#include "ImGui/imgui_impl_dx11.h"		
#include "ImGui/imgui_impl_win32.h"		

// FVertexSimple, triangle_vertices, cube_vertices, sphere_vertices
#include "Sphere.h"
#include "URenderer.h"

// 화면 경계
const float leftBorder = -1.0f;
const float rightBorder = 1.0f;
const float topBorder = 1.0f;
const float bottomBorder = -1.0f;

// 중력
const FVector G(0.0f, -9.8f, 0.0f);

extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	// ImGui 메시지 처리
	if (ImGui_ImplWin32_WndProcHandler(hWnd, message, wParam, lParam))
	{
		return true;
	}

	switch (message)
	{
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}

	return 0;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
	WCHAR WindowClass[] = L"JungleWindowClass";
	WCHAR Title[] = L"Game Tech Lab";

	WNDCLASSW wndclass = { 0, WndProc, 0, 0, 0, 0, 0, 0, 0, WindowClass };
	RegisterClassW(&wndclass);

	HWND hWnd = CreateWindowExW(0, WindowClass, Title,
		WS_POPUP | WS_VISIBLE | WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT, 1024, 1024, nullptr, nullptr, hInstance, nullptr);

	// 렌더러 초기화
	URenderer renderer;
	renderer.Create(hWnd);
	renderer.CreateShader();
	renderer.CreateConstantBuffer();

	// ImGui 초기화
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	ImGui_ImplWin32_Init((void*)hWnd);
	ImGui_ImplDX11_Init(renderer.Device, renderer.DeviceContext);

	const int targetFPS = 144;
	const double targetFrameTime = 1000.0 / targetFPS;	// 한 프레임의 목표 시간 (밀리초 단위)

	LARGE_INTEGER frequency;	// tick/sec
	QueryPerformanceFrequency(&frequency);

	LARGE_INTEGER startTime, endTime;
	double elapsedTime = 0.0;

	bool bIsExit = false;

	// Main Loop (Quit Message가 들어오기 전까지 아래 Loop를 무한히 실행하게 됨)
	while (bIsExit == false)
	{
		QueryPerformanceCounter(&startTime);

		MSG msg;

		while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);

			if (msg.message == WM_QUIT)
			{
				bIsExit = true;
				break;
			}
		}

		renderer.Prepare();
		renderer.PrepareShader();

		ImGui_ImplDX11_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();

		ImGui::Render();										// 그리기 명령 준비	
		ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());	// 그리기 명령 실행

		renderer.SwapBuffer();

		do	// 프레임 대기
		{
			Sleep(0);
			QueryPerformanceCounter(&endTime);

			// 한 프레임이 소요된 시간 계산 (밀리초 단위로 변환)
			elapsedTime = (endTime.QuadPart - startTime.QuadPart) * 1000.0 / frequency.QuadPart;
		} while (elapsedTime < targetFrameTime);
	}

	ImGui_ImplDX11_Shutdown();	//ImGui 리소스 해제
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();

	// 렌더러 리소스 해제
	renderer.ReleaseConstantBuffer();
	renderer.ReleaseShader();
	renderer.Release();

	return 0;
}