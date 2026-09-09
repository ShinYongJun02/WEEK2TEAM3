#include <windows.h>
#include <windowsx.h>

#include <cfloat>
#include <cstdio>
#include <cstring>
#include <iterator>

#include "ImGui/imgui.h"
#include "ImGui/imgui_impl_dx11.h"
#include "ImGui/imgui_impl_win32.h"
#include "ImGui/ImGuizmo.h"

#include "Core.h"
#include "FConsoleWindow.h"
#include "FGizmo.h"
#include "FInputContext.h"
#include "FLogger.h"
#include "FRenderer.h"
#include "FRenderPipeline.h"
#include "FSceneManager.h"
#include "FUObjectAllocator.h"
#include "FWindowEventHandler.h"
#include "Helper.h"
#include "UCamera.h"
#include "UCubeComp.h"
#include "UObject.h"
#include "UPlaneComp.h"
#include "UPrimitiveComponent.h"
#include "UResourceManager.h"
#include "USceneComponent.h"
#include "USphereComp.h"

static void CreateDebugConsole()
{
	// 1. Allocate a new console for the calling process
	if (AllocConsole()) {
		FILE* Fp;

		// 2. Redirect standard output (stdout) to the console
		freopen_s(&Fp, "CONOUT$", "w", stdout);
		// 3. Redirect standard error (stderr) to the console
		freopen_s(&Fp, "CONOUT$", "w", stderr);
		// 4. Redirect standard input (stdin) to the console
		freopen_s(&Fp, "CONIN$", "r", stdin);

		// 5. Clear the error state for each of the C++ standard streams
		std::clog.clear();
		std::cerr.clear();
		std::cout.clear();
		std::cin.clear();

		// Optional: Set a title for your debug window
		SetConsoleTitle(L"Debug Console");
	}
}

static UPrimitiveComponent* SpawnPrimitiveByType(int TypeIndex, UResourceManager& ResourceManager)
{
	UPrimitiveComponent* NewPrimitive = nullptr;

	switch (TypeIndex)
	{
	case 0:
	{
		UCubeComp* Cube = (UCubeComp*)FObjectFactory::ConstructObject(UCubeComp::StaticClass());
		Cube->Initialize(ResourceManager);
		NewPrimitive = Cube;
	}
	break;
	case 1:
	{
		USphereComp* Sphere = (USphereComp*)FObjectFactory::ConstructObject(USphereComp::StaticClass());
		Sphere->Initialize(ResourceManager);
		NewPrimitive = Sphere;
	}
	break;
	case 2:
	{
		UPlaneComp* Plane = (UPlaneComp*)FObjectFactory::ConstructObject(UPlaneComp::StaticClass());
		Plane->Initialize(ResourceManager);
		NewPrimitive = Plane;
	}
	break;
	}

	return NewPrimitive;
}

extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND WindowHandle, UINT Message, WPARAM WParam, LPARAM LParam);

LRESULT CALLBACK WndProc(HWND WindowHandle, UINT Message, WPARAM WParam, LPARAM LParam)
{
	FWindowEventHandler* EventHandler = reinterpret_cast<FWindowEventHandler*>(GetWindowLongPtrW(WindowHandle, GWLP_USERDATA));

	// ImGui 메시지 처리
	if (ImGui_ImplWin32_WndProcHandler(WindowHandle, Message, WParam, LParam))
	{
		return true;
	}

	switch (Message)
	{
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	case WM_SIZE:
		if (EventHandler)
		{
			UINT Width = LOWORD(LParam);
			UINT Height = HIWORD(LParam);
			EventHandler->HandleResize(Width, Height);
		}
		break;
	case WM_KEYDOWN:
		if (EventHandler)
		{
			EventHandler->HandleKeyDown(WParam);
		}
		break;
	case WM_KEYUP:
		if (EventHandler)
		{
			EventHandler->HandleKeyUp(WParam);
		}
		break;
	case WM_LBUTTONDOWN:
		if (EventHandler)
		{
			EventHandler->HandleMouseButtonDown(0);
		}
		break;
	case WM_LBUTTONUP:
		if (EventHandler)
		{
			EventHandler->HandleMouseButtonUp(0);
		}
		break;
	case WM_RBUTTONDOWN:
		if (EventHandler)
		{
			EventHandler->HandleMouseButtonDown(1);
		}
		break;
	case WM_RBUTTONUP:
		if (EventHandler)
		{
			EventHandler->HandleMouseButtonUp(1);
		}
		break;
	case WM_MOUSEMOVE:
		if (EventHandler)
		{
			int32 X = GET_X_LPARAM(LParam);
			int32 Y = GET_Y_LPARAM(LParam);
			EventHandler->HandleMouseMove(X, Y);
		}
		break;
	default:
		return DefWindowProc(WindowHandle, Message, WParam, LParam);
	}

	return 0;
}

int WINAPI WinMain(HINSTANCE Instance, HINSTANCE PrevInstance, LPSTR CmdLine, int ShowCmd)
{
#if _DEBUG
	CreateDebugConsole();
#endif

	WCHAR WindowClass[] = L"JungleWindowClass";
	WCHAR Title[] = L"Game Tech Lab";

	WNDCLASSW WndClass = { 0, WndProc, 0, 0, 0, 0, 0, 0, 0, WindowClass };
	RegisterClassW(&WndClass);

	HWND WindowHandle = CreateWindowExW(0, WindowClass, Title,
		WS_POPUP | WS_VISIBLE | WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT, 1920, 1080, nullptr, nullptr, Instance, nullptr);

	// 렌더러 초기화
	FRenderer Renderer;
	Renderer.Create(WindowHandle);

	FUObjectAllocator::Initialize(1024 * 1024 * 100); // 100MB

	UResourceManager ResourceManager;
	ResourceManager.Initialize(Renderer);

	FInputContext InputContext;

	FWindowEventHandler WindowEventHandler(Renderer, InputContext);

	SetWindowLongPtrW(WindowHandle, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(&WindowEventHandler));

	// ImGui 초기화
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui_ImplWin32_Init((void*)WindowHandle);
	ImGui_ImplDX11_Init(Renderer.Device, Renderer.DeviceContext);

	// FPS 관리
	LARGE_INTEGER Frequency;
	LARGE_INTEGER CurrentTime, LastTime;
	QueryPerformanceFrequency(&Frequency);
	QueryPerformanceCounter(&CurrentTime);
	LastTime = CurrentTime;
	double DeltaTime = 0.0f;

	// 카메라
	UPerspectiveCamera PerspectiveCamera;
	PerspectiveCamera.RelativeLocation = FVector(-5.0f, 0, 5.0f);
	PerspectiveCamera.RelativeRotation = FVector(0, -45.0f, 0);

	UOrthoCamera OrthoCamera;
	OrthoCamera.RelativeLocation += FVector(-5.0f);

	double CameraSpeed = 0.25f;

	// 마우스 추적
	int32 LastMouseX = InputContext.GetMouseX();
	int32 LastMouseY = InputContext.GetMouseY();

	//outliner
	int32 SelectedObjectIndex = -1;

	//Place Actor
	static const char* PrimitiveTypeNames[] = { "Cube", "Sphere", "Plane" };
	static int SelectedPrimitiveIndex = 0;
	static int SpawnCount = 1;

	// 종료 시그널
	bool bIsExit = false;

	bool bUsePerspectiveCamera = true;

	struct FOutlineConstant
	{
		FMatrix Model;
		FMatrix ViewProjection;
		FVector OutlineColor;
		float Padding[3];
	};

	TSharedPtr<FRenderPipeline> OutlinePipeline = Renderer.CreateRenderPipeline();
	OutlinePipeline->SetRasterRizerState(D3D11_CULL_FRONT);
	OutlinePipeline->SetDepthStencilState(true, true);
	OutlinePipeline->SetShader("Assets/Shaders/Outline.hlsl");
	OutlinePipeline->AddConstantBuffer<FOutlineConstant>();

	TSharedPtr<FRenderPipeline> PlaneOutlinePipeline = Renderer.CreateRenderPipeline();
	PlaneOutlinePipeline->SetRasterRizerState(D3D11_CULL_BACK, 1);
	PlaneOutlinePipeline->SetDepthStencilState(true, true);
	PlaneOutlinePipeline->SetShader("Assets/Shaders/Outline.hlsl");
	PlaneOutlinePipeline->AddConstantBuffer<FOutlineConstant>();

	FConsoleWindow ConsoleWindow;
	FGizmo Gizmo(Renderer, InputContext);

	bool bCurrentGizmoWorldMode = true;
	EGizmoOperation CurrentGizmoOperation = EGizmoOperation::Translate;

	UE_LOG(Test, Info, "Game Tech Lab Start!");
	UE_LOG(Test, Info, "This is long message This is long message This is long message This is long message This is long message This is long message This is long message This is long message This is long message This is long message");
	UE_LOG(Test, Warning, "This is a warning message.");
	UE_LOG(Test, Error, "This is an error message.");

	while (bIsExit == false)
	{
		QueryPerformanceCounter(&CurrentTime);
		DeltaTime = (double)(CurrentTime.QuadPart - LastTime.QuadPart) / (double)(Frequency.QuadPart);
		LastTime = CurrentTime;

		MSG Msg;

		while (PeekMessage(&Msg, nullptr, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&Msg);
			DispatchMessage(&Msg);

			if (Msg.message == WM_QUIT)
			{
				bIsExit = true;
				break;
			}
		}

		if (InputContext.IsKeyDown(VK_SPACE))
		{
			if (CurrentGizmoOperation == EGizmoOperation::Translate)
			{
				CurrentGizmoOperation = EGizmoOperation::Rotate;
			}
			else if (CurrentGizmoOperation == EGizmoOperation::Rotate)
			{
				CurrentGizmoOperation = EGizmoOperation::Scale;
			}
			else
			{
				CurrentGizmoOperation = EGizmoOperation::Translate;
			}
		}
		else if (InputContext.IsKeyDown('V'))
		{
			bCurrentGizmoWorldMode = true;
		}
		else if (InputContext.IsKeyDown('B'))
		{
			bCurrentGizmoWorldMode = false;
		}

		// 마우스 추적
		int32 MouseX = InputContext.GetMouseX();
		int32 MouseY = InputContext.GetMouseY();
		float DistX = MouseX - LastMouseX;
		float DistY = LastMouseY - MouseY;
		LastMouseX = MouseX;
		LastMouseY = MouseY;

		UCamera* Camera;
		if (bUsePerspectiveCamera)
		{
			Camera = &PerspectiveCamera;
		}
		else
		{
			Camera = &OrthoCamera;
		}

		float Aspect = (float)Renderer.GetWidth() / (float)Renderer.GetHeight();

		Camera->Aspect = Aspect;

		// 카메라 무빙 (카메라 z축 회전 = 오른쪽 보기, 카메라 y축 회전 = 아래 보기
		if (!ImGui::GetIO().WantCaptureKeyboard)
		{
			FVector Axis(0.f, 0.f, 0.f);
			if (InputContext.IsKeyPressed('W'))
			{
				Axis.X = 1.f;
			}
			else if (InputContext.IsKeyPressed('S'))
			{
				Axis.X = -1.f;
			}

			if (InputContext.IsKeyPressed('D'))
			{
				Axis.Y = 1.f;
			}
			else if (InputContext.IsKeyPressed('A'))
			{
				Axis.Y = -1.f;
			}

			if (InputContext.IsKeyPressed('E'))
			{
				Axis.Z = 1.f;
			}
			else if (InputContext.IsKeyPressed('Q'))
			{
				Axis.Z = -1.f;
			}

			if (Axis.LengthSquared())
			{
				Axis.Normalize();
				Camera->RelativeLocation += (Camera->GetForward() * Axis.X + Camera->GetRight() * Axis.Y + Camera->GetUp() * Axis.Z) * DeltaTime;
			}
		}

		if (InputContext.IsMouseButtonPressed(1))
		{
			Camera->RelativeRotation += FVector(0.0f, DistY, DistX) * CameraSpeed;
		}

		FMatrix View = Camera->GetViewMatrix();
		FMatrix Projection = Camera->GetProjectionMatrix();
		FMatrix ViewProjection = View * Projection;
		FMatrix InvViewProjection = ViewProjection.GetInverse();

		float MappedX = Remap(MouseX, 0, Renderer.GetWidth(), -1.f, 1.f);
		float MappedY = -Remap(MouseY, 0, Renderer.GetHeight(), -1.f, 1.f);

		FVector4 NdcPos(MappedX, MappedY, 1.0f, 1.0f);
		FVector4 WorldPos = NdcPos * InvViewProjection;
		WorldPos /= WorldPos.W;

		ImGui_ImplDX11_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();
		ImGuizmo::BeginFrame();

		if (InputContext.IsMouseButtonDown(0)) {
			if (!ImGui::GetIO().WantCaptureMouse)
			{
				// 기즈모 핸들 위에서 누른 클릭은 기즈모가 가져간다.
				// 핸들이 오브젝트 실루엣 밖으로 나와 있어도 피킹이 선택을 지우지 않도록,
				// 레이캐스트보다 기즈모에 우선권을 준다.
				const bool bClickedGizmoHandle = SelectedObjectIndex != -1 && Gizmo.IsMouseOverHandle();

				if (!bClickedGizmoHandle)
				{
					SelectedObjectIndex = -1;
					FRay Ray;
					Ray.Origin = Camera->RelativeLocation;
					Ray.Direction = FVector(WorldPos.X, WorldPos.Y, WorldPos.Z) - Camera->RelativeLocation;
					Ray.Direction.Normalize();

					float ClosestT = FLT_MAX;
					for (int Index = 0; Index < GUObjectArray.size(); Index++)
					{
						if (!GUObjectArray[Index]->IsA<UPrimitiveComponent>())
						{
							continue;
						}

						UPrimitiveComponent* SelectedObject = static_cast<UPrimitiveComponent*>(GUObjectArray[Index]);
						float T = SelectedObject->CheckIntersection(Ray);
						if (T >= 0.0f && T < ClosestT)
						{
							ClosestT = T;
							SelectedObjectIndex = SelectedObject->InternalIndex;
						}
					}
				}
			}
		}

		// Transform
		Renderer.Prepare();
		Renderer.UpdateViewConstant(ViewProjection);

		for (int32 Index = 0; Index < GUObjectArray.size(); Index++)
		{
			UObject* Obj = GUObjectArray[Index];

			if (Obj->IsA<UPrimitiveComponent>())
			{
				UPrimitiveComponent* Prim = static_cast<UPrimitiveComponent*>(Obj);
				if (Index == SelectedObjectIndex)
				{
					TSharedPtr<FRenderPipeline> Pipeline = OutlinePipeline;
					if (Prim->IsA<UPlaneComp>())
					{
						PlaneOutlinePipeline->UpdateConstantBuffer(0, FOutlineConstant{ Prim->GetModelMatrix(), ViewProjection, FVector(1.f, 1.f, 1.f), 0.03f });
						Pipeline = PlaneOutlinePipeline;
					}
					Pipeline->UpdateConstantBuffer(0, FOutlineConstant{ Prim->GetModelMatrix(), ViewProjection, FVector(1.f, 1.f, 1.f), 0.03f });
					Renderer.RenderPrimitive(Pipeline, Prim->GetStaticMesh()->VertexBuffer, Prim->GetStaticMesh()->VertexCount);
				}
				Prim->Render(Renderer);
			}
		}

		Renderer.RenderWorldAxis(View, Projection, FVector4(0.f, 0.f, 1.f, 1.f), Up, 2.0f);
		Renderer.RenderWorldGrid(ViewProjection, Camera->RelativeLocation);

		// ImGui
		ConsoleWindow.Draw("Debug Console", nullptr);

		ImGui::Begin("Outliner");

		for (int32 Index = 0; Index < GUObjectArray.size(); Index++)
		{
			if (!GUObjectArray[Index]->IsA<UPrimitiveComponent>())
			{
				continue;
			}

			UPrimitiveComponent* Prim = static_cast<UPrimitiveComponent*>(GUObjectArray[Index]);

			char Label[64];
			sprintf_s(Label, "%s : %s", Prim->GetName().c_str(), Prim->UUID.ToString().c_str());

			bool bIsSelected = (SelectedObjectIndex == Prim->InternalIndex);
			if (ImGui::Selectable(Label, bIsSelected))
			{
				SelectedObjectIndex = Prim->InternalIndex;
			}
		}
		ImGui::End();

		ImGui::Begin("Details Panel");

		if (SelectedObjectIndex != -1 && GUObjectArray[SelectedObjectIndex]->IsA<USceneComponent>())
		{
			USceneComponent* SelectedObject = static_cast<USceneComponent*>(GUObjectArray[SelectedObjectIndex]);
			ImGui::DragFloat3("Translation", &SelectedObject->RelativeLocation.X, 0.1f);
			ImGui::DragFloat3("Rotation", &SelectedObject->RelativeRotation.X, 0.1f);
			ImGui::DragFloat3("Scale", &SelectedObject->RelativeScale3D.X, 0.1f);
		}

		ImGui::End();

		// ImGuizmo
		if (SelectedObjectIndex != -1 && GUObjectArray[SelectedObjectIndex]->IsA<USceneComponent>())
		{
			USceneComponent* SelectedObject = static_cast<USceneComponent*>(GUObjectArray[SelectedObjectIndex]);
#if 0
			FMatrix Mat = SelectedObject->GetModelMatrix();

			if (SelectedObject)
			{
				FMatrix Model = SelectedObject->GetModelMatrix();

				ImGuizmo::SetOrthographic(false);
				ImGuizmo::SetRect(0.0f, 0.0f, Renderer.GetWidth(), Renderer.GetHeight());
				ImGuizmo::Manipulate(View.M[0], Projection.M[0], TrsMode, WlMode, Model.M[0], NULL, NULL);

				if (ImGuizmo::IsUsing())
				{
					float Translation[3];
					float Rotation[3];
					float Scale[3];

					ImGuizmo::DecomposeMatrixToComponents(Model.M[0], Translation, Rotation, Scale);

					SelectedObject->RelativeLocation = FVector(Translation[0], Translation[1], Translation[2]);
					SelectedObject->RelativeRotation = FVector(-Rotation[0], -Rotation[1], Rotation[2]);
					SelectedObject->RelativeScale3D = FVector(Scale[0], Scale[1], Scale[2]);
				}
			}
#else
			Gizmo.SetOperation(CurrentGizmoOperation);
			Gizmo.SetWorldMode(bCurrentGizmoWorldMode);
			Gizmo.Draw(*SelectedObject, Camera->RelativeLocation, ViewProjection);
#endif
		}

		ImGui::Begin("Place Actors");
		ImGui::Text("FPS %.0f (%.4f ms)", 1.0f / DeltaTime, DeltaTime);
		ImGui::Separator();

		if (ImGui::BeginCombo("Primitive", PrimitiveTypeNames[SelectedPrimitiveIndex]))
		{
			for (int Index = 0; Index < std::size(PrimitiveTypeNames); Index++)
			{
				bool bIsSelected = (SelectedPrimitiveIndex == Index);
				if (ImGui::Selectable(PrimitiveTypeNames[Index], bIsSelected))
				{
					SelectedPrimitiveIndex = Index;
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
			for (int Index = 0; Index < SpawnCount; Index++)
			{
				SpawnPrimitiveByType(SelectedPrimitiveIndex, ResourceManager);
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

		ImGui::Separator();

		static char Buf[256] = "";
		static FString SceneName = "";
		if (ImGui::InputText("Scene Name", Buf, IM_ARRAYSIZE(Buf), ImGuiInputTextFlags_EnterReturnsTrue))
		{
			SceneName = Buf;
		}

		if (ImGui::Button("New Scene", ImVec2(120, 0)))
		{
			if (!SceneName.empty())
			{
				std::memset(Buf, 0, sizeof(Buf));
				SceneName = Buf;
			}
			FSceneManager::ClearScene();
		}

		if (ImGui::Button("Save Scene", ImVec2(120, 0)))
		{
			if (!SceneName.empty())
			{
				FSceneManager::SaveScene(SceneName);
			}
		}

		ImGui::SameLine();

		if (ImGui::Button("Load Scene", ImVec2(120, 0)))
		{
			if (!SceneName.empty())
			{
				FSceneManager::LoadScene(ResourceManager, SceneName);
				SelectedObjectIndex = -1;
			}
		}

		ImGui::Separator();
		ImGui::DragFloat3("Translation", &Camera->RelativeLocation.X, 0.1f);
		ImGui::DragFloat3("Rotation", &Camera->RelativeRotation.X, 0.1f);

		if (ImGui::Button("Location", ImVec2(100, 0)))
		{
			CurrentGizmoOperation = EGizmoOperation::Translate;
		}

		ImGui::SameLine();

		if (ImGui::Button("Rotation", ImVec2(100, 0)))
		{
			CurrentGizmoOperation = EGizmoOperation::Rotate;
		}

		ImGui::SameLine();

		if (ImGui::Button("Scale", ImVec2(100, 0)))
		{
			CurrentGizmoOperation = EGizmoOperation::Scale;
		}

		if (ImGui::Button("World", ImVec2(100, 0)))
		{
			bCurrentGizmoWorldMode = true;
		}

		ImGui::SameLine();

		if (ImGui::Button("Local", ImVec2(100, 0)))
		{
			bCurrentGizmoWorldMode = false;
		}

		if (Camera->IsA<UPerspectiveCamera>())
		{
			ImGui::DragFloat("FovY", &PerspectiveCamera.FovY, 0.1f);
		}
		else
		{
			ImGui::DragFloat("HalfHeight", &OrthoCamera.HalfHeight, 0.1f);
		}

		if (ImGui::Checkbox("Use Perspective Camera", &bUsePerspectiveCamera))
		{
			if (bUsePerspectiveCamera)
			{
				PerspectiveCamera.RelativeLocation = Camera->RelativeLocation;
				PerspectiveCamera.RelativeRotation = Camera->RelativeRotation;
			}
			else
			{
				OrthoCamera.RelativeLocation = Camera->RelativeLocation;
				OrthoCamera.RelativeRotation = Camera->RelativeRotation;
			}
		}

		ImGui::Separator();
		ImGui::Text("Heap Size: %zu bytes", FUObjectAllocator::GetHeapSize());
		ImGui::Text("Total Allocation Bytes: %u bytes", FUObjectAllocator::GetTotalAllocationBytes());
		ImGui::Text("Total Allocation Count: %u", FUObjectAllocator::GetTotalAllocationCount());
		ImGui::End();

		ImGui::Render();
		ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

		// 그리기 명령 실행
		Renderer.SwapBuffer();

		InputContext.Update();
	}

	for (int32 Index = 0; Index < GUObjectArray.size(); Index++)
	{
		FObjectFactory::DestroyObject(GUObjectArray[Index]);
	}
	GUObjectArray.clear();

	PlaneOutlinePipeline->Release();
	OutlinePipeline->Release();

	ResourceManager.Release();

	FUObjectAllocator::Release();

	ImGui_ImplDX11_Shutdown();	//ImGui 리소스 해제
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();

	// 렌더러 리소스 해제
	Renderer.Release();

	return 0;
}
