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

        real mins[2];
        real *ts = (real *)mins;
        real *end = ts+1;

        real zdiv = eye[2] / ray[2];
        real rdiv = R / ray[2];
        real tzp = rdiv - zdiv;
        real xside = eye[0] + ray[0] * tzp;
        real yside = eye[1] + ray[1] * tzp;
        if (fabs(xside) <= R &&
            fabs(yside) <= R) {
                ts[0] = tzp;
                ts++;
        }

        real tzn = -rdiv - zdiv;
        xside = eye[0] + ray[0] * tzn;
        yside = eye[1] + ray[1] * tzn;
        if (fabs(xside) <= R &&
            fabs(yside) <= R) {
                ts[0] = tzn;
                ts++;
        }

        if (ts > end) {
                return fmin(mins[0], mins[1]);
        }

        real xdiv = eye[0] / ray[0];
        rdiv = R / ray[0];
        real txp = rdiv - xdiv;
        yside = eye[1] + ray[1] * txp;
        real zside = eye[2] + ray[2] * txp;
        if (fabs(yside) <= R &&
            fabs(zside) <= R) {
                ts[0] = txp;
                ts++;
        }

        if (ts > end) {
                return fmin(mins[0], mins[1]);
        }

        real txn = -rdiv - xdiv;
        yside = eye[1] + ray[1] * txn;
        zside = eye[2] + ray[2] * txn;
        if (fabs(yside) <= R &&
            fabs(zside) <= R) {
                ts[0] = txn;
                ts++;
        }

        if (ts > end) {
                return fmin(mins[0], mins[1]);
        }

        real ydiv = eye[1] / ray[1];
        rdiv = R / ray[1];
        real typ = rdiv - ydiv;
        xside = eye[0] + ray[0] * typ;
        zside = eye[2] + ray[2] * typ;
        if (fabs(xside) <= R &&
            fabs(zside) <= R) {
                ts[0] = typ;
                ts++;
        }

        if (ts > end) {
                return fmin(mins[0], mins[1]);
        }

        real tyn = -rdiv - ydiv;
        xside = eye[0] + ray[0] * tyn;
        zside = eye[2] + ray[2] * tyn;
        if (fabs(xside) <= R &&
            fabs(zside) <= R) {
                ts[0] = tyn;
                ts++;
        }

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

         isect[0] = (int)(!(isect.x() <= (R - ε) && isect.x() >= (-R + ε))) * isect[0];
         isect[1] = (int)(!(isect.y() <= (R - ε) && isect.y() >= (-R + ε))) * isect[1];
         isect[2] = (int)(!(isect.z() <= (R - ε) && isect.z() >= (-R + ε))) * isect[2];

        isect.normalize();
        return isect;
}

