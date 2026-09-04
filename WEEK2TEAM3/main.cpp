#include "Core.h"

#include "Sphere.h"
#include "URenderer.h"
#include "MathF.h"

// 화면 경계
const float leftBorder = -1.0f;
const float rightBorder = 1.0f;
const float topBorder = 1.0f;
const float bottomBorder = -1.0f;

class InputContext {
	int MouseX;
	int MouseY;
	char MouseBitFlags = 0;
};

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
		CW_USEDEFAULT, CW_USEDEFAULT, 1024, 1024, nullptr, nullptr, hInstance, nullptr);

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

	const int targetFPS = 144;
	const double targetFrameTime = 1000.0 / targetFPS;	// 한 프레임의 목표 시간 (밀리초 단위)

	LARGE_INTEGER frequency;	// tick/sec
	QueryPerformanceFrequency(&frequency);

	LARGE_INTEGER startTime, endTime;
	double elapsedTime = 0.0;

	bool bIsExit = false;

	FVertexBuffer cubeVertexBuffer = renderer.CreateVertexBuffer(CubeVertices, 8);
	ID3D11Buffer* cubeIndexBuffer = renderer.CreateIndexBuffer(CubeIndices, 36);

	FCamera camera;
	camera.Position = { 0.f };
	camera.Rotation = { 0.f };
	camera.Fov = ToRadian(45.f);
	camera.ZNear = 0.1f;
	camera.ZFar = 10.f;

	FVector cubePosition(0.0f, 0.0f, 3.f);
	FVector cubeRotation(0.0f);
	FVector cubeScale(1.0f, 1.0f, 1.0f);

	// Main Loop (Quit Message가 들어오기 전까지 아래 Loop를 무한히 실행하게 됨)
	QueryPerformanceCounter(&endTime);

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

		float deltaTime = float(endTime.QuadPart - startTime.QuadPart) * 1000.0;

		float aspect = renderer.GetWidth() / static_cast<float>(renderer.GetHeight());

		FMatrix viewMatrix = camera.GetViewMatrix();
		FMatrix projMatrix = Perspective(aspect, camera.Fov, camera.ZNear, camera.ZFar);

		FMatrix translationMatrix = Translate(cubePosition);
		FMatrix scaleMatrix = Scale(cubeScale);
		FMatrix rotationMatrix = RotateY(cubeRotation.x) * RotateX(cubeRotation.y) * RotateZ(cubeRotation.z);

		FMatrix modelMatrix = scaleMatrix * rotationMatrix * translationMatrix;

		renderer.Prepare();
		renderer.PrepareShader();

		renderer.UpdateConstant(modelMatrix, viewMatrix, projMatrix);
		renderer.DrawIndexed(cubeVertexBuffer, cubeIndexBuffer, 36);

		ImGui_ImplDX11_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();

		ImGui::Begin("Camera Control");
		ImGui::SliderFloat3("Position", &camera.Position.x, -5.0f, 5.0f);
		ImGui::SliderFloat2("Rotation", &camera.Rotation.x, PI * -2.0f, PI * 2.0f);
		ImGui::End();

		ImGui::Begin("Cube Control");
		ImGui::SliderFloat3("Position", &cubePosition.x, 0.0f, 5.0f);
		ImGui::SliderFloat3("Scale", &cubeScale.x, 0.1f, 1.0f);
		ImGui::SliderFloat3("Rotation", &cubeRotation.x, 0.f, PI * 2.0f);
		ImGui::End();

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