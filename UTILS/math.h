#pragma once

#ifndef M_PI
#define M_PI		3.14159265358979323846	// matches value in gcc v2 math.h
#endif

#define M_PI_F		((float)(M_PI))	// Shouldn't collide with anything.
#ifndef RAD2DEG
#define RAD2DEG( x )  ( (float)(x) * (float)(180.f / M_PI_F) )
#endif

#ifndef DEG2RAD
#define DEG2RAD( x )  ( (float)(x) * (float)(M_PI_F / 180.f) )
#endif

namespace MATH
{
	void VectorAngles(const Vector& forward, Vector& angles);

	void SinCos(float radians, float* sine, float* cosine);
	void AngleVectors(const Vector& angles, Vector* forward = nullptr, Vector* right = nullptr, Vector* up = nullptr);

	float DotProduct(const float* a, const float* b);

	void VectorTransform(const float* in1, const matrix3x4_t& in2, float* out);
	void VectorTransform(const Vector& in1, const matrix3x4_t& in2, Vector& out);

	float CalcAngle2D(const Vector2D& src, const Vector2D& dst);
	void VectorAngle2D(const Vector2D& direction, float& angle);
	void AngleVectors2D(float angle, Vector2D& forward);

	inline float NormalizePitch(float pitch)
	{
		while (pitch > 89.f)
			pitch -= 180.f;
		while (pitch < -89.f)
			pitch += 180.f;

		return pitch;
	}

	inline float NormalizeYaw(float yaw)
	{
		if (yaw > 180)
			yaw -= (round(yaw / 360) * 360.f);
		else if (yaw < -180)
			yaw += (round(yaw / 360) * -360.f);

		return yaw;
	}

	inline float RandomNumber(float min, float max)
	{
		static std::random_device rand_device;
		static std::mt19937 rand_gen(rand_device());

		std::uniform_real_distribution<float> rand_distributer(min, max);
		return rand_distributer(rand_gen);
	}

	inline float GetYawDelta(float yaw1, float yaw2)
	{
		return fabs(NormalizeYaw(yaw1 - yaw2));
	}

	inline float Lerp(float fraction, float min, float max)
	{
		return min + (fraction * (max - min));
	}

	inline Vector NormalizeAngle(Vector angle)
	{
		angle.x = NormalizePitch(angle.x);
		angle.y = NormalizeYaw(angle.y);

		if ((angle.z > 50) || (angle.z < 50))
			angle.z = 0;

		return angle;
	}

	inline Vector CalcAngle(const Vector& src, const Vector& dst)
	{
		Vector ret;
		VectorAngles(dst - src, ret);
		return ret;
	}
}
