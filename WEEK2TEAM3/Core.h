#pragma once

#include <windows.h>					
#include <d3d11.h>						
#include <d3dcompiler.h>			

#include "ImGui/imgui.h"				
#include "ImGui/imgui_internal.h"		
#include "ImGui/imgui_impl_dx11.h"		
#include "ImGui/imgui_impl_win32.h"		

#include <cmath>
#include <iostream>

struct FVector
{
	float x;
	float y;
	float z;

	FVector(float _x = 0, float _y = 0, float _z = 0) : x(_x), y(_y), z(_z) {}

	float Dot(const FVector& rhs) const
	{
		return x * rhs.x + y * rhs.y + z * rhs.z;
	}

	FVector Cross(const FVector& rhs) const
	{
		float _x = y * rhs.z - z * rhs.y;
		float _y = z * rhs.x - x * rhs.z;
		float _z = x * rhs.y - y * rhs.x;
		return FVector(_x, _y, _z);
	}

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
		if (length > 0.0f)
		{
			x /= length;
			y /= length;
			z /= length;
		}
	}

	FVector operator+(const FVector& rhs) const
	{
		return FVector(x + rhs.x, y + rhs.y, z + rhs.z);
	}

	FVector operator-(const FVector& rhs) const
	{
		return FVector(x - rhs.x, y - rhs.y, z - rhs.z);
	}

	FVector operator*(float scalar) const
	{
		return FVector(x * scalar, y * scalar, z * scalar);
	}

	FVector& operator+=(const FVector& rhs)
	{
		x += rhs.x;
		y += rhs.y;
		z += rhs.z;
		return *this;
	}

	FVector& operator-=(const FVector& rhs)
	{
		x -= rhs.x;
		y -= rhs.y;
		z -= rhs.z;
		return *this;
	}

	FVector& operator*=(float scalar)
	{
		x *= scalar;
		y *= scalar;
		z *= scalar;
		return *this;
	}
};

struct FVector4
{
	float x;
	float y;
	float z;
	float w;

	FVector4(float _x = 0, float _y = 0, float _z = 0, float _w = 0) : x(_x), y(_y), z(_z), w(_w) {}
	FVector4(FVector _xyz, float _w) : x(_xyz.x), y(_xyz.y), z(_xyz.z), w(_w) {}

	float Dot(const FVector4& rhs) const
	{
		return x * rhs.x + y * rhs.y + z * rhs.z + w * rhs.w;
	}

	float LengthSquared() const
	{
		return x * x + y * y + z * z + w * w;
	}

	float Length() const
	{
		return sqrt(LengthSquared());
	}

	float Length3Squared() const
	{
		return x * x + y * y + z * z;
	}

	float Length3() const
	{
		return sqrt(Length3Squared());
	}

	FVector4 operator+(const FVector4& rhs) const
	{
		return FVector4(x + rhs.x, y + rhs.y, z + rhs.z, w + rhs.w);
	}

	FVector4 operator-(const FVector4& rhs) const
	{
		return FVector4(x - rhs.x, y - rhs.y, z - rhs.z, w - rhs.w);
	}

	FVector4 operator*(float scalar) const
	{
		return FVector4(x * scalar, y * scalar, z * scalar, w * scalar);
	}

	FVector4& operator+=(const FVector4& rhs)
	{
		x += rhs.x;
		y += rhs.y;
		z += rhs.z;
		w += rhs.w;
		return *this;
	}

	FVector4& operator-=(const FVector4& rhs)
	{
		x -= rhs.x;
		y -= rhs.y;
		z -= rhs.z;
		w -= rhs.w;
		return *this;
	}

	FVector4& operator*=(float scalar)
	{
		x *= scalar;
		y *= scalar;
		z *= scalar;
		w *= scalar;
		return *this;
	}
};

struct FMatrix
{
	float M[4][4] = {};

	FMatrix() {}

	FMatrix(FVector4 _x, FVector4 _y, FVector4 _z, FVector4 _w)
	{
		M[0][0] = _x.x;
		M[0][1] = _x.y;
		M[0][2] = _x.z;
		M[0][3] = _x.w;

		M[1][0] = _y.x;
		M[1][1] = _y.y;
		M[1][2] = _y.z;
		M[1][3] = _y.w;

		M[2][0] = _z.x;
		M[2][1] = _z.y;
		M[2][2] = _z.z;
		M[2][3] = _z.w;

		M[3][0] = _w.x;
		M[3][1] = _w.y;
		M[3][2] = _w.z;
		M[3][3] = _w.w;
	}

	FMatrix operator*(const FMatrix& rhs) const
	{
		FMatrix result;
		for (int i = 0; i < 4; i++)
		{
			for (int j = 0; j < 4; j++)
			{
				for (int k = 0; k < 4; k++)
				{
					result.M[i][j] += M[i][k] * rhs.M[k][j];
				}
			}
		}
		return result;
	}

	FMatrix operator*(float scalar) const
	{
		FMatrix result;
		for (int i = 0; i < 4; i++)
		{
			for (int j = 0; j < 4; j++)
			{
				result.M[i][j] = M[i][j] * scalar;
			}
		}
		return result;
	}

	FMatrix GetTranspose() const
	{
		FMatrix result;
		for (int i = 0;i < 4;i++)
		{
			for (int j = 0;j < 4;j++)
			{
				result.M[i][j] = M[j][i];
			}
		}

		return result;
	}

	static FMatrix GetIdentity()
	{
		return FMatrix(
			FVector4(1.0f, 0.0f, 0.0f, 0.0f),
			FVector4(0.0f, 1.0f, 0.0f, 0.0f),
			FVector4(0.0f, 0.0f, 1.0f, 0.0f),
			FVector4(0.0f, 0.0f, 0.0f, 1.0f));
	}
};
