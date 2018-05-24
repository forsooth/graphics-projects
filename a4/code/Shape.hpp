#ifndef SHAPE_H
#define SHAPE_H

#include <GL/glui.h>
#include "Algebra.hpp"
#include "SceneData.hpp"

#define NormL 1.1f

class Shape {
public:
        Shape() {};
        virtual ~Shape() = default;

        void setSegments(int x, int y) {
                m_segmentsX = x;
                m_segmentsY = y;
        }

        virtual void draw() {};
        virtual void drawNormal() {};
        virtual real
        Intersect(const Point eye, const Vector ray) = 0;
        virtual Vector findIsectNormal(const Point eye, const Vector ray,
                                       real t) = 0;

        PrimitiveType type;

protected:
        void normalizeNormal (real x, real y, real z) {
                normalizeNormal(Vector(x, y, z));
        };

        void normalizeNormal (Vector v) {
                v.normalize();
#ifdef FLOATALG
                glNormal3fv(v.unpack());
#else
                glNormal3dv(v.unpack());
#endif
        };

real m_segmentsX = 3, m_segmentsY = 3;

};

#endif
