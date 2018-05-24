#ifndef CUBE_H
#define CUBE_H

#include <limits>
#include "Shape.hpp"
#include "SceneData.hpp"

class Cube : public Shape {
public:
        Cube();
        void draw();
        void drawNormal();
        real Intersect(const Point eye, const Vector ray);
        Vector findIsectNormal(const Point eye, const Vector ray, real t);
        PrimitiveType type = SHAPE_CUBE;
};

#endif
