#include "Cylinder.h"

Cylinder::Cylinder() {};

static inline void cylindrical_v(float r, float theta, float y) {
        float x, z;
        x = cos(theta);
        z = sin(theta);
        glNormal3f(x, 0.0, z);
        x = r * cos(theta);
        z = r * sin(theta);
        glVertex3f(x, y, z);
}

static inline void cylindrical_n(float r, float theta, float y) {
        float x, z;
        x = r * cos(theta);
        z = r * sin(theta);
        glVertex3f(x, y, z);
        glVertex3f(NormL * x, y, NormL * z);
}

void Cylinder::draw() {
        glBegin(GL_TRIANGLES);
        float R = 0.5;
        float y = R;
        float thetay = 2 * R / float(m_segmentsY);
        float theta = 0.0f;
        float thetatheta = 2 * PI / float(m_segmentsX);
        for (int slice = 0; slice < m_segmentsX; slice++) {
                glNormal3f(0.0, 1.0, 0.0);
                glVertex3f(R * cos(theta), y, R * sin(theta));
                glVertex3f(0.0, y, 0.0);
                glVertex3f(R * cos(theta + thetatheta), y, R * sin(theta + thetatheta));
                for (int layer = 0; layer < m_segmentsY; layer++) {
                        cylindrical_v(R, theta, y);
                        cylindrical_v(R, theta, y - thetay);
                        cylindrical_v(R, theta + thetatheta, y);
                        cylindrical_v(R, theta + thetatheta, y - thetay);
                        cylindrical_v(R, theta + thetatheta, y);
                        cylindrical_v(R, theta, y - thetay);
                        y -= thetay;
                }
                glNormal3f(0.0, -1.0, 0.0);
                glVertex3f(R * cos(theta), y, R * sin(theta));
                glVertex3f(0.0, y, 0.0);
                glVertex3f(R * cos(theta + thetatheta), y, R * sin(theta + thetatheta));
                theta += thetatheta;
                y = R;
        }
        glEnd();

};

void Cylinder::drawNormal() {
        //glLineWidth();
        glColor3f(1.0, 0.0, 0.0);
        glBegin(GL_LINES);
        float R = 0.5;
        float y = R;
        float thetay = 2 * R / float(m_segmentsY);
        float line_length = 0.02;
        float thetatheta = 2 * PI / float(m_segmentsX);
        float theta = 0.0f;
        glVertex3f(0.0, 0.5, 0.0);
        glVertex3f(0.0, 0.5 + line_length, 0.0);
        for (int pieslice = 0; pieslice < m_segmentsX; pieslice++) {
                for (int layer = 0; layer < m_segmentsY; layer++) {
                        cylindrical_n(R, theta, y);
                        cylindrical_n(R, theta, y - thetay);
                        cylindrical_n(R, theta + thetatheta, y);
                        cylindrical_n(R, theta + thetatheta, y - thetay);
                        y -= thetay;
                }
                glVertex3f(0.0, -0.5, 0.0);
                glVertex3f(0.0, -0.5 - line_length, 0.0);
                theta += thetatheta;
                y = R;
        }
        glEnd();
};
