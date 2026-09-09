#include "FInputContext.h"

void FInputContext::HandleKeyDown(uint64 KeyCode)
{
	KeyDownStates.set(KeyCode, true);
	KeyPressedStates.set(KeyCode, true);
}

void FInputContext::HandleKeyUp(uint64 KeyCode)
{
	KeyUpStates.set(KeyCode, true);
	KeyPressedStates.set(KeyCode, false);
}

void FInputContext::HandleMouseButtonDown(uint8 ButtonIndex)
{
	MouseButtonDownStates |= (1 << ButtonIndex);
	MouseButtonPressedStates |= (1 << ButtonIndex);
}

void FInputContext::HandleMouseButtonUp(uint8 ButtonIndex)
{
	MouseButtonUpStates |= (1 << ButtonIndex);
	MouseButtonPressedStates &= ~(1 << ButtonIndex);
}

void FInputContext::HandleMouseMove(int32 X, int32 Y)
{
	MouseX = X;
	MouseY = Y;
}

void FInputContext::Update()
{
	KeyDownStates.reset();
	KeyUpStates.reset();

	MouseButtonDownStates = 0;
	MouseButtonUpStates = 0;
}

bool FInputContext::IsKeyDown(uint64 KeyCode) const
{
	return KeyDownStates.test(KeyCode);
}

bool FInputContext::IsKeyUp(uint64 KeyCode) const
{
	return KeyUpStates.test(KeyCode);
}

bool FInputContext::IsKeyPressed(uint64 KeyCode) const
{
	return KeyPressedStates.test(KeyCode);
}

bool FInputContext::IsMouseButtonDown(uint8 ButtonIndex) const
{
	return (MouseButtonDownStates & (1 << ButtonIndex)) != 0;
}

bool FInputContext::IsMouseButtonUp(uint8 ButtonIndex) const
{
	return (MouseButtonUpStates & (1 << ButtonIndex)) != 0;
}

bool FInputContext::IsMouseButtonPressed(uint8 ButtonIndex) const
{
	return (MouseButtonPressedStates & (1 << ButtonIndex)) != 0;
}

int32 FInputContext::GetMouseX() const
{
	return MouseX;
}

int32 FInputContext::GetMouseY() const
{
	return MouseY;
}
