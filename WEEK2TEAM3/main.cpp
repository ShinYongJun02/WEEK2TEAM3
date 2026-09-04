#include "Core.h"

#include "Sphere.h"
#include "URenderer.h"
#include "UObject.h"
#include "UCamera.h"
#include "MathF.h"

// 화면 경계
const float leftBorder = -1.0f;
const float rightBorder = 1.0f;
const float topBorder = 1.0f;
const float bottomBorder = -1.0f;

class TWindowEventHandler {
public:
	TWindowEventHandler(URenderer& renderer) : Renderer(renderer) {}

	void HandleResize(UINT width, UINT height) 
	{
		Renderer.Resize(width, height);
	}

private:
	URenderer& Renderer;
};

struct FCamera
{
	FVector Position;
	FVector Rotation;

	float Fov;
	float ZNear;
	float ZFar;

	inline FMatrix GetViewMatrix()
	{
		FVector4 target = FVector4(Front, 0.0f) * RotateY(Rotation.y) * RotateX(Rotation.x);
		return LookAt(Position, Position + FVector(target.x, target.y, target.z), Up);
	}
};

void CreateDebugConsole() {
	// 1. Allocate a new console for the calling process
	if (AllocConsole()) {
		FILE* fp;

		// 2. Redirect standard output (stdout) to the console
		freopen_s(&fp, "CONOUT$", "w", stdout);
		// 3. Redirect standard error (stderr) to the console
		freopen_s(&fp, "CONOUT$", "w", stderr);
		// 4. Redirect standard input (stdin) to the console
		freopen_s(&fp, "CONIN$", "r", stdin);

		// 5. Clear the error state for each of the C++ standard streams
		std::clog.clear();
		std::cerr.clear();
		std::cout.clear();
		std::cin.clear();

		// Optional: Set a title for your debug window
		SetConsoleTitle(L"Debug Console");
	}
}

extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	TWindowEventHandler* eventHandler = reinterpret_cast<TWindowEventHandler*>(GetWindowLongPtrW(hWnd, GWLP_USERDATA));
	
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
	case WM_SIZE:
		if (eventHandler) 
		{
			UINT width = LOWORD(lParam);
			UINT height = HIWORD(lParam);
			eventHandler->HandleResize(width, height);
		}
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}

	return 0;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
	{
	#if _DEBUG
	CreateDebugConsole();
	#endif

	WCHAR WindowClass[] = L"JungleWindowClass";
	WCHAR Title[] = L"Game Tech Lab";

	WNDCLASSW wndclass = { 0, WndProc, 0, 0, 0, 0, 0, 0, 0, WindowClass };
	RegisterClassW(&wndclass);

	HWND hWnd = CreateWindowExW(0, WindowClass, Title,
		WS_POPUP | WS_VISIBLE | WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT, 1920, 1080, nullptr, nullptr, hInstance, nullptr);

	// 렌더러 초기화
	URenderer renderer;
	renderer.Create(hWnd);
	renderer.CreateShader();
	renderer.CreateConstantBuffer();

	TWindowEventHandler windowEventHandler(renderer);

	SetWindowLongPtrW(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(&windowEventHandler));

	// ImGui 초기화
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	ImGui_ImplWin32_Init((void*)hWnd);
	ImGui_ImplDX11_Init(renderer.Device, renderer.DeviceContext);

	// FPS 관리
	const int targetFPS = 144;
	const double targetFrameTime = 1000.0 / targetFPS;
	LARGE_INTEGER frequency;
	QueryPerformanceFrequency(&frequency);
	LARGE_INTEGER startTime, endTime;
	double elapsedTime = 0.0;

	// 리소스 생성
	UINT numVerticesCube = sizeof(cube_vertices) / sizeof(FVertexSimple);
	ID3D11Buffer* vertexBufferCube = renderer.CreateVertexBuffer(cube_vertices, sizeof(cube_vertices));

	// 큐브
	UObject cube;

	// 카메라
	UCamera camera;
	camera.Translation += FVector(-5.0f);
	bool pressed[7] = {}; // WSDAEQ, MR
	float cameraSpeed = 5.0f;

	// 마우스 추적
	POINT pt;
	GetCursorPos(&pt);          // 화면 좌표
	ScreenToClient(hWnd, &pt);  // 클라이언트 좌표로 변환

	// 종료 시그널
	bool bIsExit = false;

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
			else if (msg.message == WM_KEYDOWN)
			{
				if (msg.wParam == 'W')
				{
					pressed[0] = true;
				}
				if (msg.wParam == 'S')
				{
					pressed[1] = true;
				}
				if (msg.wParam == 'D')
				{
					pressed[2] = true;
				}
				if (msg.wParam == 'A')
				{
					pressed[3] = true;
				}
				if (msg.wParam == 'E')
				{
					pressed[4] = true;
				}
				if (msg.wParam == 'Q')
				{
					pressed[5] = true;
				}
			}
			else if (msg.message == WM_KEYUP)
			{
				if (msg.wParam == 'W')
				{
					pressed[0] = false;
				}
				if (msg.wParam == 'S')
				{
					pressed[1] = false;
				}
				if (msg.wParam == 'D')
				{
					pressed[2] = false;
				}
				if (msg.wParam == 'A')
				{
					pressed[3] = false;
				}
				if (msg.wParam == 'E')
				{
					pressed[4] = false;
				}
				if (msg.wParam == 'Q')
				{
					pressed[5] = false;
				}
			}
			else if (msg.message == WM_RBUTTONDOWN)
			{
				pressed[6] = true;
			}
			else if (msg.message == WM_RBUTTONUP)
			{
				pressed[6] = false;
			}
		}

		// 마우스 추적
		POINT temp;
		GetCursorPos(&temp);          // 화면 좌표
		ScreenToClient(hWnd, &temp);  // 클라이언트 좌표로 변환
		float distX = temp.x - pt.x;
		float distY = pt.y - temp.y;
		pt = temp;

		// 카메라 무빙 (카메라 z축 회전 = 오른쪽 보기, 카메라 y축 회전 = 아래 보기
		camera.Translation += FVector(pressed[0] - pressed[1], pressed[2] - pressed[3], pressed[4] - pressed[5]) * ((float)elapsedTime / 1000.0f);
		if (pressed[6])
		{
			camera.Rotation += FVector(0.0f, distY, distX) * ((float)elapsedTime / 1000.0f) * cameraSpeed;
		}

		// Transform
		cube.Rotation += FVector(0.0f, 0.0f, 90.0f) * ((float)elapsedTime / 1000.0f);

		renderer.Prepare();
		renderer.PrepareShader();

		renderer.UpdateConstant(modelMatrix, viewMatrix, projMatrix);
		renderer.DrawIndexed(cubeVertexBuffer, cubeIndexBuffer, 36);

		ImGui_ImplDX11_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();

		// ImGui
		ImGui::Begin("Debug Cube");
		ImGui::DragFloat3("Translation", &cube.Translation.x, 0.1f);
		ImGui::DragFloat3("Rotation", &cube.Rotation.x, 0.1f);
		ImGui::DragFloat3("Scale", &cube.Scale.x, 0.1f);
		ImGui::End();

		ImGui::Begin("Debug Camera");
		ImGui::DragFloat3("Translation", &camera.Translation.x, 0.1f);
		ImGui::DragFloat3("Rotation", &camera.Rotation.x, 0.1f);
		ImGui::DragFloat("fovY", &camera.fovY, 0.1f);
		ImGui::Text("%d", pressed[6]);
		ImGui::End();

		ImGui::Render();
		ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
	
		// 그리기 명령 실행
		renderer.SwapBuffer();

		do
		{
			Sleep(0);
			QueryPerformanceCounter(&endTime);

			// 한 프레임이 소요된 시간 계산 (밀리초 단위로 변환)
			elapsedTime = (endTime.QuadPart - startTime.QuadPart) * 1000.0 / frequency.QuadPart;
		} while (elapsedTime < targetFrameTime);
	}

	renderer.ReleaseVertexBuffer(cubeVertexBuffer);
	renderer.ReleaseIndexBuffer(cubeIndexBuffer);

	ImGui_ImplDX11_Shutdown();	//ImGui 리소스 해제
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();

	// 렌더러 리소스 해제
	renderer.ReleaseConstantBuffer();
	renderer.ReleaseShader();
	renderer.Release();

	return 0;
}
