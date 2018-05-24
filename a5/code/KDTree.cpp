#include "KDTree.hpp"
#include "KDNode.hpp"

KDTree::KDTree() {
        root = NULL;
}

KDTree::KDTree(std::vector<const SceneOp *> sops) {
        root = new KDNode(sops, 0);
}

std::vector<const SceneOp *> KDTree::get_intersected(const Point &eye,
                                                     const Vector &ray) const {
        std::vector<const SceneOp *> intersected;
        root->intersected(intersected, eye, ray);
        return intersected;
}

void KDTree::print() {
        std::cerr << "KDTree: ";
        root->print();
}

void KDTree::print_structure() {
        root->print_structure(0);
}
