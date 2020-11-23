#pragma once
#include "Hitable.h"
#include"Sdf.h"
using namespace sdf;
namespace ry
{
    class UnionHit : public Hitable
    {
    public:

        UnionHit() {}

        UnionHit(Hitable* a, Hitable* b, Material* mat)
            :Hitable(new SdfUnion(a->sdf, b->sdf), mat),
            hA(a), hB(b)
        {
            SetMaterial(mat);
        }

        virtual bool Hit(const Ray& r, const float& tMin,
            const float& tMax, HitRecord& rec)
        {
            SdfRecord sdfR;
            auto result = sdf->Hit(r, tMin, tMax, rec, sdfR);
            if (sdfR == SdfRecord::A)
            {
                if (hA->material)rec.mat = hA->material;
            }
            else if (sdfR == SdfRecord::B)
            {
                if (hB->material)rec.mat = hB->material;
            }
            else
            {
                if (material)rec.mat = material;
            }
            //return result != SdfRecord::FALSE;
            return result;
        }

        virtual void SetMaterial(Material* mat)
        {
            material = mat;
            hA->SetMaterial(hA->material);
            hB->SetMaterial(hB->material);
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
