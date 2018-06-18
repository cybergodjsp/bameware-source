#include "../includes.h"

#include "math.h"

namespace MATH
{
	void VectorAngles(const Vector& forward, Vector& angles)
	{
		float tmp, yaw, pitch;

		if (forward[1] == 0 && forward[0] == 0)
		{
			yaw = 0;
			if (forward[2] > 0)
				pitch = 270;
			else
				pitch = 90;
		}
		else
		{
			yaw = (atan2(forward[1], forward[0]) * 180 / M_PI);
			if (yaw < 0)
				yaw += 360;

			tmp = sqrt(forward[0] * forward[0] + forward[1] * forward[1]);
			pitch = (atan2(-forward[2], tmp) * 180 / M_PI);
			if (pitch < 0)
				pitch += 360;
		}

		angles[0] = pitch;
		angles[1] = yaw;
		angles[2] = 0;
	}

	void inline SinCos(float radians, float* sine, float* cosine)
	{
		*sine = sin(radians);
		*cosine = cos(radians);
	}

	void AngleVectors(const Vector& angles, Vector* forward, Vector* right, Vector* up)
	{
		float sr, sp, sy, cr, cp, cy;
		SinCos(DEG2RAD(angles[1]), &sy, &cy);
		SinCos(DEG2RAD(angles[0]), &sp, &cp);
		SinCos(DEG2RAD(angles[2]), &sr, &cr);

		if (forward)
		{
			forward->x = cp * cy;
			forward->y = cp * sy;
			forward->z = -sp;
		}
		if (right)
		{
			right->x = (-1 * sr * sp * cy + -1 * cr * -sy);
			right->y = (-1 * sr * sp * sy + -1 * cr * cy);
			right->z = -1 * sr * cp;
		}
		if (up)
		{
			up->x = (cr * sp * cy + -sr * -sy);
			up->y = (cr * sp * sy + -sr * cy);
			up->z = cr * cp;
		}
	}

	__forceinline float DotProduct(const float* a, const float* b)
	{
		return a[0] * b[0] + a[1] * b[1] + a[2] * b[2];
	}

	__forceinline float DotProduct(const Vector& a, const Vector& b)
	{
		return a.x * b.x + a.y * b.y + a.z * b.z;
	}

	void VectorTransform(const float* in1, const matrix3x4_t& in2, float* out)
	{
		out[0] = DotProduct(in1, in2[0]) + in2[0][3];
		out[1] = DotProduct(in1, in2[1]) + in2[1][3];
		out[2] = DotProduct(in1, in2[2]) + in2[2][3];
	}

	void VectorTransform(const Vector& in1, const matrix3x4_t& in2, Vector& out)
	{
		VectorTransform(&in1.x, in2, &out.x);
	}

	float CalcAngle2D(const Vector2D& src, const Vector2D& dst)
	{
		float angle;
		VectorAngle2D(dst - src, angle);
		return angle;
	}

	void VectorAngle2D(const Vector2D& direction, float& angle)
	{
		angle = RAD2DEG(std::atan2(direction.y, direction.x)) + 90.f;
	}

	void AngleVectors2D(float angle, Vector2D& forward)
	{
		angle = DEG2RAD(angle);
		Vector2D slope(sin(angle), -cos(angle));
		forward = slope;
	}

	/*
	void draw_angle_line2d(const Vector2D center, const float yaw, const float
		line_distance_from_center, const float line_length, const CColor line_color)
	{
		const auto start_position = get_rotated_screen_position(center, yaw, line_distance_from_center);
		const auto end_position = get_rotated_screen_position(center, yaw, line_distance_from_center + line_length);

		IRender->DrawLine(start_position.x, start_position.y, end_position.x, end_position.y, line_color);
	}
	Vector2D get_rotated_screen_position(Vector2D center, float yaw, float distance)
	{
		Vector2D slope = { sin(DEG2RAD(yaw)), -cos(DEG2RAD(yaw)) }; // x = run, y = rise
		Vector2D direction = slope;
		return center + (slope * distance);
	}
	*/
}
