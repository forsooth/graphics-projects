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
        int dim1 = dimension == 0 ? 1 : dimension == 1 ? 2 : 1;
        int dim2 = dimension == 0 ? 2 : 0;
        if (ray[dimension] != 0.0f) {
                real t = (side - eye[dimension]) / ray[dimension];
                Point coords = eye + ray * t;
                if (t >= 0 && coords[dim1] <= corner[dim1] + dims[dim1] && coords[dim1] >= corner[dim1] &&
                        coords[dim2] <= corner[dim2] + dims[dim2] && coords[dim2] >= corner[dim2]) {
                        return true;
                } else {
                        return false;
                }
        }

        return false;
}

bool Box::intersect_box(Box b) {
#define RANGE(a, b, x) (((a) <= (x)) && ((x) <= (b)))

#define INTERVAL_INTERSECT(a, b, c, d)\
    (RANGE((a), (b), (c)) || RANGE((a), (b), (d)) || \
    RANGE((c), (d), (a)) || RANGE((c), (d), (b)))

        real x_a0 = this->corner[0];
        real x_a1 = this->dims[0] + this->corner[0];

        real y_a0 = this->corner[1];
        real y_a1 = this->dims[1] + this->corner[1];
        
        real z_a0 = this->corner[2];
        real z_a1 = this->dims[2] + this->corner[2];

        real x_b0 = b.corner[0];
        real x_b1 = b.dims[0] + b.corner[0];

        real y_b0 = b.corner[1];
        real y_b1 = b.dims[1] + b.corner[1];
        
        real z_b0 = b.corner[2];
        real z_b1 = b.dims[2] + b.corner[2];

        return  INTERVAL_INTERSECT(x_a0, x_a1, x_b0, x_b1) &&
                INTERVAL_INTERSECT(y_a0, y_a1, y_b0, y_b1) &&
                INTERVAL_INTERSECT(z_a0, z_a1, z_b0, z_b1);
}

bool Box::intersect_box(Point eye, Vector ray) {
        Point center = corner + (1.0f / 2.0f) * dims;
        Vector veye = center - eye;
        Vector vray = normalize(ray);
        real dotprod = dot(veye, vray);

        Vector eyeprojray = veye - dotprod * vray;
        real dist = SQR((eyeprojray[0])) + SQR((eyeprojray[1])) + SQR((eyeprojray[2]));
        if (dist > SQR(dims[largest_dim])) {
                return false;
        }

        return  this->squareIntersect(eye, ray, 0, corner[0]) ||
                this->squareIntersect(eye, ray, 1, corner[1]) ||
                this->squareIntersect(eye, ray, 2, corner[2]) ||
                this->squareIntersect(eye, ray, 0, corner[0] + dims[0]) ||
                this->squareIntersect(eye, ray, 1, corner[1] + dims[1]) ||
                this->squareIntersect(eye, ray, 2, corner[2] + dims[2]);
}

//bool Box::intersect_box(Point eye, Vector ray) {
//         Vector veye = eye - (this->corner + (1.0f / 2.0f) * this->dims);
//         eye[0] = veye[0];
//         eye[1] = veye[1];
//         eye[2] = veye[2];
//         Vector vray = normalize(ray);
//         real dotprod = dot(veye, vray);
//
//
//        Vector closest = veye - dotprod * vray;
//        real dist = SQR((closest[0])) + SQR((closest[1])) + SQR((closest[2]));
//        if (dist > SQR(this->dims[largest_dim])) {
//                return std::numeric_limits<real>::max();
//        }
//
//         real zdiv = eye[2] / ray[2];
//         real rdiv = this->dims[2] / ray[2];
//         real tzp = rdiv - zdiv;
//         real xside = eye[0] + ray[0] * tzp;
//         real yside = eye[1] + ray[1] * tzp;
//         if (fabs(xside) <= this->dims[0] &&
//             fabs(yside) <= this->dims[1]) {
//                 return true;
//         }
//
//         real tzn = -rdiv - zdiv;
//         xside = eye[0] + ray[0] * tzn;
//         yside = eye[1] + ray[1] * tzn;
//         if (fabs(xside) <= this->dims[0] &&
//             fabs(yside) <= this->dims[1]) {
//                 return true;
//         }
//
//         real xdiv = eye[0] / ray[0];
//         rdiv = this->dims[0] / ray[0];
//         real txp = rdiv - xdiv;
//         yside = eye[1] + ray[1] * txp;
//         real zside = eye[2] + ray[2] * txp;
//         if (fabs(yside) <= this->dims[1] &&
//             fabs(zside) <= this->dims[2]) {
//                 return true;
//         }
//
//         real txn = -rdiv - xdiv;
//         yside = eye[1] + ray[1] * txn;
//         zside = eye[2] + ray[2] * txn;
//         if (fabs(yside) <= this->dims[1] &&
//             fabs(zside) <= this->dims[2]) {
//                 return true;
//         }
//
//         real ydiv = eye[1] / ray[1];
//         rdiv = this->dims[1] / ray[1];
//         real typ = rdiv - ydiv;
//         xside = eye[0] + ray[0] * typ;
//         zside = eye[2] + ray[2] * typ;
//         if (fabs(xside) <= this->dims[0] &&
//             fabs(zside) <= this->dims[2]) {
//                 return true;
//         }
//
//         real tyn = -rdiv - ydiv;
//         xside = eye[0] + ray[0] * tyn;
//         zside = eye[2] + ray[2] * tyn;
//         if (fabs(xside) <= this->dims[0] &&
//             fabs(zside) <= this->dims[2]) {
//                 return true;
//         }
//
//         return false;
// }


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
