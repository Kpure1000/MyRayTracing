#pragma once
#include "Hitable.h"
#include"Sdf.h"
using namespace sdf;
namespace ry
{
    class InterHit : public Hitable
    {
    public:

        InterHit() {}

        InterHit(Hitable* a, Hitable* b, Material* mat)
            :Hitable(new SdfIntersection(a->sdf, b->sdf), mat),
            hA(a), hB(b)
        {
            SetMaterial(mat);
        }

        virtual bool Hit(const Ray& r, const float& tMin,
            const float& tMax, HitRecord& rec)
        {
            if (material != nullptr)
            {
                rec.mat = material;
            }
            return sdf->Hit(r, tMin, tMax, rec);
        }

        virtual void SetMaterial(Material* mat)
        {
            material = mat;
            hA->SetMaterial(material);
            hB->SetMaterial(material);
        }

        Hitable* hA, * hB;

    private:

        virtual void draw(RenderTarget& target, RenderStates states)const
        {
            target.draw(*hA, states);
            target.draw(*hB, states);
        }
    };
}
