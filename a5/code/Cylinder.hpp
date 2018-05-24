#ifndef CYLINDER_H
#define CYLINDER_H

#include <limits>
#include <cmath>
#include "Shape.hpp"
#include "SceneData.hpp"

class Cylinder : public Shape {
public:
        Cylinder();
        void draw();
        void drawNormal();
        real Intersect(const Point eye, const Vector ray);
        Vector findIsectNormal(const Point eye, const Vector ray, real t);
        Point to_unit_plane(const Point &isect);
        PrimitiveType type = SHAPE_CYLINDER;

};
#endif
