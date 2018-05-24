#ifndef CONE_H
#define CONE_H

#include <limits>
#include "Shape.hpp"
#include "SceneData.hpp"

class Cone : public Shape {
public:
        Cone();
        void draw();
        void drawNormal();
        real Intersect(const Point eye, const Vector ray);
        Vector findIsectNormal(const Point eye, const Vector ray, real t);
        PrimitiveType type = SHAPE_CONE;
};

#endif
