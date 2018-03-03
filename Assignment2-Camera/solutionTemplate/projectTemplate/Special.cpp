#include "Special.h"

Special::Special() {};

static inline void cylinrical_v(float r, float theta, float y) {
        float x, z;
        x = cos(theta);
        z = sin(theta);
        glNormal3f(-x, 0.0, -z);
        x = r * cos(theta);
        z = r * sin(theta);
        glVertex3f(x, y, z);
}

static inline void cylindrical_n(float r, float theta, float y) {
        float x, z;
        x = r * cos(theta);
        z = r * sin(theta);
        glVertex3f(x, y, z);
        glVertex3f(x + 2 * sqrt(5)/5 * 0.1 * cos(theta), y + (sqrt(5)/5 * 0.1), z + 2 * sqrt(5)/5 * 0.1 * sin(theta));
}

void Special::draw() {
        glBegin(GL_TRIANGLES);
        float R = 0.5;
        float y = R;
        float deltay = 2 * R / float(m_segmentsY);
        float deltatheta = 2 * PI / float(m_segmentsX);
        float theta = 0.0f;
        for (int pieslice = 0; pieslice < m_segmentsX; pieslice++) {
                float r = 0;
                float deltar = R / m_segmentsY;
                for (int layer = 0; layer < m_segmentsY; layer++) {
                        cylinrical_v(r, theta, y);
                        cylinrical_v(r + deltar, theta, y - deltay);
                        cylinrical_v(r, theta + deltatheta, y);
                        cylinrical_v(r + deltar, theta + deltatheta, y - deltay);
                        cylinrical_v(r, theta + deltatheta, y);
                        cylinrical_v(r + deltar, theta, y - deltay);
                        glNormal3f(0.0, 1, 0.0);
                        glVertex3f(r * cos(theta), y, r * sin(theta));
                        glVertex3f(0.0, y, 0.0);
                        glVertex3f(r * cos(theta + deltatheta), y, r * sin(theta + deltatheta));
                        glNormal3f(0.0, -1, 0.0);
                        glVertex3f((r + deltar) * cos(theta), y - deltay, (r + deltar) * sin(theta));
                        glVertex3f(0.0, y - deltay, 0.0);
                        glVertex3f((r + deltar) * cos(theta + deltatheta), y - deltay, (r + deltar) * sin(theta + deltatheta));
                        y -= deltay;
                        r -= deltar;
                }
                theta += deltatheta;
                y = R;

        }
        glEnd();

};

void Special::drawNormal() {
        glColor3f(1.0, 0.0, 0.0);
        glBegin(GL_LINES);
        float R = 0.5;
        float y = R;
        float deltay = 2 * R / float(m_segmentsY);
        float deltatheta = 2 * PI / float(m_segmentsX);
        float theta = 0.0f;
        glVertex3f(0.0, -R, 0.0);
        glVertex3f(0.0, -R - NormL, 0.0);
        for (int pieslice = 0; pieslice < m_segmentsX; pieslice++) {
                float r = 0;
                float deltar = R / m_segmentsY;
                for (int layer = 0; layer < m_segmentsY; layer++) {
                        if (r != 0) {
                                cylindrical_n(r, theta, y);
                                cylindrical_n(r, theta + deltatheta, y);
                        }
                        y -= deltay;
                        r += deltar;
                }
                theta += deltatheta;
                y = R;
        }
        glEnd();
};
