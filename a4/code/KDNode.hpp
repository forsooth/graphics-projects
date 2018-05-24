#ifndef KDNODE_H
#define KDNODE_H

#include "Flattener.hpp"
#include "Box.hpp"

class KDNode {
public:
        // Recursively Construct Tree
        KDNode(std::vector<const SceneOp *> sops, unsigned int split_dim);
        void print();
        void print_structure(int depth);
        void intersected(std::vector<const SceneOp *> &intersect_obj, Point p, Vector ray);
        KDNode* left;
        KDNode* right;
        Box bounding_box;
        std::vector<const SceneOp *> sceneops;
private:
        int dim; // 0, 1, 2 -- depending on which dim this node splits on
};

#endif
