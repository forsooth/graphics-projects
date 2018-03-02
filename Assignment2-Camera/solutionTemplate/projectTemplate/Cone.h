#ifndef CONE_H
#define CONE_H

#include "Shape.h"

class Cone : public Shape {
public:
        Cone();
        virtual ~Cone() = default;


        void draw();

        void drawNormal();
};

#endif
