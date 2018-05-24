#include "BVHTree.hpp"

BVHTree::BVHTree(std::vector<const SceneOp *> sceneops) {
        KDTree kdtree(sceneops);
        KDNode kdrootnode = kdtree->root;
        root = BVHNode(kdrootnode);
}

void BVHTree::print() {

}

void BVHTree::print_structure(int depth) {

}


void BVHTree::intersected(std::vector<const SceneOp *> &intersect_obj, Point p, Vector ray) {
        if (!bounding_box.intersect_box(p, ray)) {
                return;
        }

        intersect_obj.insert(std::end(intersect_obj), std::begin(sceneops), std::end(sceneops));

        if (left != nullptr) {
                left->intersected(intersect_obj, p, ray);
        }
        if (right != nullptr) {
                right->intersected(intersect_obj, p, ray);
        }
}