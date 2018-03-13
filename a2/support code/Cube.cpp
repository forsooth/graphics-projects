#include "Cube.h"

Cube::Cube() {};

static inline void draw_face(float xorig, float yorig, float zorig,
                             float Δx, float Δy, float Δz_r, float Δz_c,
                             float xs, float ys, float zs) {
        float x = xorig, y = yorig, z = zorig;
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

static inline void draw_normal_face(float xorig, float yorig, float zorig,
                             float Δx, float Δy, float Δz_r, float Δz_c,
                             float xs, float ys, float zs,
                             float xscale, float yscale, float zscale) {
        float x = xorig, y = yorig, z = zorig;
        for (int cur_y = 0; cur_y < ys; cur_y++) {
                for (int cur_z = 0; cur_z < zs; cur_z++) {
                        for (int cur_x = 0; cur_x < xs; cur_x++) {
                                glVertex3f(x, y, z);
                                glVertex3f(xscale * x, yscale * y, zscale * z);
                                glVertex3f(x + Δx, y, z + Δz_c);
                                glVertex3f(xscale * (x + Δx), yscale * y, zscale * (z + Δz_c));
                                glVertex3f(x, y + Δy, z + Δz_r);
                                glVertex3f(xscale * x, yscale * (y + Δy), zscale * (z + Δz_r));
                                glVertex3f(x + Δx, y + Δy, z + Δz_r + Δz_c);
                                glVertex3f(xscale * (x + Δx), yscale * (y + Δy), zscale * (z + Δz_r + Δz_c));
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
        float R = 0.5;
        float drow = 1.0 / m_segmentsX;
        float dcol = 1.0 / m_segmentsY;
        float rows = m_segmentsX;
        float cols = m_segmentsY;
        //         X   Y   Z     DX     DY    DZR    DZC
        glNormal3f(0, 0, 1);
        draw_face(-R,  R,  R,  drow, -dcol,     0,     0, rows, cols, 1); /* +XY */
        glNormal3f(0, 0, -1);
        draw_face( R,  R, -R, -drow, -dcol,     0,     0, rows, cols, 1); /* -XY */
        glNormal3f(0, 1, 0);
        draw_face(-R,  R, -R,  dcol,     0,  drow,     0, cols, 1, rows); /* +XZ */
        glNormal3f(0, -1, 0);
        draw_face(-R, -R,  R,  dcol,     0, -drow,     0, cols, 1, rows); /* -XZ */
        glNormal3f(1, 0, 0);
        draw_face( R,  R,  R,     0, -drow,     0, -dcol, 1, rows, cols); /* +YZ */
        glNormal3f(-1, 0, 0);
        draw_face(-R,  R, -R,     0, -drow,     0,  dcol, 1, rows, cols); /* -YZ */
        glEnd();

};

void Cube::drawNormal() {
        glColor3f(1.0, 0.0, 0.0);
        glBegin(GL_LINES);
        float R = 0.5;
        float drow = 1.0 / m_segmentsX;
        float dcol = 1.0 / m_segmentsY;
        float rows = m_segmentsX;
        float cols = m_segmentsY;
        draw_normal_face(-R,  R,  R,  drow, -dcol,     0,     0, rows, cols, 1, 1, 1, NormL); /* +XY */
        draw_normal_face( R,  R, -R, -drow, -dcol,     0,     0, rows, cols, 1, 1, 1, NormL); /* -XY */
        draw_normal_face(-R,  R, -R,  dcol,     0,  drow,     0, cols, 1, rows, 1, NormL, 1); /* +XZ */
        draw_normal_face(-R, -R,  R,  dcol,     0, -drow,     0, cols, 1, rows, 1, NormL, 1); /* -XZ */
        draw_normal_face( R,  R,  R,     0, -drow,     0, -dcol, 1, rows, cols, NormL, 1, 1); /* +YZ */
        draw_normal_face(-R,  R, -R,     0, -drow,     0,  dcol, 1, rows, cols, NormL, 1, 1); /* -YZ */
        glEnd();
};

