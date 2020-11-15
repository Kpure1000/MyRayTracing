#ifndef RAYMATH_H
#define RAYMATH_H
#include"Vector3.h"
#include<iostream>

using namespace ry;

#ifndef Pi
#define Pi (acosf(-1.0f))
#endif // !Pi


Vector3 randomUnitVector()
{
	Vector3 re;
	do
	{
		re = 2.0f * Vector3(rand() % 100 / (float)100,
			rand() % 100 / (float)100, rand() % 100 / (float)100) - Vector3::One;
	} while (re.Magnitude() > 1.0f);
	return re;
}

Vector3 Reflect(const Vector3& in, const Vector3& normal)
{
	return in - 2 * Vector3::Dot(in, normal) * normal;
}

/*
* ����
* in �����
* normal ����
* refRate ��������ʣ���/����
* refracted �����
*/
bool Refract(const Vector3& in, const Vector3& normal, const float& refRate, Vector3& refracted)
{
	//float dot_in_n = Vector3::Dot(in, normal);
	//if (refRate <= 1) //  �ӹ�����ʵ����ܽ���
	//{
	//	refracted = in * refRate + normal * dot_in_n * (1 + refRate);
	//	return true;
	//}
	//else //  �ӹ��ܽ��ʵ��������
	//{
	//	float re_refRate = 1.0f / refRate;
	//	float consine_in = dot_in_n / in.Magnitude();
	//	float sinsine_in_sq = 1 - consine_in * consine_in;
	//	if (sinsine_in_sq <= re_refRate * re_refRate) //  δ��ȫ�ڷ����
	//	{
	//		refracted = in * refRate + normal * dot_in_n * (1 + refRate);
	//		return true;
	//	}
	//}
	//return false;
	Vector3 uvin = Vector3::Normalize(in);
	float dt = Vector3::Dot(uvin, normal);
	float discrimination = 1 - refRate * refRate * (1 - dt * dt);
	if (discrimination > 0)
	{
		refracted = refRate * (uvin - normal * dt) - normal * sqrt(discrimination);
		return true;
	}
	return false;
}

float Schlick(const float& cosine, const float& refRate)
{
	float r0 = (1 - refRate) / (1 + refRate);
	r0 *= r0;
	return r0 + (1 - r0) * powf((1 - cosine), 5);
}

#endif // !RAYMATH_H

#ifndef MAX_FLOAT
#define MAX_FLOAT 1e3f
#endif // !MAX_FLOAT

#ifndef TO_RGB
#define TO_RGB 255.5f

#endif // !_TO_RGB
