#pragma once

#include "Core.h"

class FInputContext
{
public:
	inline void HandleKeyDown(uint64 KeyCode) {
		KeyDownStates.set(KeyCode, true);
		KeyPressedStates.set(KeyCode, true);
	}

	inline void HandleKeyUp(uint64 KeyCode) {
		KeyUpStates.set(KeyCode, true);
		KeyPressedStates.set(KeyCode, false);
	}

	inline void HandleMouseButtonDown(uint8 ButtonIndex) {
		MouseButtonDownStates |= (1 << ButtonIndex);
		MouseButtonPressedStates |= (1 << ButtonIndex);
	}

	inline void HandleMouseButtonUp(uint8 ButtonIndex) {
		MouseButtonUpStates |= (1 << ButtonIndex);
		MouseButtonPressedStates &= ~(1 << ButtonIndex);
	}

	inline void HandleMouseMove(int32 X, int32 Y) {
		MouseX = X;
		MouseY = Y;
	}

	inline void Update() {
		KeyDownStates.reset();
		KeyUpStates.reset();

		MouseButtonDownStates = 0;
		MouseButtonUpStates = 0;
	}

	inline bool IsKeyDown(uint64 KeyCode) const {
		return KeyDownStates.test(KeyCode);
	}

	inline bool IsKeyUp(uint64 KeyCode) const {
		return KeyUpStates.test(KeyCode);
	}

	inline bool IsKeyPressed(uint64 KeyCode) const {
		return KeyPressedStates.test(KeyCode);
	}

	inline bool IsMouseButtonDown(uint8 ButtonIndex) const {
		return (MouseButtonDownStates & (1 << ButtonIndex)) != 0;
	}

	inline bool IsMouseButtonUp(uint8 ButtonIndex) const {
		return (MouseButtonUpStates & (1 << ButtonIndex)) != 0;
	}

	inline bool IsMouseButtonPressed(uint8 ButtonIndex) const {
		return (MouseButtonPressedStates & (1 << ButtonIndex)) != 0;
	}

	inline int32 GetMouseX() const {
		return MouseX;
	}

	inline int32 GetMouseY() const {
		return MouseY;
	}

private:
	FBitSet<256> KeyDownStates;
	FBitSet<256> KeyUpStates;
	FBitSet<256> KeyPressedStates;

	uint8 MouseButtonDownStates;
	uint8 MouseButtonUpStates;
	uint8 MouseButtonPressedStates;

	int32 MouseX, MouseY;
};