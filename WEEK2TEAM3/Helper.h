#pragma once

#include <functional>

#include "Core.h"
#include "FUUID.h"

constexpr float PI = 3.14159265358979323846f;
constexpr float Epsilon = 1e-6f;
constexpr float Rad2Deg = 180.0f / PI;
constexpr float Deg2Rad = PI / 180.0f;

float DegreeToRadian(float Degree);
bool EpsilonEqual(float A, float B, float InEpsilon = Epsilon);
float Remap(float Value, float InMin, float InMax, float OutMin, float OutMax);
float Clamp(float Value, float Min, float Max);

FMatrix Translate(float X, float Y, float Z);
FMatrix Scale(float X, float Y, float Z);
FMatrix RotateX(float Angle);
FMatrix RotateY(float Angle);
FMatrix RotateZ(float Angle);
FMatrix Rotate(float AngleX, float AngleY, float AngleZ);
FVector Rotate(const FQuaternion& Q, const FVector& V);
FMatrix Ortho(float Left, float Right, float Bottom, float Top, float NearZ, float FarZ);

FMatrix ToMatrix(const FQuaternion& Q);
FQuaternion ToQuaternion(const FMatrix& Matrix);
FVector ExtractRotationFromMatrix(const FMatrix& Matrix);
FVector ToEulerAngles(const FQuaternion& Q);

float DistanceSquared(const FVector2& A, const FVector2& B);
float DistanceSquared(const FVector& A, const FVector& B);
float PointToLineSegmentDistanceSquared(const FVector2& Point, const FVector2& LineStart, const FVector2& LineEnd);
float PointToLineSegmentDistanceSquared(const FVector& Point, const FVector& LineStart, const FVector& LineEnd);

float Raycast(const FRay& Ray, const FTriangle& Triangle);
FVector2 WorldToScreen(const FVector& WorldPos, const FMatrix& ViewProjection, int32 ScreenWidth, int32 ScreenHeight);
bool LineSegmentIntersect(const FVector2& P0, const FVector2& P1, const FVector2& P2, const FVector2& P3, FVector2& Intersection);
void GenerateCircleVertices(const std::function<void(int32 Index, const FVector2&)>& Handler, float Radius, int Segments);
FVector Lerp(const FVector& A, const FVector& B, float T);

FUUID StringToUUID(const FString InString);
