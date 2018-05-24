#include "KDNode.hpp"
#include "Flattener.hpp"
#include <limits>

KDNode::KDNode(std::vector<const SceneOp *> sops, unsigned int depth) {

        unsigned int split_dim = depth % 3;

        if (sops.size() == 0) {
                sceneops = sops;
                dim = -1;
                left = nullptr;
                right = nullptr;
                return;
        }
        if (sops.size() == 1) {
                sceneops = sops;
                dim = -1;
                left = nullptr;
                right = nullptr;
                bounding_box = sops[0]->mat * Box();
                return;
        }

        if (depth > 5) {
                bounding_box = Box();
                for (const SceneOp *op : sops) {
                        Box b = op-> mat * Box();
                        bounding_box.expand_box(b);
                }
                sceneops = sops;
                dim = -1;
                left = nullptr;
                right = nullptr;
                return;
        }

        bool after_first = false;

        for (const SceneOp *op : sops) {
                Box op_box = Box();
                op_box = op->mat * op_box;
                if (after_first) {
                        bounding_box.expand_box(op_box);
                } else {
                        bounding_box = op_box;
                        after_first = true;
                }
        }

        Box leftb;
        Box rightb;

        std::vector<Box> bbxs;

        for (const SceneOp *op : sops) {
                bbxs.emplace_back(op->mat * Box());
        }

        bounding_box.split_box(leftb, rightb, bbxs, split_dim);
        dim = split_dim;

        std::vector<const SceneOp *> sops_left;
        std::vector<const SceneOp *> sops_right;
        std::vector<const SceneOp *> sops_both;

        for (const SceneOp *op : sops) {
                Point p = Point(0, 0, 0);
                p = op->mat * p;
                if (leftb.in_box(p) && rightb.in_box(p)) {
                        sops_both.emplace_back(op);
                } else if (leftb.in_box(p)) {
                        sops_left.emplace_back(op);
                } else if (rightb.in_box(p)) {
                        sops_right.emplace_back(op);
                } else {
                        std::cerr << "Didn't intersect either" << std::endl;
                }
        }

        for (const SceneOp *op : sops_both) {
                sops_left.size() > sops_right.size() ?
                        sops_right.emplace_back(op) :
                        sops_left.emplace_back(op);
        }


        if (sops_left.size() > 0) {
                left = new KDNode(sops_left, depth + 1);
        } else {
                left = nullptr;
        }

        if (sops_right.size() > 0) {
                right = new KDNode(sops_right, depth + 1);
        } else {
                right = nullptr;
        }

        if (left == nullptr && right == nullptr) {
        } else if (left == nullptr && right != nullptr) {
                bounding_box = Box(right->bounding_box.corner,
                                   right->bounding_box.dims);
        } else if (left != nullptr && right == nullptr) {
                bounding_box = Box(left->bounding_box.corner,
                                   left->bounding_box.dims);
        } else {
                bounding_box = Box(left->bounding_box.corner,
                                   left->bounding_box.dims);
                bounding_box.expand_box(right->bounding_box);
        }
}

void KDNode::print_structure(int depth) {
        for (int i = 0; i < depth; ++i)
        {
        }
        for (int i = 0; i < depth; ++i)
        {
        }
        bounding_box.print();
        for (int i = 0; i < depth; ++i)
        {
        }
        if (left != nullptr) {
                left->print_structure(depth+1);
        }
        if (right != nullptr) {
                right->print_structure(depth+1);
        }
}

void KDNode::print() {
        bounding_box.print();
}

static inline bool add_elems(std::vector<const SceneOp *> &intersect_obj, std::vector<const SceneOp *> &sceneops) {
        intersect_obj.insert(std::end(intersect_obj), std::begin(sceneops), std::end(sceneops));
        return true;
}

bool KDNode::intersected(std::vector<const SceneOp *> &intersect_obj,
                         const Point &eye, const Vector &ray) {

        return !bounding_box.intersect_box(eye, ray)
               || ((sceneops.empty() || add_elems(intersect_obj, sceneops))
                   && (left == nullptr
                       || left->intersected(intersect_obj, eye, ray) || true)
                   && (right == nullptr
                       || right->intersected(intersect_obj, eye, ray) || true));
}
