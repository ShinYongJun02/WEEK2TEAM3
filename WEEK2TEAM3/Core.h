#pragma once

#include <windows.h>
#include <windowsx.h>
#include <d3d11.h>						
#include <d3dcompiler.h>			

#include "ImGui/imgui.h"				
#include "ImGui/imgui_internal.h"		
#include "ImGui/imgui_impl_dx11.h"		
#include "ImGui/imgui_impl_win32.h"		
#include "ImGui/ImGuizmo.h"

#include <cmath>
#include <iostream>
#include <vector>
#include <map>
#include <unordered_map>
#include <bitset>

#define ASSERT(expr) if (!(expr)) { std::cerr << "Assertion failed: " << #expr << std::endl; std::abort(); }

typedef char int8;
typedef unsigned char uint8;
typedef int int32;
typedef unsigned int uint32;
typedef unsigned long long uint64;

template <typename T>
using TArray = std::vector<T>;

template <typename T>
using TSharedPtr = std::shared_ptr<T>;

template <typename T, typename... Args>
TSharedPtr<T> MakeShared(Args&&... args)
{
	return std::make_shared<T>(std::forward<Args>(args)...);
}

using FString = std::string;

template <typename TKey, typename TValue>
using TMap = std::unordered_map<TKey, TValue>;

template <size_t N>
using FBitSet = std::bitset<N>;

struct FPoint
{
	float x;
	float y;
};

struct FVector2
{
	float x;
	float y;

	FVector2(float _x = 0, float _y = 0) : x(_x), y(_y) {}
};

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

	FVector& operator*=(const FVector& rhs)
	{
		x *= rhs.x;
		y *= rhs.y;
		z *= rhs.z;
		return *this;
	}
};

inline static float Dot(const FVector& a, const FVector& b)
{
	return a.Dot(b);
}

inline static FVector Cross(const FVector& a, const FVector& b)
{
	return a.Cross(b);
}

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

	FVector4& operator/=(float scalar)
	{
		x /= scalar;
		y /= scalar;
		z /= scalar;
		w /= scalar;
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

	FMatrix GetInverse() const
	{
		// Left side: original matrix M, Right side: identity matrix
		float temp[4][8];
		for (int32 i = 0; i < 4; i++)
		{
			for (int32 j = 0; j < 4; j++)
			{
				temp[i][j] = M[i][j];
				temp[i][j + 4] = (i == j) ? 1.0f : 0.0f;
			}
		}

		for (int32 col = 0; col < 4; col++)
		{
			// Find the pivot row
			int32 pivot = col;
			for (int32 row = col + 1; row < 4; row++)
			{
				if (abs(temp[row][col]) > abs(temp[pivot][col]))
				{
					pivot = row;
				}
			}

			if (abs(temp[pivot][col]) < 1e-6f)
			{
				// Matrix is singular, cannot invert
				return GetIdentity();
			}

			for (int32 j = 0; j < 8; j++)
			{
				std::swap(temp[col][j], temp[pivot][j]);
			}

			// Normalize the pivot row
			float divisio = temp[col][col];
			for (int32 j = 0; j < 8; j++)
			{
				temp[col][j] /= divisio;
			}

			// Eliminate the current column in other rows
			for (int32 row = 0; row < 4; row++)
			{
				if (row == col)
				{
					continue;
				}

				float factor = temp[row][col];
				for (int32 j = 0; j < 8; j++)
				{
					temp[row][j] -= factor * temp[col][j];
				}
			}
		}

		return FMatrix(
			FVector4(temp[0][4], temp[0][5], temp[0][6], temp[0][7]),
			FVector4(temp[1][4], temp[1][5], temp[1][6], temp[1][7]),
			FVector4(temp[2][4], temp[2][5], temp[2][6], temp[2][7]),
			FVector4(temp[3][4], temp[3][5], temp[3][6], temp[3][7])
		);
	}

	static FMatrix GetIdentity()
	{
		return FMatrix(
			FVector4(1.0f, 0.0f, 0.0f, 0.0f),
			FVector4(0.0f, 1.0f, 0.0f, 0.0f),
			FVector4(0.0f, 0.0f, 1.0f, 0.0f),
			FVector4(0.0f, 0.0f, 0.0f, 1.0f));
	}

	float* GetFloat16()
	{

		return (&M[0][0]);
	}
};

inline static FVector4 operator*(const FVector4& vec, const FMatrix& mat)
{
	FVector4 result;
	result.x = vec.x * mat.M[0][0] + vec.y * mat.M[1][0] + vec.z * mat.M[2][0] + vec.w * mat.M[3][0];
	result.y = vec.x * mat.M[0][1] + vec.y * mat.M[1][1] + vec.z * mat.M[2][1] + vec.w * mat.M[3][1];
	result.z = vec.x * mat.M[0][2] + vec.y * mat.M[1][2] + vec.z * mat.M[2][2] + vec.w * mat.M[3][2];
	result.w = vec.x * mat.M[0][3] + vec.y * mat.M[1][3] + vec.z * mat.M[2][3] + vec.w * mat.M[3][3];
	return result;
}

struct FRay
{
	FVector Origin;
	FVector Direction;

	FRay() = default;

	FRay(const FVector& InOrigin, const FVector& InDirection)
		: Origin(InOrigin)
		, Direction(InDirection)
	{
	}
};

struct FTriangle
{
	FVector P0;
	FVector P1;
	FVector P2;

	FTriangle(const FVector& InP0, const FVector& InP1, const FVector& InP2)
		: P0(InP0)
		, P1(InP1)
		, P2(InP2)
	{
	}
};

static const FVector Front = FVector(1.0f, 0.0f, 0.0f);

struct FVertexSimple
{
	float x, y, z;    // Position
	float r, g, b, a; // Color
};

