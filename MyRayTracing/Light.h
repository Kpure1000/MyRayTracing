#ifndef LIGHT_H
#define LIGHT_H
#include"Hitable.h"
namespace ry
{
	class Light : public Hitable
	{
	public:

		Light() {}

		virtual bool Hit(const Ray& r, const float& tMin, const float& tMax, HitRecord& rec)const
		{

		}
	};
}
#endif // !LIGHT_H