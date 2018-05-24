#ifndef BOX_H
#define BOX_H

#include "Algebra.hpp"
#include <iostream>
#include <limits>
#include <vector>
#include <algorithm>

class Box
{
public:
        Box();

        Box(Point corner_point, Vector dimensions);

        void expand_box(Box b);

        void expand_box(Point p);

        bool in_box(Point p);

        bool intersect_box(Box b);

        bool intersect_box(const Point &eye, const Vector &ray);

        /* Splits box along largest dim and makes left and right be the resulting boxes */
        void split_box(Box &left, Box &right, std::vector<Box> bbxs, unsigned int split_dim);

        Point corner;
        /* each dim should always be positive */
        Vector dims;
        int largest_dim;
        void print();
        bool squareIntersect(const Point &eye, const Vector &ray,
                        int dimension, real side);
        Box operator*(const Matrix& m);
        int get_largest_dim();
private:

};

inline Box operator*(const Matrix& m, const Box& b) {
        Box d;

        Point ps[8];
        ps[0] = b.corner;
        ps[1] = b.corner + Vector(b.dims[0], 0, 0);
        ps[2] = b.corner + Vector(0, b.dims[1], 0);
        ps[3] = b.corner + Vector(0, 0, b.dims[2]);
        ps[4] = b.corner + Vector(b.dims[0], b.dims[1], 0);
        ps[5] = b.corner + Vector(0, b.dims[1], b.dims[2]);
        ps[6] = b.corner + Vector(b.dims[0], 0, b.dims[2]);
        ps[7] = b.corner + Vector(b.dims[0], b.dims[1], b.dims[2]);

        ps[0] = m * ps[0];
        ps[1] = m * ps[1];
        ps[2] = m * ps[2];
        ps[3] = m * ps[3];
        ps[4] = m * ps[4];
        ps[5] = m * ps[5];
        ps[6] = m * ps[6];
        ps[7] = m * ps[7];

        float xmin = std::numeric_limits<float>::max(),
               ymin =  std::numeric_limits<float>::max(),
               zmin = std::numeric_limits<float>::max();
        float xmax = std::numeric_limits<float>::lowest(),
               ymax = std::numeric_limits<float>::lowest(),
               zmax = std::numeric_limits<float>::lowest();

        for (int i = 0; i < 8; i++) {
                if (ps[i][0] < xmin) {
                    xmin = ps[i][0];
                }
                if (ps[i][1] < ymin) {
                    ymin = ps[i][1];
                }
                if (ps[i][2] < zmin) {
                    zmin = ps[i][2];
                }
                if (ps[i][0] > xmax) {
                    xmax = ps[i][0];
                }
                if (ps[i][1] > ymax) {
                    ymax = ps[i][1];
                }
                if (ps[i][2] > zmax) {
                    zmax = ps[i][2];
                }
        }

        d.largest_dim = d.get_largest_dim();

        d.corner = Point(xmin, ymin, zmin);
        d.dims = Vector(xmax - xmin, ymax - ymin, zmax - zmin);
        return d;
};

#endif
