#include "Sphere.h"

Sphere::Sphere() {};

static inline void spherical_v(float r, float theta, float phi) {
        float x, y, z;
        x = 1/r * sin(phi) * cos(theta);
        y = 1/r * cos(phi);
        z = 1/r * sin(phi) * sin(theta);
        glNormal3f(x, y, z);
        x = r * sin(phi) * cos(theta);
        y = r * cos(phi);
        z = r * sin(phi) * sin(theta);
        glVertex3f(x, y, z);
}

static inline void spherical_n(float r, float theta, float phi) {
        float x, y, z;
        x = r * sin(phi) * cos(theta);
        y = r * cos(phi);
        z = r * sin(phi) * sin(theta);
        glVertex3f(x, y, z);
        glVertex3f(NormL * x, NormL * y, NormL * z);
}

void Sphere::draw() {
        glBegin(GL_TRIANGLES);
        float R = 0.5;
        float thetatheta = 2 * PI / float(m_segmentsX);
        float theta = 0.0f;
        float thetaphi = PI / float(m_segmentsY);
        float phi = 0.0f;

        for (int slice = 0; slice < m_segmentsX; slice++) {
                for (int layer = 0; layer < m_segmentsY; layer++) {
                        spherical_v(R, theta, phi);
                        spherical_v(R, theta + thetatheta, phi);
                        spherical_v(R, theta + thetatheta, phi + thetaphi);
                        spherical_v(R, theta, phi);
                        spherical_v(R, theta, phi + thetaphi);
                        spherical_v(R, theta + thetatheta, phi + thetaphi);
                        phi += thetaphi;
                }
                phi = 0;
                theta += thetatheta;
        }


        glEnd();

};

void Sphere::drawNormal() {
        glColor3f(1.0, 0.0, 0.0);
        glBegin(GL_LINES);
        float R = 0.5;
        float thetatheta = 2 * PI / float(m_segmentsX);
        float theta = 0.0f;
        float thetaphi = PI / float(m_segmentsY);
        float phi = 0.0f;

        for (int slice = 0; slice < m_segmentsX; slice++) {
                for (int layer = 0; layer < m_segmentsY; layer++) {
                        spherical_n(R, theta, phi);
                        spherical_n(R, theta + thetatheta, phi);
                        spherical_n(R, theta + thetatheta, phi + thetaphi);
                        spherical_n(R, theta, phi);
                        spherical_n(R, theta, phi + thetaphi);
                        spherical_n(R, theta + thetatheta, phi + thetaphi);
                        phi += thetaphi;
                }
                phi = 0;
                theta += thetatheta;
        }
        glEnd();
};
