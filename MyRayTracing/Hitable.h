#ifndef HITABLE_H
#define HITABLE_H
#include"Ray.h"

namespace ry
{

	class Metarial;

	struct HitRecord
	{
		float t;
		Vector3 vertex;
		Vector3 normal;
	};

	class Hitable
	{
	public:

		//����������ײ����record,���߱�Լ����tMin��tMax֮��
		virtual bool Hit(const Ray& r, const float& tMin, const float& tMax, HitRecord& rec)const = 0;
	};

}

#endif // !HITABLE_H