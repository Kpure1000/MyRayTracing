#ifndef SDF_H
#define SDF_H
#include"Ray.h"
#include"Hitable.h"
class Sdf
{
public:
	virtual bool Hit(const Ray& r, const float& tMin,
		const float& tMax, HitRecord& result)const = 0;

	virtual bool sdf(const Vector2f& p, float& sdfResult)const = 0;

};



#endif // !SDF_H
