#include "FWindowEventHandler.h"

#include "FInputContext.h"
#include "FRenderer.h"

FWindowEventHandler::FWindowEventHandler(FRenderer& InRenderer, FInputContext& InInputContext)
	: Renderer(InRenderer)
	, InputContext(InInputContext)
{
}

void FWindowEventHandler::HandleResize(UINT Width, UINT Height)
{
	Renderer.Resize(Width, Height);
}

void FWindowEventHandler::HandleKeyDown(uint64 KeyCode)
{
	InputContext.HandleKeyDown(KeyCode);
}

void FWindowEventHandler::HandleKeyUp(uint64 KeyCode)
{
	InputContext.HandleKeyUp(KeyCode);
}

void FWindowEventHandler::HandleMouseButtonDown(uint8 ButtonIndex)
{
	InputContext.HandleMouseButtonDown(ButtonIndex);
}

void FWindowEventHandler::HandleMouseButtonUp(uint8 ButtonIndex)
{
	InputContext.HandleMouseButtonUp(ButtonIndex);
}

void FWindowEventHandler::HandleMouseMove(int32 X, int32 Y)
{
	InputContext.HandleMouseMove(X, Y);
}
