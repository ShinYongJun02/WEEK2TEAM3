#pragma once

#include "Core.h"

class UInputContext
{
public:
	inline void HandleKeyDown(uint64 keyCode) {
		KeyDownStates.set(keyCode, true);
		KeyPressedStates.set(keyCode, true);
	}

	inline void HandleKeyUp(uint64 keyCode) {
		KeyUpStates.set(keyCode, true);
		KeyPressedStates.set(keyCode, false);
	}

	inline void HandleMouseButtonDown(uint8 buttonIndex) {
		MouseButtonDownStates |= (1 << buttonIndex);
		MouseButtonPressedStates |= (1 << buttonIndex);
	}

	inline void HandleMouseButtonUp(uint8 buttonIndex) {
		MouseButtonUpStates |= (1 << buttonIndex);
		MouseButtonPressedStates &= ~(1 << buttonIndex);
	}

	inline void HandleMouseMove(int32 x, int32 y) {
		MouseX = x;
		MouseY = y;
	}

	inline void Update() {
		KeyDownStates.reset();
		KeyUpStates.reset();

		MouseButtonDownStates = 0;
		MouseButtonUpStates = 0;
	}

	inline bool IsKeyDown(uint64 keyCode) const {
		return KeyDownStates.test(keyCode);
	}

	inline bool IsKeyUp(uint64 keyCode) const {
		return KeyUpStates.test(keyCode);
	}

	inline bool IsKeyPressed(uint64 keyCode) const {
		return KeyPressedStates.test(keyCode);
	}

	inline bool IsMouseButtonDown(uint8 buttonIndex) const {
		return (MouseButtonDownStates & (1 << buttonIndex)) != 0;
	}

	inline bool IsMouseButtonUp(uint8 buttonIndex) const {
		return (MouseButtonUpStates & (1 << buttonIndex)) != 0;
	}

	inline bool IsMouseButtonPressed(uint8 buttonIndex) const {
		return (MouseButtonPressedStates & (1 << buttonIndex)) != 0;
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