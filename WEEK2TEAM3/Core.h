#pragma once

#pragma comment(lib, "user32")			
#pragma comment(lib, "d3d11")			
#pragma comment(lib, "d3dcompiler")		

#include <windows.h>					
#include <d3d11.h>						
#include <d3dcompiler.h>			

#include "ImGui/imgui.h"				
#include "ImGui/imgui_internal.h"		
#include "ImGui/imgui_impl_dx11.h"		
#include "ImGui/imgui_impl_win32.h"		

struct FVector
{
	float x, y, z;
	FVector(float _x = 0, float _y = 0, float _z = 0) : x(_x), y(_y), z(_z) {}

	float LengthSquared() const
	{
		return x * x + y * y + z * z;
	}

	float Length() const
	{
		return sqrt(LengthSquared());
	}

	void Normalize()
	{
		float length = Length();
		if (length > 0)
		{
			x /= length;
			y /= length;
			z /= length;
		}
	}

	float DotProduct(const FVector& other)
	{
		return x * other.x + y * other.y + z * other.z;
	}

	FVector operator+(const FVector& other) const
	{
		return FVector(x + other.x, y + other.y, z + other.z);
	}

	FVector operator-(const FVector& other) const
	{
		return FVector(x - other.x, y - other.y, z - other.z);
	}

	FVector operator*(float scalar) const
	{
		return FVector(x * scalar, y * scalar, z * scalar);
	}

	FVector& operator+=(const FVector& other)
	{
		x += other.x;
		y += other.y;
		z += other.z;
		return *this;
	}

	FVector& operator-=(const FVector& other)
	{
		x -= other.x;
		y -= other.y;
		z -= other.z;
		return *this;
	}
};
