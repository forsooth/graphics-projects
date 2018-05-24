#ifndef FLATTENER_H
#define FLATTENER_H

#include <iostream>
#include "SceneParser.hpp"
#include "SceneData.hpp"
#include "Algebra.hpp"
#include "Shape.hpp"
#include "Cube.hpp"
#include "Sphere.hpp"
#include "Cylinder.hpp"
#include "Cone.hpp"
#include <algorithm>
#include <vector>

Shape *select_shape(int shapeType);

struct SceneOp {
        const unsigned int pops;
        const Matrix mat;
        const Matrix invmat;
        const Matrix invtransmat;
        const std::vector<ScenePrimitive *> objs;
        const std::vector<Shape *> shapes;
};

class Flattener {
public:
        std::vector<const SceneOp *> flattened;

        Flattener();
        ~Flattener();
        void flattenTree(SceneParser *parser);
        void flattenChild(SceneNode *node);
private:
        unsigned int id = 0;
};

#endif
