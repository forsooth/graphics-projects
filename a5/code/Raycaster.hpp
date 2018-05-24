#ifndef RAYCASTER_H
#define RAYCASTER_H

#include <limits>
#include <memory>
#include "Camera.hpp"
#include "SceneParser.hpp"
#include "Algebra.hpp"
#include "Flattener.hpp"
#include "Shape.hpp"
#include "KDTree.hpp"


struct Intersection {
    real minT;
    Shape *minShape;
    const SceneOp *minOp;
};

class Raycaster {
public:
        const Camera &cam;
        Flattener &flattener;
        const SceneParser &parser;
        const KDTree &tree;
        const real pixW;
        const real pixH;
        const int lens;
        unsigned int depthlimit;
        const int kdtree;
        const int shadows;
        const int reflections;
        const int objray;
        const int precompute_mats;

        Raycaster(const Camera &c, Flattener &f,
                     const SceneParser &p, const KDTree &t,
                     const real W, const real H,
                     const int lens, unsigned int limit,
                     const int kdtree, const int shadows,
                     const int reflections, const int objray,
                     const int precompute_mats);
        Vector generate_ray(real col, real row);
        SceneColor get_intensity(const SceneMaterial &material,
                                 const Point &isect,
                                 const Vector &N,
                                 const Vector &worldRay,
                                 Shape *shape,
                                 const SceneOp *op,
                                 const unsigned int depth);
        Intersection get_intersection(const Point &eye, const Vector &ray);
        SceneColor raycast(real col, real row);
        std::map<std::string, Texture> textures;
};

/* these are the global variables used for rendering */



#endif
