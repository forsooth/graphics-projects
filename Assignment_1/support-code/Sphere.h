#ifndef SPHERE_H
#define SPHERE_H

#include "Shape.h"

class Sphere : public Shape {
public:
        Sphere();
        virtual ~Sphere() = default;

        void draw();

        void drawNormal();
};

#endif
