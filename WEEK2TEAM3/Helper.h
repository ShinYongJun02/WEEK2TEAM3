#pragma once

#include <Core.h>
#include <cmath>

constexpr float PI = 3.14159265358979323846f;
constexpr float Epsilon = 1e-6f;
constexpr float Rad2Deg = 180.0f / PI;
constexpr float Deg2Rad = PI / 180.0f;

static float DegreeToRadian(float Degree)
{
	return Degree * Deg2Rad;
}

static bool EpsilonEqual(float A, float B, float InEpsilon = Epsilon)
{
	return std::fabs(A - B) < InEpsilon;
}

static float Remap(float Value, float InMin, float InMax, float OutMin, float OutMax)
{
	return (Value - InMin) / (InMax - InMin) * (OutMax - OutMin) + OutMin;
}

static float Clamp(float Value, float Min, float Max)
{
	return Value < Min ? Min : (Value > Max ? Max : Value);
}

static FMatrix Translate(float X, float Y, float Z)
{
	return FMatrix(
		FVector4(1.0f, 0.0f, 0.0f, 0.0f),
		FVector4(0.0f, 1.0f, 0.0f, 0.0f),
		FVector4(0.0f, 0.0f, 1.0f, 0.0f),
		FVector4(X, Y, Z, 1.0f)
	);
}

static FMatrix Scale(float X, float Y, float Z)
{
	return FMatrix(
		FVector4(X, 0.0f, 0.0f, 0.0f),
		FVector4(0.0f, Y, 0.0f, 0.0f),
		FVector4(0.0f, 0.0f, Z, 0.0f),
		FVector4(0.0f, 0.0f, 0.0f, 1.0f)
	);
}

static FMatrix RotateX(float Angle)
{
	float Radian = DegreeToRadian(Angle);
	float CosA = cos(Radian);
	float SinA = sin(Radian);
	return FMatrix(
		FVector4(1.0f, 0.0f, 0.0f, 0.0f),
		FVector4(0.0f, CosA, SinA, 0.0f),
		FVector4(0.0f, -SinA, CosA, 0.0f),
		FVector4(0.0f, 0.0f, 0.0f, 1.0f)
	);
}

static FMatrix RotateY(float Angle)
{
	float Radian = DegreeToRadian(Angle);
	float CosA = cos(Radian);
	float SinA = sin(Radian);
	return FMatrix(
		FVector4(CosA, 0.0f, -SinA, 0.0f),
		FVector4(0.0f, 1.0f, 0.0f, 0.0f),
		FVector4(SinA, 0.0f, CosA, 0.0f),
		FVector4(0.0f, 0.0f, 0.0f, 1.0f)
	);
}

static FMatrix RotateZ(float Angle)
{
	float Radian = DegreeToRadian(Angle);
	float CosA = cos(Radian);
	float SinA = sin(Radian);
	return FMatrix(
		FVector4(CosA, SinA, 0.0f, 0.0f),
		FVector4(-SinA, CosA, 0.0f, 0.0f),
		FVector4(0.0f, 0.0f, 1.0f, 0.0f),
		FVector4(0.0f, 0.0f, 0.0f, 1.0f)
	);
}

static FMatrix Rotate(float AngleX, float AngleY, float AngleZ)
{
	return RotateX(AngleX) * RotateY(AngleY) * RotateZ(AngleZ);
}

static FVector Rotate(const FQuaternion& Q, const FVector& V)
{
	FQuaternion QV(V.X, V.Y, V.Z, 0.0f);
	FQuaternion InvQ = Q.Inverse();
	return (Q * QV * InvQ).V;
}

static FMatrix ToMatrix(const FQuaternion& Q)
{
	float XX = Q.X * Q.X;
	float YY = Q.Y * Q.Y;
	float ZZ = Q.Z * Q.Z;
	float XY = Q.X * Q.Y;
	float XZ = Q.X * Q.Z;
	float YZ = Q.Y * Q.Z;
	float WX = Q.W * Q.X;
	float WY = Q.W * Q.Y;
	float WZ = Q.W * Q.Z;

	return FMatrix(
		FVector4(1.0f - 2.0f * (YY + ZZ), 2.0f * (XY + WZ), 2.0f * (XZ - WY), 0.0f),
		FVector4(2.0f * (XY - WZ), 1.0f - 2.0f * (XX + ZZ), 2.0f * (YZ + WX), 0.0f),
		FVector4(2.0f * (XZ + WY), 2.0f * (YZ - WX), 1.0f - 2.0f * (XX + YY), 0.0f),
		FVector4(0.0f, 0.0f, 0.0f, 1.0f)
	);
}

static FQuaternion ToQuaternion(const FMatrix& Matrix)
{
	float Trace = Matrix.M[0][0] + Matrix.M[1][1] + Matrix.M[2][2];

	FQuaternion Q;
	if (Trace > 0.f)
	{
		float S = sqrt(Trace + 1.f);
		Q[3] = S * 0.5f;

		float T = 0.5f / S;

		Q[0] = (Matrix.M[1][2] - Matrix.M[2][1]) * T;
		Q[1] = (Matrix.M[2][0] - Matrix.M[0][2]) * T;
		Q[2] = (Matrix.M[0][1] - Matrix.M[1][0]) * T;
	}
	else
	{
		int32 I = 0;
		if (Matrix.M[1][1] > Matrix.M[0][0]) I = 1;
		if (Matrix.M[2][2] > Matrix.M[I][I]) I = 2;

		static const int32 Next[3] = { 1, 2, 0 };

		int32 J = Next[I];
		int32 K = Next[J];

		float S = sqrt((Matrix.M[I][I] - (Matrix.M[J][J] + Matrix.M[K][K])) + 1.f);
		Q[I] = S * 0.5f;

		float T = S;
		if (S != 0.f) T = 0.5f / S;

		Q[3] = (Matrix.M[J][K] - Matrix.M[K][J]) * T;
		Q[J] = (Matrix.M[J][I] + Matrix.M[I][J]) * T;
		Q[K] = (Matrix.M[K][I] + Matrix.M[I][K]) * T;
	}

	return Q;
}

static FVector ExtractRotationFromMatrix(const FMatrix& Matrix)
{
	float Y = asin(Clamp(Matrix.M[0][2], -1.f, 1.f));
	float X = atan2(-Matrix.M[1][2], Matrix.M[2][2]);
	float Z = atan2(Matrix.M[0][1], Matrix.M[0][0]);
	return FVector(X, Y, Z);
}

static FVector ToEulerAngles(const FQuaternion& Q)
{
	FMatrix Matrix = ToMatrix(Q);
	return ExtractRotationFromMatrix(Matrix);
}

static FMatrix Ortho(float Left, float Right, float Bottom, float Top, float NearZ, float FarZ)
{
	return FMatrix{
		FVector4(2.0f / (Right - Left), 0.0f, 0.0f, 0.0f),
		FVector4(0.0f, 2.0f / (Top - Bottom), 0.0f, 0.0f),
		FVector4(0.0f, 0.0f, 1.0f / (FarZ - NearZ), 0.0f),
		FVector4(-(Right + Left) / (Right - Left), -(Top + Bottom) / (Top - Bottom), -NearZ / (FarZ - NearZ), 1.0f)
	};
}

static float DistanceSquared(const FVector2& A, const FVector2& B)
{
	return (A - B).LengthSquared();
}

static float DistanceSquared(const FVector& A, const FVector& B)
{
	return (A - B).LengthSquared();
}

static float Raycast(const FRay& Ray, const FTriangle& Triangle)
{
	FVector E1 = Triangle.P1 - Triangle.P0;
	FVector E2 = Triangle.P2 - Triangle.P0;

	FVector H = Cross(Ray.Direction, E2);

	float Det = Dot(E1, H);
	if (EpsilonEqual(Det, 0.0f))
	{
		return -1.0f; // Ray is parallel to the triangle
	}

	FVector S = Ray.Origin - Triangle.P0;

	float U = Dot(S, H) / Det;
	if (U < 0.0f || U > 1.0f)
	{
		return -1.0f; // Intersection point is outside the triangle
	}

	FVector Q = Cross(S, E1);

	float V = Dot(Ray.Direction, Q) / Det;
	if (V < 0.0f || U + V > 1.0f)
	{
		return -1.0f; // Intersection point is outside the triangle
	}

	float T = Dot(E2, Q) / Det;
	if (T < Epsilon)
	{
		return -1.0f; // Intersection point is behind the ray origin
	}

	return T;
}

static FVector2 WorldToScreen(const FVector& WorldPos, const FMatrix& ViewProjection, int32 ScreenWidth, int32 ScreenHeight)
{
	FVector4 ClipSpacePos = FVector4(WorldPos.X, WorldPos.Y, WorldPos.Z, 1.0f) * ViewProjection;

	FVector2 NdcPos(ClipSpacePos.X / ClipSpacePos.W, ClipSpacePos.Y / ClipSpacePos.W);
	FVector2 ScreenPos(
		(NdcPos.X + 1.0f) * 0.5f * ScreenWidth,
		(1.0f - (NdcPos.Y + 1.0f) * 0.5f) * ScreenHeight
	);

	return ScreenPos;
}

static bool LineSegmentIntersect(const FVector2& P0, const FVector2& P1, const FVector2& P2, const FVector2& P3, FVector2& Intersection)
{
	float Denominator = (P0.X - P1.X) * (P2.Y - P3.Y) - (P0.Y - P1.Y) * (P2.X - P3.X);

	if (EpsilonEqual(Denominator, 0.0f))
	{
		return false; // Lines are parallel or coincident
	}

	float T = ((P0.X - P2.X) * (P2.Y - P3.Y) - (P0.Y - P2.Y) * (P2.X - P3.X)) / Denominator;
	float U = ((P0.X - P2.X) * (P0.Y - P1.Y) - (P0.Y - P2.Y) * (P0.X - P1.X)) / Denominator;

	if (T < 0.f || T > 1.f || U < 0.f || U > 1.f)
	{
		return false;
	}

	Intersection.X = P0.X + T * (P1.X - P0.X);
	Intersection.Y = P0.Y + T * (P1.Y - P0.Y);

	return true;
}

static float PointToLineSegmentDistanceSquared(const FVector2& Point, const FVector2& LineStart, const FVector2& LineEnd)
{
	FVector2 LineVec = LineEnd - LineStart;

	float LineLength = LineVec.Length();
	if (LineLength == 0.f)
	{
		return DistanceSquared(Point, LineStart);
	}
	LineVec /= LineLength;

	FVector2 StartToPoint = Point - LineStart;
	float ProjectedLength = Dot(StartToPoint, LineVec);

	if (ProjectedLength < 0.f)
	{
		ProjectedLength = 0.f;
	}
	else if (ProjectedLength > LineLength)
	{
		ProjectedLength = LineLength;
	}

	FVector2 Closest = LineStart + LineVec * ProjectedLength;
	return DistanceSquared(Point, Closest);
}

static float PointToLineSegmentDistanceSquared(const FVector& Point, const FVector& LineStart, const FVector& LineEnd)
{
	FVector LineVec = LineEnd - LineStart;

	float LineLength = LineVec.Length();
	if (LineLength == 0.f)
	{
		return DistanceSquared(Point, LineStart);
	}

	LineVec /= LineLength;

	FVector StartToPoint = Point - LineStart;
	float ProjectedLength = Dot(StartToPoint, LineVec);

	if (ProjectedLength < 0.f)
	{
		ProjectedLength = 0.f;
	}
	else if (ProjectedLength > LineLength)
	{
		ProjectedLength = LineLength;
	}

	FVector Closest = LineStart + LineVec * ProjectedLength;
	return DistanceSquared(Point, Closest);
}

static void GenerateCircleVertices(const std::function<void(int32 Index, const FVector2&)>& Handler, float Radius, int Segments)
{
	const float Step = 2.0f * PI / static_cast<float>(Segments);

	for (int32 Index = 0; Index < Segments; ++Index)
	{
		float Angle = Step * static_cast<float>(Index);
		float X = Radius * cos(Angle);
		float Y = Radius * sin(Angle);

		Handler(Index, FVector2(X, Y));
	}
}

static FVector Lerp(const FVector& A, const FVector& B, float T)
{
	return A * (1.0f - T) + B * T;
}

static FUUID StringToUUID(const FString InString)
{
	FUUID Result;

	std::stringstream Stream(InString);
	std::string Token;

	TArray<uint32> UIntArray;

	while (std::getline(Stream, Token, '-'))
	{
		if (!Token.empty())
		{
			uint32 Num = static_cast<uint32>(std::stoul(Token));
			UIntArray.push_back(Num);
		}
	}

	if (UIntArray.size() == 4)
	{
		Result = FUUID(UIntArray[0], UIntArray[1], UIntArray[2], UIntArray[3]);
	}

	return Result;
}