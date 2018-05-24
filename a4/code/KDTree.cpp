#include "KDTree.hpp"
#include "KDNode.hpp"

KDTree::KDTree(std::vector<const SceneOp *> sops) {
        root = new KDNode(sops, 0);
}

std::vector<const SceneOp *> KDTree::get_intersected(Point ray_origin, Vector ray_dir) const {
        std::vector<const SceneOp *> intersected;
        root->intersected(intersected, ray_origin, ray_dir);
        //std::cout << intersected.size() << std::endl;
        return intersected;
}

void KDTree::print() {
        std::cerr << "KDTree: ";
        root->print();
}

void KDTree::print_structure() {
        root->print_structure(0);
}
