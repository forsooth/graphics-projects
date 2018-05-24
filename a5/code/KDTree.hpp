#ifndef KD_TREE
#define KD_TREE

#include "Algebra.hpp"
#include "Flattener.hpp"
#include <vector>
#include "KDNode.hpp"

class KDTree {
public:
        KDTree();
        KDTree(std::vector<const SceneOp *> sops);
        std::vector<const SceneOp *> get_intersected(const Point &eye,
                                                     const Vector &ray) const;
        void print();
        void print_structure();
        KDNode *root;
};

#endif
