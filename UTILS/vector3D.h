#pragma once
#include <math.h>

#define M_PI 3.14159265358979323846

class Vector
{
public:
	float x, y, z;

	inline void Init(float ix, float iy, float iz)
	{
		x = ix;
		y = iy;
		z = iz;
	}

	Vector()
	{
		x = 0;
		y = 0;
		z = 0;
	};

	Vector(float X, float Y, float Z)
	{
		x = X;
		y = Y;
		z = Z;
	};

	float operator[](int i) const
	{
		if (i == 0)
			return x;
		if (i == 1)
			return y;
		return z;
	};

	float& operator[](int i)
	{
		if (i == 0)
			return x;
		if (i == 1)
			return y;
		return z;
	};

	bool operator==(const Vector& v) const
	{
		return x == v.x && y == v.y && z == v.z;
	}

	bool operator!=(const Vector& v) const
	{
		return x != v.x || y != v.y || z != v.z;
	}

	inline Vector operator-(const Vector& v) const
	{
		return Vector(x - v.x, y - v.y, z - v.z);
	}

	inline Vector operator+(const Vector& v) const
	{
		return Vector(x + v.x, y + v.y, z + v.z);
	}

	inline Vector operator*(const Vector& v) const
	{
		return Vector(x * v.x, y * v.y, z * v.z);
	}

	inline Vector operator/(const Vector& v) const
	{
		return Vector(x / v.x, y / v.y, z / v.z);
	}

	inline Vector operator*(const int n) const
	{
		return Vector(x * n, y * n, z * n);
	}

	inline Vector operator*(const float n) const
	{
		return Vector(x * n, y * n, z * n);
	}

	inline Vector operator/(const int n) const
	{
		return Vector(x / n, y / n, z / n);
	}

	inline Vector operator/(const float n) const
	{
		return Vector(x / n, y / n, z / n);
	}

	inline Vector operator-()
	{
		return Vector(-x, -y, -z);
	}

	inline Vector& operator+=(const Vector& v)
	{
		x += v.x;
		y += v.y;
		z += v.z;
		return *this;
	}

	inline Vector& operator-=(const Vector& v)
	{
		x -= v.x;
		y -= v.y;
		z -= v.z;
		return *this;
	}

	inline Vector& operator*=(const Vector& v)
	{
		x *= v.x;
		y *= v.y;
		z *= v.z;
		return *this;
	}

	inline Vector& operator/=(const Vector& v)
	{
		x /= v.x;
		y /= v.y;
		z /= v.z;
		return *this;
	}

	inline void operator*=(const float& v)
	{
		x *= v;
		y *= v;
		z *= v;
	}

	float LengthSqr(void)
	{
		return (x * x + y * y + z * z);
	}

	float Length(void) const
	{
		return sqrt(x * x + y * y + z * z);
	}

	inline float Length2D() const
	{
		return sqrt((x * x) + (y * y));
	}

	//T must be between 0 and 1
	Vector lerp(Vector target, float t)
	{
		return *this * (1 - t) + target * t;
	}

	void lerpme(Vector target, float t)
	{
		*this = *this * (1 - t) + target * t;
	}

	inline float Dot(const Vector& e) const
	{
		return (x * e.x) + (y * e.y) + (z * e.z);
	}

	float NormalizeInPlace()
	{
		Vector& v = *this;

		float iradius = 1.f / (this->LengthSqr() + 1.192092896e-07F); //FLT_EPSILON

		v.x *= iradius;
		v.y *= iradius;
		v.z *= iradius;
		return iradius;
	}
};
