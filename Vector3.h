/*---------------------------------------------------------------------
*
* Copyright © 2015  Minsi Chen
* E-mail: m.chen@derby.ac.uk
*
* The source is written for the Graphics I and II modules. You are free
* to use and extend the functionality. The code provided here is functional
* however the author does not guarantee its performance.
---------------------------------------------------------------------*/
#pragma once

#include <immintrin.h>

typedef __m128 Vec4;

class Vector3
{
private:
	//float	m_element[3];
	Vec4	mVector;

public:
	Vector3();

	Vector3(const Vector3& rhs);

	Vector3(float x, float y, float z);

	~Vector3() { ; }

	float operator [] (const int i) const;
	float& operator [] (const int i);
	Vector3 operator + (const Vector3& rhs) const;
	Vector3 operator - (const Vector3& rhs) const;
	//Vector3 operator = (const Vector3& rhs);
	Vector3 operator * (const Vector3& rhs) const;
	Vector3 operator * (float scale) const;
	Vector3 operator / (const Vector3& rhs) const;
	Vector3 operator / (float scale) const;

	float Norm()	const;
	float Norm_Sqr() const;
	Vector3 Normalise();

	float DotProduct(const Vector3& rhs) const;
	Vector3 CrossProduct(const Vector3& rhs) const;

	Vector3 Reflect(const Vector3& n) const;
	Vector3 Refract(const Vector3& n, float r_index) const;

	void SetZero();

	inline void SetVector(float x, float y, float z)
	{
		mVector = _mm_set_ps(0.0f, z, y, x);
	}
};
