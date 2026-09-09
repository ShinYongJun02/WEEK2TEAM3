#pragma once

#include "Core.h"

class FInputContext
{
public:
	void HandleKeyDown(uint64 KeyCode);
	void HandleKeyUp(uint64 KeyCode);
	void HandleMouseButtonDown(uint8 ButtonIndex);
	void HandleMouseButtonUp(uint8 ButtonIndex);
	void HandleMouseMove(int32 X, int32 Y);

	void Update();

	bool IsKeyDown(uint64 KeyCode) const;
	bool IsKeyUp(uint64 KeyCode) const;
	bool IsKeyPressed(uint64 KeyCode) const;

	bool IsMouseButtonDown(uint8 ButtonIndex) const;
	bool IsMouseButtonUp(uint8 ButtonIndex) const;
	bool IsMouseButtonPressed(uint8 ButtonIndex) const;

	int32 GetMouseX() const;
	int32 GetMouseY() const;

private:
	FBitSet<256> KeyDownStates;
	FBitSet<256> KeyUpStates;
	FBitSet<256> KeyPressedStates;

	uint8 MouseButtonDownStates;
	uint8 MouseButtonUpStates;
	uint8 MouseButtonPressedStates;

	int32 MouseX, MouseY;
};
