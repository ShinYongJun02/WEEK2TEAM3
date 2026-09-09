#pragma once

#include <bitset>
#include <chrono>
#include <cstdlib>
#include <deque>
#include <iostream>
#include <memory>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>
#include "Types.h"

#define ASSERT(expr) if (!(expr)) { std::cerr << "Assertion failed: " << #expr << std::endl; std::abort(); }



template <typename T, typename... Args>
TSharedPtr<T> MakeShared(Args&&... Arguments)
{
	return std::make_shared<T>(std::forward<Args>(Arguments)...);
}


struct FVector2
{
	float X;
	float Y;

	FVector2(float InX = 0, float InY = 0);

	float LengthSquared() const;
	float Length() const;
	void Normalize();

	FVector2 operator+(const FVector2& Other) const;
	FVector2 operator-(const FVector2& Other) const;
	FVector2 operator*(float Scalar) const;
	FVector2& operator/=(float Scalar);
};

struct FVector
{
	float X;
	float Y;
	float Z;

	FVector(float InX = 0, float InY = 0, float InZ = 0);

	float Dot(const FVector& Rhs) const;
	FVector Cross(const FVector& Rhs) const;

	float LengthSquared() const;
	float Length() const;
	void Normalize();

	FVector operator+(const FVector& Rhs) const;
	FVector operator-(const FVector& Rhs) const;
	FVector operator-() const;
	FVector operator*(float Scalar) const;
	FVector& operator+=(const FVector& Rhs);
	FVector& operator-=(const FVector& Rhs);
	FVector& operator*=(float Scalar);
	FVector& operator*=(const FVector& Rhs);
	FVector& operator/=(float Scalar);
};

struct FVector4
{
	float X;
	float Y;
	float Z;
	float W;

	FVector4(float InX = 0, float InY = 0, float InZ = 0, float InW = 0);
	FVector4(FVector InXYZ, float InW);

	float Dot(const FVector4& Rhs) const;

	float LengthSquared() const;
	float Length() const;
	float Length3Squared() const;
	float Length3() const;

	FVector4 operator+(const FVector4& Rhs) const;
	FVector4 operator-(const FVector4& Rhs) const;
	FVector4 operator*(float Scalar) const;
	FVector4& operator+=(const FVector4& Rhs);
	FVector4& operator-=(const FVector4& Rhs);
	FVector4& operator*=(float Scalar);
	FVector4& operator/=(float Scalar);
};

struct FMatrix
{
	float M[4][4] = {};

	FMatrix();
	FMatrix(FVector4 InX, FVector4 InY, FVector4 InZ, FVector4 InW);

	FMatrix operator*(const FMatrix& Rhs) const;
	FMatrix operator*(float Scalar) const;

	FMatrix GetTranspose() const;
	FMatrix GetInverse() const;
	float* GetFloat16();

	static FMatrix GetIdentity();
};

struct FQuaternion
{
	union
	{
		struct
		{
			float X, Y, Z, W;
		};
		struct
		{
			FVector V;
			float S;
		};
		float Data[4];
	};

	FQuaternion();
	FQuaternion(float InX, float InY, float InZ, float InW);
	FQuaternion(FVector Axis, float Angle);

	void Normalize();
	FQuaternion Inverse() const;

	FQuaternion operator*(const FQuaternion& Other) const;
	float& operator[](int Index);
};

struct FRay
{
	FVector Origin;
	FVector Direction;

	FRay() = default;
	FRay(const FVector& InOrigin, const FVector& InDirection);

	static bool CheckAABB(FRay LocalRay, FVector MinVector, FVector MaxVector);
};

struct FTriangle
{
	FVector P0;
	FVector P1;
	FVector P2;

	FTriangle(const FVector& InP0, const FVector& InP1, const FVector& InP2);
};

float Dot(const FVector2& A, const FVector2& B);
float Dot(const FVector& A, const FVector& B);
FVector Cross(const FVector& A, const FVector& B);

FVector4 operator*(const FVector4& Vec, const FMatrix& Mat);

extern const FVector Front;
extern const FVector Right;
extern const FVector Up;
