#pragma once

#include <windows.h>

#include "Core.h"

class FInputContext;
class FRenderer;

class FWindowEventHandler
{
public:
	FWindowEventHandler(FRenderer& InRenderer, FInputContext& InInputContext);

	void HandleResize(UINT Width, UINT Height);

	void HandleKeyDown(uint64 KeyCode);
	void HandleKeyUp(uint64 KeyCode);

	void HandleMouseButtonDown(uint8 ButtonIndex);
	void HandleMouseButtonUp(uint8 ButtonIndex);
	void HandleMouseMove(int32 X, int32 Y);

private:
	FRenderer& Renderer;
	FInputContext& InputContext;
};
