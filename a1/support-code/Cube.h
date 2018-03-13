#ifndef CUBE_H
#define CUBE_H

#include "Shape.h"

class Cube : public Shape {
public:
        Cube();
        virtual ~Cube() = default;

        void draw();


        void drawNormal();
};

#endif
