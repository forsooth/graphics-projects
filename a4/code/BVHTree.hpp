#ifndef BVHTree_H
#define BVHTree_H

#include "KDTree.hpp"
#include "BVHNode.hpp"
#include "KDNode.hpp"
#include "Box.hpp"

class BVHTree {
public:
        BVHTree(std::vector<const SceneOp *> sceneops);
        void intersected(std::vector<const SceneOp *> &intersect_obj, Point p, Vector ray);
        void print();
        void print_structure(int depth);
private:
        BVHNode* root;
};

#endif
