/*---------------------------------------------------------------------
*
* Copyright © 2015  Minsi Chen
* E-mail: m.chen@derby.ac.uk
*
* The source is written for the Graphics I and II modules. You are free
* to use and extend the functionality. The code provided here is functional
* however the author does not guarantee its performance.
---------------------------------------------------------------------*/
#include <math.h>
#include "Vector3.h"

Vector3::Vector3()
{
	SetVector(0.0f, 0.0f, 0.0f);
}

Vector3::Vector3(const Vector3& rhs)
{
	mVector = rhs.mVector;
}

Vector3::Vector3(float x, float y, float z)
{
	SetVector(x, y, z);
}

float Vector3::operator [] (const int i) const
{
	return mVector.m128_f32[i];
}

float& Vector3::operator [] (const int i)
{
	return mVector.m128_f32[i];
}

Vector3 Vector3::operator + (const Vector3& rhs) const
{
	__m128 r = _mm_add_ps(mVector, rhs.mVector);

	return Vector3(r.m128_f32[0], r.m128_f32[1], r.m128_f32[2]);
}

Vector3 Vector3::operator - (const Vector3& rhs) const
{
	__m128 r = _mm_sub_ps(mVector, rhs.mVector);

	return Vector3(r.m128_f32[0], r.m128_f32[1], r.m128_f32[2]);
}

//Vector3 Vector3::operator = (const Vector3& rhs)
//{
//	/*m_element[0] = rhs[0];
//	m_element[1] = rhs[1];
//	m_element[2] = rhs[2];*/
//
//	mVector = rhs.mVector;
//
//	return *this;
//}

Vector3 Vector3::operator * (const Vector3& rhs) const
{
	__m128 r = _mm_mul_ps(mVector, rhs.mVector);

	return Vector3(r.m128_f32[0], r.m128_f32[1], r.m128_f32[2]);
}

Vector3 Vector3::operator * (float scale) const
{
	return Vector3(mVector.m128_f32[0] * scale, mVector.m128_f32[1] * scale, mVector.m128_f32[2] * scale);
}

Vector3 Vector3::operator / (const Vector3& rhs) const
{
	__m128 r = _mm_div_ps(mVector, rhs.mVector);

	return Vector3(r.m128_f32[0], r.m128_f32[1], r.m128_f32[2]);
}

Vector3 Vector3::operator / (float scale) const
{
	return Vector3(mVector.m128_f32[0] / scale, mVector.m128_f32[1] / scale, mVector.m128_f32[2] / scale);
}

float Vector3::Norm() const
{
	__m128 r = _mm_mul_ps(mVector, mVector);

	return sqrt(r.m128_f32[0] + r.m128_f32[1] + r.m128_f32[2]);
}

float Vector3::Norm_Sqr() const
{
	__m128 r = _mm_mul_ps(mVector, mVector);

	return r.m128_f32[0] + r.m128_f32[1] + r.m128_f32[2];
}

float Vector3::DotProduct(const Vector3& rhs) const
{
	__m128 r = _mm_mul_ps(mVector, rhs.mVector);

	return r.m128_f32[0] + r.m128_f32[1] + r.m128_f32[2];
}

Vector3 Vector3::Normalise()
{
	float length = this->Norm_Sqr();

	if (length > 1.0e-8f)
	{
		__m128 l = _mm_set1_ps(length);
		l = _mm_rsqrt_ps(l);
		mVector = _mm_mul_ps(mVector, l);
	}

	return *this;
}

Vector3 Vector3::CrossProduct(const Vector3& rhs) const
{
	__m128 a = _mm_shuffle_ps(mVector, rhs.mVector, _MM_SHUFFLE(0, 2, 2, 1));
	__m128 b = _mm_shuffle_ps(rhs.mVector, mVector, _MM_SHUFFLE(2, 0, 1, 2));
	__m128 c = _mm_mul_ps(a, b);

	return Vector3(c.m128_f32[0] - c.m128_f32[1], c.m128_f32[3] - c.m128_f32[2], mVector.m128_f32[0] * rhs.mVector.m128_f32[1] - mVector.m128_f32[1] * rhs.mVector.m128_f32[0]);
}

Vector3 Vector3::Reflect(const Vector3 & n) const
{
	Vector3 result;

	float IndotN = -2.0f*this->DotProduct(n);

	result = *this + n*IndotN;

	return result;
}

Vector3 Vector3::Refract(const Vector3 & n, float r_index) const
{
	Vector3 result;
	result.SetZero();
	float IndotN = this->DotProduct(n);

	if (IndotN > 0.0f)
	{
		Vector3 nn = n*(-1.0f);
		IndotN = -this->DotProduct(nn);
	}
	else
		IndotN = -IndotN;

	float k = 1.0f - r_index*r_index*(1.0f - IndotN*IndotN);

	if (k >= 0.0f)
		result = (*this)*r_index + n*(r_index*IndotN - sqrt(k));

	return result;
}

void Vector3::SetZero()
{
	mVector = _mm_setzero_ps();
}
