#ifndef BVHNODE_H
#define BVHNODE_H

#include "Flattener.hpp"
#include "Box.hpp"

class BVHNode {
public:
        BVHNode(KDNode *root);
        void intersected(std::vector<const SceneOp *> &intersect_obj, Point p, Vector ray);
private:
        BVHNode* left;
        BVHNode* right;
        Box bounding_box;
        std::vector<const SceneOp *> sceneops;
};

#endif