#ifndef SPHERE_H
#define SPHERE_H

#include <limits>
#include "Shape.hpp"
#include "SceneData.hpp"

class Sphere : public Shape {
public:
        Sphere();
        void draw();
        void drawNormal();
        real Intersect(const Point eye, const Vector ray);
        Vector findIsectNormal(const Point eye, const Vector ray, real t);
        Point to_unit_plane(const Point &isect);
        PrimitiveType type = SHAPE_SPHERE;

};

#endif
