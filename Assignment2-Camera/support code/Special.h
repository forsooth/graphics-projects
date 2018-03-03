#ifndef SPECIAL_H
#define SPECIAL_H

#include "Shape.h"

class Special : public Shape {
public:
        Special();
        virtual ~Special() = default;

        void draw();

        void drawNormal();
};

#endif
