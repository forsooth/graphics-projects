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

/* these are the global variables used for rendering */
Vector generate_ray(bool obj, real col, real row,
                    real pixW, real pixH, const Camera *cam, int lens);
SceneColor get_intensity(const SceneMaterial &material, const Vector &Ixt,
                         const Vector &N, const SceneParser *parser,
                         const SceneOp *thisOp, PrimitiveType thisType,
                         const KDTree *tree);
SceneColor raycast(real col, real row, real pixW, real pixH, const Camera *cam,
                   const SceneParser *parser, const KDTree *tree, int lens);

#endif
