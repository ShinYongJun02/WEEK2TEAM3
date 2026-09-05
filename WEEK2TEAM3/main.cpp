#include "Core.h"
#include "FVertex.h"
#include "URenderer.h"
#include "UCamera.h"
#include "UResourceManager.h"


#include "UCubeComp.h"
#include "USphereComp.h"
#include "UPlaneComp.h"

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

UPrimitiveComponent* SpawnPrimitiveByType(int typeIndex, URenderer& Renderer)
{
	switch (typeIndex)
	{
	case 0:
		return NewObject<UCubeComp>(Renderer);
	case 1:
		return NewObject<USphereComp>(Renderer);
	case 2:
		return NewObject<UPlaneComp>(Renderer);
	default:
		return nullptr;
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

	// Init ResourceManager
	UResourceManager resourceManager;
	resourceManager.Initialize(renderer);

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

	// 카메라
	UCamera camera;
	camera.RelativeLocation += FVector(-5.0f);
	bool pressed[7] = {}; // WSDAEQ, MR
	float cameraSpeed = 5.0f;

	// 마우스 추적
	POINT pt;
	GetCursorPos(&pt);          // 화면 좌표
	ScreenToClient(hWnd, &pt);  // 클라이언트 좌표로 변환

	//outliner
	int32 SelectedObjectIndex = -1;

	//Place Actor
	static const char* PrimitiveTypeNames[] = { "Cube", "Sphere", "Plane" };
	static int SelectedPrimitiveIndex = 0;
	static int SpawnCount = 1;

	// 종료 시그널
	bool bIsExit = false;

	ImGuizmo::OPERATION TrsMode = ImGuizmo::TRANSLATE;
	ImGuizmo::MODE WlMode = ImGuizmo::WORLD;

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
				if (msg.wParam == 'Z')
				{
					TrsMode = ImGuizmo::TRANSLATE;
				}
				if (msg.wParam == 'X')
				{
					TrsMode = ImGuizmo::ROTATE;
				}
				if (msg.wParam == 'C')
				{
					TrsMode = ImGuizmo::SCALE;
				}
				if (msg.wParam == 'V')
				{
					WlMode = ImGuizmo::WORLD;
				}
				if (msg.wParam == 'B')
				{
					WlMode = ImGuizmo::LOCAL;
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

		float aspectRatio = (float)renderer.GetWidth() / (float)renderer.GetHeight();

		// 마우스 추적
		POINT temp;
		GetCursorPos(&temp);          // 화면 좌표
		ScreenToClient(hWnd, &temp);  // 클라이언트 좌표로 변환
		float distX = temp.x - pt.x;
		float distY = pt.y - temp.y;
		pt = temp;

		// 카메라 무빙 (카메라 z축 회전 = 오른쪽 보기, 카메라 y축 회전 = 아래 보기
		camera.RelativeLocation += (
			camera.GetForward() * (pressed[0] - pressed[1]) +
			camera.GetRight() * (pressed[2] - pressed[3]) +
			camera.GetUp() * (pressed[4] - pressed[5])) * ((float)elapsedTime / 1000.0f);
		if (pressed[6])
		{
			camera.RelativeRotation += FVector(0.0f, distY, distX) * ((float)elapsedTime / 1000.0f) * cameraSpeed;
		}

		// Transform
		renderer.Prepare();
		renderer.PrepareShader();

		renderer.UpdateViewConstant(camera.GetViewMatrix() * camera.GetProjectionMatrix(renderer.ViewportInfo.Width / renderer.ViewportInfo.Height));

		for (UObject* obj : GUObjectArray)
		{
			UPrimitiveComponent* prim = dynamic_cast<UPrimitiveComponent*>(obj);
			if (prim)
			{
				prim->Render(renderer);
			}
		}

		ImGui_ImplDX11_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();
		ImGuizmo::BeginFrame();

		// ImGui
		ImGui::Begin("Outliner");
		
		UObject* TempObject;
		UPrimitiveComponent* prim;
		for (int i = 0; i < GUObjectArray.size(); i++)
		{
			TempObject = GUObjectArray[i];

			prim = dynamic_cast<UPrimitiveComponent*>(TempObject);
			if (!prim) continue;

			char label[64];
			sprintf_s(label, "Object_%d%d%d%d", TempObject->UUID.A, TempObject->UUID.B, TempObject->UUID.C, TempObject->UUID.D);

			bool isSelected = (SelectedObjectIndex == prim->InternalIndex);
			if (ImGui::Selectable(label, isSelected))
			{
				SelectedObjectIndex = prim->InternalIndex;
			}
		}
		ImGui::End();

		UPrimitiveComponent* SelectedObject;

		ImGui::Begin("Details Panel");
		
		if (SelectedObjectIndex != -1)
		{
			TempObject = GUObjectArray[SelectedObjectIndex];
			SelectedObject = dynamic_cast<UPrimitiveComponent*>(TempObject);
			if (SelectedObject)
			{
				ImGui::DragFloat3("Translation", &SelectedObject->RelativeLocation.x, 0.1f);
				ImGui::DragFloat3("Rotation", &SelectedObject->RelativeRotation.x, 0.1f);
				ImGui::DragFloat3("Scale", &SelectedObject->RelativeScale3D.x, 0.1f);
			}
		}
		
		ImGui::End();

		// ImGuizmo

		if (SelectedObjectIndex != -1)
		{
			TempObject = GUObjectArray[SelectedObjectIndex];
			SelectedObject = dynamic_cast<UPrimitiveComponent*>(TempObject);
			FMatrix Mat = SelectedObject->GetModelMatrix();

			if (SelectedObject)
			{
				FMatrix model = SelectedObject->GetModelMatrix();
				FMatrix view = camera.GetViewMatrix();
				FMatrix projection = camera.GetProjectionMatrix(aspectRatio);

				ImGuizmo::SetOrthographic(false);
				ImGuizmo::SetRect(0.0f, 0.0f, renderer.GetWidth(), renderer.GetHeight());
				ImGuizmo::Manipulate(view.M[0], projection.M[0], TrsMode, WlMode, model.M[0], NULL, NULL);

				if (ImGuizmo::IsUsing())
				{
					float translation[3];
					float rotation[3];
					float scale[3];

					ImGuizmo::DecomposeMatrixToComponents(model.M[0], translation, rotation, scale);

					SelectedObject->RelativeLocation = FVector(translation[0], translation[1], translation[2]);
					SelectedObject->RelativeRotation = FVector(-rotation[0], -rotation[1], rotation[2]);
					SelectedObject->RelativeScale3D = FVector(scale[0], scale[1], scale[2]);
				}
			}
		}

		ImGui::Begin("Debug Camera");
		ImGui::DragFloat3("Translation", &camera.RelativeLocation.x, 0.1f);
		ImGui::DragFloat3("Rotation", &camera.RelativeRotation.x, 0.1f);
		ImGui::DragFloat("fovY", &camera.fovY, 0.1f);
		ImGui::Text("%d", pressed[6]);
		ImGui::End();

		ImGui::Begin("Place Actors");
		ImGui::Text("FPS %.0f (%.2f ms)", ImGui::GetIO().Framerate, 1000.0f / ImGui::GetIO().Framerate);
		ImGui::Separator();

		if (ImGui::BeginCombo("Primitive", PrimitiveTypeNames[SelectedPrimitiveIndex]))
		{
			for (int i = 0; i < std::size(PrimitiveTypeNames); i++)
			{
				bool bIsSelected = (SelectedPrimitiveIndex == i);
				if (ImGui::Selectable(PrimitiveTypeNames[i], bIsSelected))
				{
					SelectedPrimitiveIndex = i;
				}
				if (bIsSelected)
				{
					ImGui::SetItemDefaultFocus();
				}
			}
			ImGui::EndCombo();
		}

		if (ImGui::Button("Spawn", ImVec2(80, 0)))
		{
			for (int i = 0; i < SpawnCount; i++)
			{
				SpawnPrimitiveByType(SelectedPrimitiveIndex, renderer);
			}
		}

		ImGui::SameLine();
		ImGui::SetNextItemWidth(150);
		ImGui::SliderInt("Number of spawn", &SpawnCount, 1, 100);

		if (ImGui::Button("Delete", ImVec2(80, 0)))
		{
			if (SelectedObjectIndex >= 0 && SelectedObjectIndex < (int32)GUObjectArray.size())
			{
				delete GUObjectArray[SelectedObjectIndex];
				SelectedObjectIndex = -1;
			}
		}
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

	ImGui_ImplDX11_Shutdown();	//ImGui 리소스 해제
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();

	resourceManager.Release();

	// 렌더러 리소스 해제
	renderer.ReleaseConstantBuffer();
	renderer.ReleaseShader();
	renderer.Release();

	return 0;
}
