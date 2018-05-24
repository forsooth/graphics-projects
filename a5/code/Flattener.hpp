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
#include "ppm.hpp"
#include <algorithm>
#include <vector>

Shape *select_shape(int shapeType);

struct SceneOp {
        const bool notexture;
        const unsigned int pops;
        const Matrix mat;
        const Matrix invmat;
        const Matrix invtransmat;
        const std::string filename;
        const std::vector<ScenePrimitive *> objs;
        const std::vector<Shape *> shapes;
};

struct Texture {
    unsigned int width;
    unsigned int height;
    unsigned int i;
    unsigned int j;
    unsigned char *pixels;
};

class Flattener {
public:
        std::vector<const SceneOp *> flattened;
        std::map<std::string, Texture> textures;

        Flattener();
        ~Flattener();
        void flattenTree(SceneParser &parser);
        void flattenChild(SceneNode *node);
private:
        unsigned int id = 0;
};

#endif
