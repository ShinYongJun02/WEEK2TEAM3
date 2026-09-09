# WEEK2 TEAM3

DirectX 11 기반의 미니 3D 씬 에디터입니다. 언리얼 엔진의 오브젝트 모델(`UObject` / `USceneComponent` / `UPrimitiveComponent`)과
좌표계 규약(X=Front, Y=Right, Z=Up)을 흉내 낸 C++20 Win32 애플리케이션으로, 프리미티브를 배치·선택·조작하고
씬을 파일로 저장/불러올 수 있습니다.

---

## 주요 기능

### 렌더링
- **DirectX 11 파이프라인**: 스왑체인, 프레임버퍼, 깊이/스텐실 버퍼를 직접 구성
- **FRenderPipeline**: 래스터라이저 / 깊이스텐실 / 블렌드 상태, 셰이더, 상수 버퍼를 하나로 묶은 파이프라인 객체 (런타임 HLSL 컴파일)
- **2D 헬퍼 렌더링**: 화면 공간 선(`RenderLine2D`), 원(`RenderCircle2D`), 삼각형(`RenderTriangle2D`)
- **월드 보조 표시**: 월드 축(`RenderWorldAxis`), 무한 그리드(`RenderWorldGrid`)
- **아웃라인**: 선택된 오브젝트를 front-face culling 기반 외곽선 셰이더로 강조 (평면은 depth-bias 전용 파이프라인 사용)

### 오브젝트 시스템 (언리얼 스타일)
- **`UObject` + `FClass` 리플렉션**: `GENERATED_BODY` 매크로가 타입 이름, 부모 타입, 생성 함수를 담은 `FClass`를 만들고
  전역 레지스트리에 자동 등록합니다. `IsA<T>()`로 런타임 타입 검사, `FindClass("UCubeComp")`로 이름 기반 생성이 가능합니다.
- **`FObjectFactory`**: 모든 오브젝트 생성/파괴 창구. 생성 시 UUID 발급 및 `GUObjectArray` 등록, 소멸 시 swap-remove로 제거
- **`FUObjectAllocator`**: 100MB 메모리 풀 위에서 동작하는 커스텀 얼로케이터. `UObject::operator new/delete`가 이를 사용하며,
  정렬(alignment) 패딩 헤더, free-list 관리, 인접 블록 병합(coalescing)을 직접 구현. 힙 사용량이 UI에 실시간 표시됩니다.
- **`FUUID`**: 128비트(32bit × 4) 식별자. 씬 저장/로드 시 오브젝트 동일성을 보존합니다.

### 씬 편집
- **Place Actors**: Cube / Sphere / Plane 스폰 (한 번에 1~100개), 선택 오브젝트 삭제
- **Outliner**: 씬의 모든 프리미티브를 `이름 : UUID` 형태로 나열, 클릭 선택
- **Details Panel**: 선택 오브젝트의 Translation / Rotation / Scale 편집
- **마우스 피킹**: 화면 좌표 → NDC → 월드 레이 변환 후, 로컬 공간 AABB 사전 검사 → 삼각형 단위 레이캐스트로 최근접 오브젝트 선택
- **기즈모(`FGizmo`)**: Translate / Rotate / Scale 3가지 모드, World / Local 좌표계 전환.
  스크린 공간 선분과 마우스 거리로 핸들 히트 판정을 하며, 기즈모 핸들 클릭이 피킹보다 우선권을 가집니다.
- **카메라**: 원근(`UPerspectiveCamera`, FovY 조절) / 직교(`UOrthoCamera`, HalfHeight 조절) 전환.
  UE 좌표계를 DX 좌표계로 바꾸는 축 스왑 행렬을 뷰 행렬에 포함합니다.

### 씬 직렬화
- **`FSceneManager`**: `<SceneName>.Scene` 파일로 저장/로드. 버전(`SceneVersion = 1`)과 `NextUUID`를 함께 기록하여
  다시 불러와도 UUID가 이어집니다.
- **`FJsonParser`**: 외부 라이브러리 없이 직접 작성한 재귀 하강 JSON 파서. 문법 검증만 담당하고, 의미 검증(버전/타입 확인)은
  `FSceneManager`가 수행합니다.
- 로드 시 `Type` 문자열 → `FindClass` → `FObjectFactory::ConstructObject`로 오브젝트를 복원합니다.

### 디버깅 / 로깅
- **`UE_LOG(Category, Level, Fmt, ...)`** 매크로와 `FLogger` (Info / Warning / Error)
- **`FConsoleWindow`**: ImGui 기반 인게임 콘솔. 레벨 필터, 텍스트 필터, 오토스크롤, 클립보드 복사 지원
- Debug 빌드에서는 별도의 Win32 콘솔 창을 할당해 stdout/stderr을 리다이렉트

---

## 프로젝트 구조

```
WEEK2TEAM3/
├── premake5.lua              # 워크스페이스 정의 (x64, Debug/Release)
├── WEEK2TEAM3.sln            # premake가 생성한 VS 솔루션
├── Assets/Shaders/           # HLSL 셰이더
│   ├── ShaderW0.hlsl         # 기본 메시 셰이더
│   ├── Outline.hlsl          # 선택 오브젝트 외곽선
│   ├── Line2D / Circle2D / Triangle2D.hlsl   # 화면 공간 프리미티브
│   ├── WorldAxis.hlsl        # 월드 축
│   └── WorldGrid.hlsl        # 월드 그리드
├── Sample.Scene              # 씬 파일 예시
└── WEEK2TEAM3/
    ├── main.cpp              # WinMain, 메시지 루프, 에디터 UI 전체
    ├── Core.h / Core.cpp     # FVector2/3/4, FMatrix, FQuaternion, FRay, FTriangle
    ├── Types.h               # TArray/TMap/TSet/FString 등 UE 스타일 타입 별칭
    ├── Helper.h / .cpp       # 수학 유틸 (행렬 생성, 레이캐스트, 좌표 변환 등)
    │
    ├── UObject.h / .cpp              # UObject, FClass, 클래스 레지스트리, FObjectFactory
    ├── FUObjectAllocator.h / .cpp    # 메모리 풀 얼로케이터
    ├── FUUID.h / .cpp                # UUID
    ├── FEngineStatics.h / .cpp       # 전역 UUID 카운터
    ├── TSparseArray.h                # free-list 기반 스파스 배열
    │
    ├── USceneComponent.h / .cpp      # Transform (Location/Rotation/Scale, 행렬 생성)
    ├── UPrimitiveComponent.h / .cpp  # 렌더링 가능한 컴포넌트 (추상)
    ├── UCubeComp / USphereComp / UPlaneComp    # 구체 프리미티브 + 교차 판정
    ├── UCamera.h / .cpp              # UCamera, UPerspectiveCamera, UOrthoCamera
    ├── UResourceManager.h / .cpp     # 정적 메시(버텍스 버퍼) 캐시
    ├── FVertex.h / .cpp              # 큐브(36) / 스피어(2400) / 플레인(6) 버텍스 데이터
    │
    ├── FRenderer.h / .cpp            # D3D11 디바이스, 스왑체인, 드로우 호출
    ├── FRenderPipeline.h / .cpp      # 파이프라인 상태 + 셰이더 + 상수 버퍼
    ├── FGizmo.h / .cpp               # 트랜스폼 기즈모
    │
    ├── FInputContext.h / .cpp        # 키/마우스 상태 (Down/Up/Pressed)
    ├── FWindowEventHandler.h / .cpp  # WndProc 이벤트를 렌더러/입력으로 라우팅
    ├── FSceneManager.h / .cpp        # 씬 저장/로드/클리어
    ├── FJsonParser.h / .cpp          # JSON 파서
    ├── FLogger.h / .cpp              # 로깅
    ├── FConsoleWindow.h / .cpp       # ImGui 콘솔 창
    └── ImGui/                        # Dear ImGui (Win32 + DX11 백엔드)
```

---

## 아키텍처

### 클래스 계층

```
UObject
├── USceneComponent            (Transform)
│   ├── UPrimitiveComponent    (StaticMesh + Render + CheckIntersection)
│   │   ├── UCubeComp
│   │   ├── USphereComp
│   │   └── UPlaneComp
│   └── UCamera
│       ├── UPerspectiveCamera
│       └── UOrthoCamera
└── UResourceManager
```

### 메인 루프 (`main.cpp`)

```
1. DeltaTime 측정 (QueryPerformanceCounter)
2. Win32 메시지 펌프 → FWindowEventHandler → FInputContext
3. 입력 처리
   - Space: 기즈모 모드 순환 (Translate → Rotate → Scale)
   - V / B: World / Local 좌표계 전환
   - WASD / QE: 카메라 이동, 우클릭 드래그: 카메라 회전
4. View / Projection / InvViewProjection 계산, 마우스 → 월드 레이 생성
5. ImGui NewFrame
6. 좌클릭 피킹 (기즈모 핸들 우선)
7. 렌더링: 아웃라인 → 프리미티브 → 월드 축 → 그리드
8. ImGui 패널: Console / Outliner / Details / Place Actors, 기즈모 Draw
9. SwapBuffer, InputContext.Update()
```

### 좌표계

언리얼 규약을 따릅니다.

| 축 | 방향 | 회전 |
|---|---|---|
| X | Front | Roll |
| Y | Right | Pitch |
| Z | Up | Yaw |

모델 행렬은 `Scale * Rotation * Translation` (행 벡터 기준), 렌더링 직전 `UCamera::GetUEtoDXAxisSwap()`으로
DX 좌표계(X=Right, Y=Up, Z=Front)로 변환합니다.

### 씬 파일 포맷

```json
{
	"Version" : 1,
	"NextUUID" : "854622255-57208259-3912481696-3415713608",
	"Primitives" : {
		"<UUID>" : {
			"Location" : [0, 0, 0],
			"Rotation" : [0, 0, 0],
			"Scale" : [1, 1, 1],
			"Type" : "UCubeComp"
		}
	}
}
```

---

## 조작법

| 입력 | 동작 |
|---|---|
| `W` / `S` | 카메라 전진 / 후진 |
| `A` / `D` | 카메라 좌 / 우 이동 |
| `Q` / `E` | 카메라 하강 / 상승 |
| 마우스 우클릭 드래그 | 카메라 회전 |
| 마우스 좌클릭 | 오브젝트 선택 / 기즈모 핸들 조작 |
| `Space` | 기즈모 모드 순환 (Translate → Rotate → Scale) |
| `V` | 기즈모 World 좌표계 |
| `B` | 기즈모 Local 좌표계 |

### UI 패널

- **Debug Console** — 로그 출력, 레벨/텍스트 필터, 복사
- **Outliner** — 씬 오브젝트 목록 및 선택
- **Details Panel** — 선택 오브젝트의 Transform 편집
- **Place Actors** — FPS, 프리미티브 스폰/삭제, 씬 New/Save/Load, 카메라 설정, 기즈모 모드 버튼, 메모리 통계

---

그 외 수학, JSON 파싱, 메모리 할당은 모두 직접 구현했습니다.
