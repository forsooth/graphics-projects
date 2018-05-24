#include "Box.hpp"
#include <algorithm>
#include <vector>

Box::Box() {
        corner = Point(-0.5, -0.5, -0.5);
        dims = Vector(1, 1, 1);
}

Box::Box(Point corner_point, Vector dimensions) {
        corner = corner_point;
        dims = dimensions;
}

void Box::expand_box(Box b) {
        std::vector<real> xs;
        std::vector<real> ys;
        std::vector<real> zs;

        xs.emplace_back(this->corner[0]);
        xs.emplace_back(this->dims[0] + this->corner[0]);
        xs.emplace_back(b.corner[0]);
        xs.emplace_back(b.dims[0] + b.corner[0]);

        ys.emplace_back(this->corner[1]);
        ys.emplace_back(this->dims[1] + this->corner[1]);
        ys.emplace_back(b.corner[1]);
        ys.emplace_back(b.dims[1] + b.corner[1]);

        zs.emplace_back(this->corner[2]);
        zs.emplace_back(this->dims[2] + this->corner[2]);
        zs.emplace_back(b.corner[2]);
        zs.emplace_back(b.dims[2] + b.corner[2]);

        this->corner[0] = *std::min_element(std::begin(xs), std::end(xs));
        this->dims[0]   = *std::max_element(std::begin(xs), std::end(xs)) - this->corner[0];
        this->corner[1] = *std::min_element(std::begin(ys), std::end(ys));
        this->dims[1]   = *std::max_element(std::begin(ys), std::end(ys)) - this->corner[1];
        this->corner[2] = *std::min_element(std::begin(zs), std::end(zs));
        this->dims[2]   = *std::max_element(std::begin(zs), std::end(zs)) - this->corner[2];

        largest_dim = get_largest_dim();

}

void Box::expand_box(Point p) {
        if (!in_box(p)) {
                for (int i = 0; i < 3; ++i) {
                        if (p[i] - corner[i] < 0) {
                                // p is to the left of corner shift corner to left
                                corner[i] = p[i];
                        } else if (p[i] - corner[i] > dims[i]) {
                                //p is to the right of corner and corner+dims, increase dims
                                dims[i] = p[i] - corner[i];
                        }
                }
                largest_dim = get_largest_dim();
        }

}

bool Box::in_box(Point p) {
        return  (p[0] - corner[0]) <= dims[0] && 0 <= (p[0] - corner[0]) &&
                (p[1] - corner[1]) <= dims[1] && 0 <= (p[1] - corner[1]) &&
                (p[2] - corner[2]) <= dims[2] && 0 <= (p[2] - corner[2]);
}

bool Box::squareIntersect(const Point &eye, const Vector &ray,
                          int dimension, real side) {
        //dimension: 0 1 2
        //     dim1: 1 2 0
        //     dim2: 2 0 1

        //int dim1 = (dimension + 1) % 3;
        //int dim2 = (dimension + 2) % 3;

        static int dim1a[3] = {1, 2, 0};
        static int dim2a[3] = {2, 0, 1};
        int dim1 = dim1a[dimension];
        int dim2 = dim2a[dimension];

        return (ray[dimension] != 0.0f)
               && (side - eye[dimension]) / ray[dimension] >= 0
               && eye[dim1] + ray[dim1] * ((side) - eye[dimension]) / ray[dimension] >= corner[dim1]
               && eye[dim2] + ray[dim2] * ((side) - eye[dimension]) / ray[dimension] >= corner[dim2]
               && eye[dim1] + ray[dim1] * ((side) - eye[dimension]) / ray[dimension] <= corner[dim1] + dims[dim1]
               && eye[dim2] + ray[dim2] * ((side) - eye[dimension]) / ray[dimension] <= corner[dim2] + dims[dim2];
}

bool Box::intersect_box(Box b) {
#define RANGE(a, b, x) (((a) <= (x)) && ((x) <= (b)))

//#define INTERVAL_INTERSECT(a, b, c, d)\
//    (RANGE((a), (b), (c)) || RANGE((a), (b), (d)) || \
//     RANGE((c), (d), (a)) || RANGE((c), (d), (b)))
#define INTERVAL_INTERSECT(a, b, c, d) (((a) <= (c)) && ((b) <= (d)))

        bool acc = true;

        real x_a0 = this->corner[0];
        real x_a1 = this->dims[0] + this->corner[0];
        real x_b0 = b.corner[0];
        real x_b1 = b.dims[0] + b.corner[0];
        acc = acc && INTERVAL_INTERSECT(x_a0, x_a1, x_b0, x_b1);
        if (!acc) return false;

        real y_a0 = this->corner[1];
        real y_a1 = this->dims[1] + this->corner[1];
        real y_b0 = b.corner[1];
        real y_b1 = b.dims[1] + b.corner[1];
        acc = acc && INTERVAL_INTERSECT(y_a0, y_a1, y_b0, y_b1);
        if (!acc) return false;

        real z_a0 = this->corner[2];
        real z_a1 = this->dims[2] + this->corner[2];
        real z_b0 = b.corner[2];
        real z_b1 = b.dims[2] + b.corner[2];

        return acc && INTERVAL_INTERSECT(z_a0, z_a1, z_b0, z_b1);
}

bool Box::intersect_box(const Point &eye, const Vector &ray) {

        return  this->squareIntersect(eye, ray, 0, corner[0]) ||
                this->squareIntersect(eye, ray, 1, corner[1]) ||
                this->squareIntersect(eye, ray, 2, corner[2]) ||
                this->squareIntersect(eye, ray, 0, corner[0] + dims[0]) ||
                this->squareIntersect(eye, ray, 1, corner[1] + dims[1]) ||
                this->squareIntersect(eye, ray, 2, corner[2] + dims[2]);
}

int Box::get_largest_dim() {
        if (dims[0] >= dims[1] && dims[0] >= dims[2]) {
                return 0;
        } else if (dims[1] >= dims[0] && dims[1] >= dims[2]) {
                return 1;
        } else if (dims[2] >= dims[0] && dims[2] >= dims[1]) {
                return 2;
        } else {
                return 0;
        }
}

void Box::split_box(Box &left, Box &right, std::vector<Box> bbxs, unsigned int split_dim) {
        std::vector<real> corner_vals;
        for (Box b : bbxs) {
                corner_vals.emplace_back(b.corner[split_dim]);
        }

        std::nth_element(corner_vals.begin(),
                         corner_vals.begin() + corner_vals.size() / 2,
                         corner_vals.end());

        real median;
        if (corner_vals.size() % 2 == 1) {
                median = corner_vals[corner_vals.size() / 2];
        } else {
                real elem1 = corner_vals[corner_vals.size() / 2];
                real elem2 = corner_vals[corner_vals.size() / 2 + 1];
                median = (elem1 + elem2) / 2.0f;
        }

        real c1 = this->corner[split_dim];
        real c2 = median;
        real d1 = c2 - c1;
        real d2 = (c1 + this->dims[split_dim]) - c2;

        left.corner = this->corner;
        right.corner = this->corner;
        right.corner[split_dim] = c2;

        left.dims = this->dims;
        right.dims = this->dims;
        left.dims[split_dim] = d1;
        right.dims[split_dim] = d2;
}

void Box::print() {
        std::cerr << "(" << this->corner[0] << " + "<< this->dims[0] << ", "
                         << this->corner[1] << " + "<< this->dims[1] << ", "
                         << this->corner[2] << " + "<< this->dims[2] << ")"
                         << std::endl;
};
