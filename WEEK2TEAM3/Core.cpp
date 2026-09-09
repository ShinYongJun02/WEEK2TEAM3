#include "Core.h"

#include <cmath>

FVector2::FVector2(float InX, float InY)
	: X(InX)
	, Y(InY)
{
}

float FVector2::LengthSquared() const
{
	return X * X + Y * Y;
}

float FVector2::Length() const
{
	return sqrt(LengthSquared());
}

void FVector2::Normalize()
{
	float VectorLength = Length();
	if (VectorLength > 0.0f)
	{
		X /= VectorLength;
		Y /= VectorLength;
	}
}

FVector2 FVector2::operator+(const FVector2& Other) const
{
	return FVector2(X + Other.X, Y + Other.Y);
}

FVector2 FVector2::operator-(const FVector2& Other) const
{
	return FVector2(X - Other.X, Y - Other.Y);
}

FVector2 FVector2::operator*(float Scalar) const
{
	return FVector2(X * Scalar, Y * Scalar);
}

FVector2& FVector2::operator/=(float Scalar)
{
	X /= Scalar;
	Y /= Scalar;
	return *this;
}

FVector::FVector(float InX, float InY, float InZ)
	: X(InX)
	, Y(InY)
	, Z(InZ)
{
}

float FVector::Dot(const FVector& Rhs) const
{
	return X * Rhs.X + Y * Rhs.Y + Z * Rhs.Z;
}

FVector FVector::Cross(const FVector& Rhs) const
{
	float ResultX = Y * Rhs.Z - Z * Rhs.Y;
	float ResultY = Z * Rhs.X - X * Rhs.Z;
	float ResultZ = X * Rhs.Y - Y * Rhs.X;
	return FVector(ResultX, ResultY, ResultZ);
}

float FVector::LengthSquared() const
{
	return X * X + Y * Y + Z * Z;
}

float FVector::Length() const
{
	return sqrt(LengthSquared());
}

void FVector::Normalize()
{
	float VectorLength = Length();
	if (VectorLength > 0.0f)
	{
		X /= VectorLength;
		Y /= VectorLength;
		Z /= VectorLength;
	}
}

FVector FVector::operator+(const FVector& Rhs) const
{
	return FVector(X + Rhs.X, Y + Rhs.Y, Z + Rhs.Z);
}

FVector FVector::operator-(const FVector& Rhs) const
{
	return FVector(X - Rhs.X, Y - Rhs.Y, Z - Rhs.Z);
}

FVector FVector::operator-() const
{
	return FVector(-X, -Y, -Z);
}

FVector FVector::operator*(float Scalar) const
{
	return FVector(X * Scalar, Y * Scalar, Z * Scalar);
}

FVector& FVector::operator+=(const FVector& Rhs)
{
	X += Rhs.X;
	Y += Rhs.Y;
	Z += Rhs.Z;
	return *this;
}

FVector& FVector::operator-=(const FVector& Rhs)
{
	X -= Rhs.X;
	Y -= Rhs.Y;
	Z -= Rhs.Z;
	return *this;
}

FVector& FVector::operator*=(float Scalar)
{
	X *= Scalar;
	Y *= Scalar;
	Z *= Scalar;
	return *this;
}

FVector& FVector::operator*=(const FVector& Rhs)
{
	X *= Rhs.X;
	Y *= Rhs.Y;
	Z *= Rhs.Z;
	return *this;
}

FVector& FVector::operator/=(float Scalar)
{
	X /= Scalar;
	Y /= Scalar;
	Z /= Scalar;
	return *this;
}

FVector4::FVector4(float InX, float InY, float InZ, float InW)
	: X(InX)
	, Y(InY)
	, Z(InZ)
	, W(InW)
{
}

FVector4::FVector4(FVector InXYZ, float InW)
	: X(InXYZ.X)
	, Y(InXYZ.Y)
	, Z(InXYZ.Z)
	, W(InW)
{
}

float FVector4::Dot(const FVector4& Rhs) const
{
	return X * Rhs.X + Y * Rhs.Y + Z * Rhs.Z + W * Rhs.W;
}

float FVector4::LengthSquared() const
{
	return X * X + Y * Y + Z * Z + W * W;
}

float FVector4::Length() const
{
	return sqrt(LengthSquared());
}

float FVector4::Length3Squared() const
{
	return X * X + Y * Y + Z * Z;
}

float FVector4::Length3() const
{
	return sqrt(Length3Squared());
}

FVector4 FVector4::operator+(const FVector4& Rhs) const
{
	return FVector4(X + Rhs.X, Y + Rhs.Y, Z + Rhs.Z, W + Rhs.W);
}

FVector4 FVector4::operator-(const FVector4& Rhs) const
{
	return FVector4(X - Rhs.X, Y - Rhs.Y, Z - Rhs.Z, W - Rhs.W);
}

FVector4 FVector4::operator*(float Scalar) const
{
	return FVector4(X * Scalar, Y * Scalar, Z * Scalar, W * Scalar);
}

FVector4& FVector4::operator+=(const FVector4& Rhs)
{
	X += Rhs.X;
	Y += Rhs.Y;
	Z += Rhs.Z;
	W += Rhs.W;
	return *this;
}

FVector4& FVector4::operator-=(const FVector4& Rhs)
{
	X -= Rhs.X;
	Y -= Rhs.Y;
	Z -= Rhs.Z;
	W -= Rhs.W;
	return *this;
}

FVector4& FVector4::operator*=(float Scalar)
{
	X *= Scalar;
	Y *= Scalar;
	Z *= Scalar;
	W *= Scalar;
	return *this;
}

FVector4& FVector4::operator/=(float Scalar)
{
	X /= Scalar;
	Y /= Scalar;
	Z /= Scalar;
	W /= Scalar;
	return *this;
}

FMatrix::FMatrix()
{
}

FMatrix::FMatrix(FVector4 InX, FVector4 InY, FVector4 InZ, FVector4 InW)
{
	M[0][0] = InX.X;
	M[0][1] = InX.Y;
	M[0][2] = InX.Z;
	M[0][3] = InX.W;

	M[1][0] = InY.X;
	M[1][1] = InY.Y;
	M[1][2] = InY.Z;
	M[1][3] = InY.W;

	M[2][0] = InZ.X;
	M[2][1] = InZ.Y;
	M[2][2] = InZ.Z;
	M[2][3] = InZ.W;

	M[3][0] = InW.X;
	M[3][1] = InW.Y;
	M[3][2] = InW.Z;
	M[3][3] = InW.W;
}

FMatrix FMatrix::operator*(const FMatrix& Rhs) const
{
	FMatrix Result;
	for (int RowIndex = 0; RowIndex < 4; RowIndex++)
	{
		for (int ColIndex = 0; ColIndex < 4; ColIndex++)
		{
			for (int InnerIndex = 0; InnerIndex < 4; InnerIndex++)
			{
				Result.M[RowIndex][ColIndex] += M[RowIndex][InnerIndex] * Rhs.M[InnerIndex][ColIndex];
			}
		}
	}
	return Result;
}

FMatrix FMatrix::operator*(float Scalar) const
{
	FMatrix Result;
	for (int RowIndex = 0; RowIndex < 4; RowIndex++)
	{
		for (int ColIndex = 0; ColIndex < 4; ColIndex++)
		{
			Result.M[RowIndex][ColIndex] = M[RowIndex][ColIndex] * Scalar;
		}
	}
	return Result;
}

FMatrix FMatrix::GetTranspose() const
{
	FMatrix Result;
	for (int RowIndex = 0; RowIndex < 4; RowIndex++)
	{
		for (int ColIndex = 0; ColIndex < 4; ColIndex++)
		{
			Result.M[RowIndex][ColIndex] = M[ColIndex][RowIndex];
		}
	}

	return Result;
}

FMatrix FMatrix::GetInverse() const
{
	// Left side: original matrix M, Right side: identity matrix
	float Temp[4][8];
	for (int32 RowIndex = 0; RowIndex < 4; RowIndex++)
	{
		for (int32 ColIndex = 0; ColIndex < 4; ColIndex++)
		{
			Temp[RowIndex][ColIndex] = M[RowIndex][ColIndex];
			Temp[RowIndex][ColIndex + 4] = (RowIndex == ColIndex) ? 1.0f : 0.0f;
		}
	}

	for (int32 Col = 0; Col < 4; Col++)
	{
		// Find the pivot row
		int32 Pivot = Col;
		for (int32 Row = Col + 1; Row < 4; Row++)
		{
			if (abs(Temp[Row][Col]) > abs(Temp[Pivot][Col]))
			{
				Pivot = Row;
			}
		}

		if (abs(Temp[Pivot][Col]) < 1e-6f)
		{
			// Matrix is singular, cannot invert
			return GetIdentity();
		}

		for (int32 ColIndex = 0; ColIndex < 8; ColIndex++)
		{
			std::swap(Temp[Col][ColIndex], Temp[Pivot][ColIndex]);
		}

		// Normalize the pivot row
		float Divisio = Temp[Col][Col];
		for (int32 ColIndex = 0; ColIndex < 8; ColIndex++)
		{
			Temp[Col][ColIndex] /= Divisio;
		}

		// Eliminate the current column in other rows
		for (int32 Row = 0; Row < 4; Row++)
		{
			if (Row == Col)
			{
				continue;
			}

			float Factor = Temp[Row][Col];
			for (int32 ColIndex = 0; ColIndex < 8; ColIndex++)
			{
				Temp[Row][ColIndex] -= Factor * Temp[Col][ColIndex];
			}
		}
	}

	return FMatrix(
		FVector4(Temp[0][4], Temp[0][5], Temp[0][6], Temp[0][7]),
		FVector4(Temp[1][4], Temp[1][5], Temp[1][6], Temp[1][7]),
		FVector4(Temp[2][4], Temp[2][5], Temp[2][6], Temp[2][7]),
		FVector4(Temp[3][4], Temp[3][5], Temp[3][6], Temp[3][7])
	);
}

float* FMatrix::GetFloat16()
{
	return (&M[0][0]);
}

FMatrix FMatrix::GetIdentity()
{
	return FMatrix(
		FVector4(1.0f, 0.0f, 0.0f, 0.0f),
		FVector4(0.0f, 1.0f, 0.0f, 0.0f),
		FVector4(0.0f, 0.0f, 1.0f, 0.0f),
		FVector4(0.0f, 0.0f, 0.0f, 1.0f));
}

FQuaternion::FQuaternion()
	: X(0)
	, Y(0)
	, Z(0)
	, W(1)
{
}

FQuaternion::FQuaternion(float InX, float InY, float InZ, float InW)
	: X(InX)
	, Y(InY)
	, Z(InZ)
	, W(InW)
{
}

FQuaternion::FQuaternion(FVector Axis, float Angle)
{
	float HalfAngle = Angle * 0.5f;
	float SinHalfAngle = sin(HalfAngle);
	X = Axis.X * SinHalfAngle;
	Y = Axis.Y * SinHalfAngle;
	Z = Axis.Z * SinHalfAngle;
	W = cos(HalfAngle);
}

void FQuaternion::Normalize()
{
	float Length = sqrt(X * X + Y * Y + Z * Z + W * W);
	if (Length > 0.0f)
	{
		X /= Length;
		Y /= Length;
		Z /= Length;
		W /= Length;
	}
}

FQuaternion FQuaternion::Inverse() const
{
	return FQuaternion(-X, -Y, -Z, W);
}

FQuaternion FQuaternion::operator*(const FQuaternion& Other) const
{
	FVector Axis = Other.V * W + V * Other.S + V.Cross(Other.V);
	float Scalar = S * Other.S - V.Dot(Other.V);
	return FQuaternion(Axis.X, Axis.Y, Axis.Z, Scalar);
}

float& FQuaternion::operator[](int Index)
{
	return Data[Index];
}

FRay::FRay(const FVector& InOrigin, const FVector& InDirection)
	: Origin(InOrigin)
	, Direction(InDirection)
{
}

bool FRay::CheckAABB(FRay LocalRay, FVector MinVector, FVector MaxVector)
{
	FVector InverseDirection(1.0f / LocalRay.Direction.X, 1.0f / LocalRay.Direction.Y, 1.0f / LocalRay.Direction.Z);

	float Enter = (MinVector.X - LocalRay.Origin.X) * InverseDirection.X;
	float Exit = (MaxVector.X - LocalRay.Origin.X) * InverseDirection.X;

	if (Enter > Exit)
		std::swap(Enter, Exit);

	float TempEnter = (MinVector.Y - LocalRay.Origin.Y) * InverseDirection.Y;
	float TempExit = (MaxVector.Y - LocalRay.Origin.Y) * InverseDirection.Y;

	if (TempEnter > TempExit)
		std::swap(TempEnter, TempExit);

	Enter = TempEnter > Enter ? TempEnter : Enter;
	Exit = TempExit < Exit ? TempExit : Exit;

	TempEnter = (MinVector.Z - LocalRay.Origin.Z) * InverseDirection.Z;
	TempExit = (MaxVector.Z - LocalRay.Origin.Z) * InverseDirection.Z;

	if (TempEnter > TempExit)
		std::swap(TempEnter, TempExit);

	Enter = TempEnter > Enter ? TempEnter : Enter;
	Exit = TempExit < Exit ? TempExit : Exit;

	if (Enter > Exit || Exit < 0)
	{
		return (false);
	}
	return (true);
}

FTriangle::FTriangle(const FVector& InP0, const FVector& InP1, const FVector& InP2)
	: P0(InP0)
	, P1(InP1)
	, P2(InP2)
{
}

float Dot(const FVector2& A, const FVector2& B)
{
	return A.X * B.X + A.Y * B.Y;
}

float Dot(const FVector& A, const FVector& B)
{
	return A.Dot(B);
}

FVector Cross(const FVector& A, const FVector& B)
{
	return A.Cross(B);
}

FVector4 operator*(const FVector4& Vec, const FMatrix& Mat)
{
	FVector4 Result;
	Result.X = Vec.X * Mat.M[0][0] + Vec.Y * Mat.M[1][0] + Vec.Z * Mat.M[2][0] + Vec.W * Mat.M[3][0];
	Result.Y = Vec.X * Mat.M[0][1] + Vec.Y * Mat.M[1][1] + Vec.Z * Mat.M[2][1] + Vec.W * Mat.M[3][1];
	Result.Z = Vec.X * Mat.M[0][2] + Vec.Y * Mat.M[1][2] + Vec.Z * Mat.M[2][2] + Vec.W * Mat.M[3][2];
	Result.W = Vec.X * Mat.M[0][3] + Vec.Y * Mat.M[1][3] + Vec.Z * Mat.M[2][3] + Vec.W * Mat.M[3][3];
	return Result;
}

const FVector Front = FVector(1.0f, 0.0f, 0.0f);
const FVector Right = FVector(0.0f, 1.0f, 0.0f);
const FVector Up = FVector(0.0f, 0.0f, 1.0f);
