#include "Cube.hpp"

#define R 0.5f

Cube::Cube() { type = SHAPE_CUBE; };

static inline void draw_face(real xorig, real yorig, real zorig,
                             real Δx, real Δy, real Δz_r, real Δz_c,
                             real xs, real ys, real zs) {
        real x = xorig, y = yorig, z = zorig;
        for (int cur_y = 0; cur_y < ys; cur_y++) {
                for (int cur_z = 0; cur_z < zs; cur_z++) {
                        for (int cur_x = 0; cur_x < xs; cur_x++) {
                                glVertex3f(x, y, z);
                                glVertex3f(x + Δx, y, z + Δz_c);
                                glVertex3f(x, y + Δy, z + Δz_r);

                                glVertex3f(x, y + Δy, z + Δz_r);
                                glVertex3f(x + Δx, y, z + Δz_c);
                                glVertex3f(x + Δx, y + Δy, z + Δz_r + Δz_c);
                                x += Δx;
                        }
                        x = xorig;
                        z += Δz_r + Δz_c;
                }
                z = zorig;
                y += Δy;
        }
}

static inline void draw_norm_face(real xorig, real yorig, real zorig,
                             real Δx, real Δy, real Δz_r, real Δz_c,
                             real xs, real ys, real zs,
                             real xscale, real yscale, real zscale) {
        real x = xorig, y = yorig, z = zorig;
        for (int cur_y = 0; cur_y < ys; cur_y++) {
                for (int cur_z = 0; cur_z < zs; cur_z++) {
                        for (int cur_x = 0; cur_x < xs; cur_x++) {
                                glVertex3f(x, y, z);
                                glVertex3f(xscale * x, yscale * y, zscale * z);
                                glVertex3f(x + Δx, y, z + Δz_c);
                                glVertex3f(xscale * (x + Δx), yscale * y,
                                           zscale * (z + Δz_c));
                                glVertex3f(x, y + Δy, z + Δz_r);
                                glVertex3f(xscale * x, yscale * (y + Δy),
                                           zscale * (z + Δz_r));
                                glVertex3f(x + Δx, y + Δy, z + Δz_r + Δz_c);
                                glVertex3f(xscale * (x + Δx),
                                           yscale * (y + Δy),
                                           zscale * (z + Δz_r + Δz_c));
                                x += Δx;
                        }
                        x = xorig;
                        z += Δz_r + Δz_c;
                }
                z = zorig;
                y += Δy;
        }
}


Point Cube::to_unit_plane(const Point &isect) {
        real u, v;

        if (fabs(isect[0]) > (R - 1e-3)) {
                u = -isect[2];
                v = -isect[1];
        } else if (fabs(isect[1]) > (R - 1e-3)) {
                u = isect[0];
                v = -isect[2];
        } else {
                u = isect[0];
                v = -isect[1];
        }

        u += 0.5;
        v += 0.5;

        u = fabs(u);
        v = fabs(v);

        return Point(u, v, 0);
};

void Cube::draw() {
        glBegin(GL_TRIANGLES);
        real drow = 1.0f / m_segmentsX;
        real dcol = 1.0f / m_segmentsY;
        real rows = m_segmentsX;
        real cols = m_segmentsY;
        //         X   Y   Z     DX     DY    DZR    DZC
        glNormal3f(0, 0, 1);
         /* +XY */
        draw_face(-R,  R,  R,  drow, -dcol,     0,     0, rows, cols, 1);
        glNormal3f(0, 0, -1);
         /* -XY */
        draw_face( R,  R, -R, -drow, -dcol,     0,     0, rows, cols, 1);
        glNormal3f(0, 1, 0);
        /* +XZ */
        draw_face(-R,  R, -R,  dcol,     0,  drow,     0, cols, 1, rows);
        glNormal3f(0, -1, 0);
         /* -XZ */
        draw_face(-R, -R,  R,  dcol,     0, -drow,     0, cols, 1, rows);
        glNormal3f(1, 0, 0);
         /* +YZ */
        draw_face( R,  R,  R,     0, -drow,     0, -dcol, 1, rows, cols);
        glNormal3f(-1, 0, 0);
         /* -YZ */
        draw_face(-R,  R, -R,     0, -drow,     0,  dcol, 1, rows, cols);
        glEnd();

};

void Cube::drawNormal() {
        glColor3f(1.0f, 0.0f, 0.0f);
        glBegin(GL_LINES);
        real drow = 1.0f / m_segmentsX;
        real dcol = 1.0f / m_segmentsY;
        real rows = m_segmentsX;
        real cols = m_segmentsY;
        real N = NormL;
        /* +XY */
        draw_norm_face(-R, R, R, drow, -dcol, 0, 0, rows, cols, 1, 1, 1, N);
        /* -XY */
        draw_norm_face( R, R, -R, -drow, -dcol, 0, 0, rows, cols, 1, 1, 1, N);
        /* +XZ */
        draw_norm_face(-R, R, -R, dcol, 0,  drow, 0, cols, 1, rows, 1, N, 1);
        /* -XZ */
        draw_norm_face(-R, -R, R, dcol, 0, -drow, 0, cols, 1, rows, 1, N, 1);
        /* +YZ */
        draw_norm_face( R, R, R, 0, -drow, 0, -dcol, 1, rows, cols, N, 1, 1);
        /* -YZ */
        draw_norm_face(-R, R, -R, 0, -drow, 0, dcol, 1, rows, cols, N, 1, 1);
        glEnd();
};

real Cube::Intersect(const Point eye, const Vector ray) {
        Vector veye = Vector(-eye);
        Vector vray = normalize(ray);
        real dotprod = dot(veye, vray);

        Vector closest = veye - dotprod * vray;
        real dist = SQR((closest[0])) + SQR((closest[1])) + SQR((closest[2]));
        if (dist > 0.75f) {
                return std::numeric_limits<real>::max();
        }

        real mins[2] = {0,0};
        real *ts = (real *)mins;
        real *end = ts+1;

        real zdiv = eye[2] / ray[2];
        real rdiv = R / ray[2];
        real tzp = rdiv - zdiv;
        real xside = (eye + ray * tzp)[0];
        real yside = (eye + ray * tzp)[1];
        bool test1 = (xside <= R && xside >= -R && yside <= R && yside >= -R);
        ts[0] += static_cast<real>(test1) * tzp;
        ts += test1;

        real tzn = -rdiv - zdiv;
        xside = (eye + ray * tzn)[0];
        yside = (eye + ray * tzn)[1];
        bool test2 = (xside <= R && xside >= -R && yside <= R && yside >= -R);
        ts[0] += static_cast<real>(test2) * tzn;
        ts += test2;

        if (ts > end) {
                return fmin(mins[0], mins[1]);
        }

        real xdiv = eye[0] / ray[0];
        rdiv = R / ray[0];
        real txp = rdiv - xdiv;
        yside      = (eye + ray * txp)[1];
        real zside = (eye + ray * txp)[2];
        bool test3 = (yside <= R && yside >= -R && zside <= R && zside >= -R);
        ts[0] += static_cast<real>(test3) * txp;
        ts += test3;

        if (ts > end) {
                return fmin(mins[0], mins[1]);
        }

        real txn = -rdiv - xdiv;
        yside = (eye + ray * txn)[1];
        zside = (eye + ray * txn)[2];
        bool test4 = (yside <= R && yside >= -R && zside <= R && zside >= -R);
        ts[0] += static_cast<real>(test4) * txn;
        ts += test4;

        if (ts > end) {
                return fmin(mins[0], mins[1]);
        }

        real ydiv = eye[1] / ray[1];
        rdiv = R / ray[1];
        real typ = rdiv - ydiv;
        xside = (eye + ray * typ)[0];
        zside = (eye + ray * typ)[2];
        bool test5 = (xside <= R && xside >= -R && zside <= R && zside >= -R);
        ts[0] += static_cast<real>(test5) * typ;
        ts += test5;

        if (ts > end) {
                return fmin(mins[0], mins[1]);
        }

        real tyn = -rdiv - ydiv;
        xside = (eye + ray * tyn)[0];
        zside = (eye + ray * tyn)[2];
        bool test6 = (xside <= R && xside >= -R && zside <= R && zside >= -R);
        ts[0] += static_cast<real>(test6) * tyn;
        ts += test6;

        if (ts > end) {
                return fmin(mins[0], mins[1]);
        } else if (ts == mins + 1) {
                return mins[0];
        }

        return std::numeric_limits<real>::max();
}

Vector Cube::findIsectNormal(const Point eye, const Vector ray, real t) {
        Vector isect = eye + (ray * t);
        real ε = 1e-4f;

        isect[0] = (int)(isect.x() > (R - ε) || isect.x() < (-R + ε)) * isect[0];
        isect[1] = (int)(isect.y() > (R - ε) || isect.y() < (-R + ε)) * isect[1];
        isect[2] = (int)(isect.z() > (R - ε) || isect.z() < (-R + ε)) * isect[2];

        isect.normalize();
        return isect;
}

