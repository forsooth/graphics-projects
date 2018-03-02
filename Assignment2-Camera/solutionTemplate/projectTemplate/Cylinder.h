#ifndef CYLINDER_H
#define CYLINDER_H

#include "Shape.h"

class Cylinder : public Shape {
public:
        Cylinder();
        virtual ~Cylinder() = default;

        void draw();
        void drawNormal();

};
#endif
