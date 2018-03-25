#include "Cylinder.h"

Cylinder::Cylinder() {};

static inline void cylindrical_v(float r, float θ, float y) {
        float x, z;
        x = cos(θ);
        z = sin(θ);
        glNormal3f(x, 0.0, z);
        x = r * cos(θ);
        z = r * sin(θ);
        glVertex3f(x, y, z);
}

static inline void cylindrical_n(float r, float θ, float y) {
        float x, z;
        x = r * cos(θ);
        z = r * sin(θ);
        glVertex3f(x, y, z);
        glVertex3f(NormL * x, y, NormL * z);
}

void Cylinder::draw() {
        glBegin(GL_TRIANGLES);
        float R = 0.5;
        float y = R;
        float Δy = 2 * R / float(m_segmentsY);
        float θ = 0.0f;
        float Δθ = 2 * PI / float(m_segmentsX);
        for (int slice = 0; slice < m_segmentsX; slice++) {
                glNormal3f(0.0, 1.0, 0.0);
                glVertex3f(R * cos(θ), y, R * sin(θ));
                glVertex3f(0.0, y, 0.0);
                glVertex3f(R * cos(θ + Δθ), y, R * sin(θ + Δθ));
                for (int layer = 0; layer < m_segmentsY; layer++) {
                        cylindrical_v(R, θ, y);
                        cylindrical_v(R, θ, y - Δy);
                        cylindrical_v(R, θ + Δθ, y);
                        cylindrical_v(R, θ + Δθ, y - Δy);
                        cylindrical_v(R, θ + Δθ, y);
                        cylindrical_v(R, θ, y - Δy);
                        y -= Δy;
                }
                glNormal3f(0.0, -1.0, 0.0);
                glVertex3f(R * cos(θ), y, R * sin(θ));
                glVertex3f(0.0, y, 0.0);
                glVertex3f(R * cos(θ + Δθ), y, R * sin(θ + Δθ));
                θ += Δθ;
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
        float Δy = 2 * R / float(m_segmentsY);
        float line_length = 0.02;
        float Δθ = 2 * PI / float(m_segmentsX);
        float θ = 0.0f;
        glVertex3f(0.0, 0.5, 0.0);
        glVertex3f(0.0, 0.5 + line_length, 0.0);
        for (int pieslice = 0; pieslice < m_segmentsX; pieslice++) {
                for (int layer = 0; layer < m_segmentsY; layer++) {
                        cylindrical_n(R, θ, y);
                        cylindrical_n(R, θ, y - Δy);
                        cylindrical_n(R, θ + Δθ, y);
                        cylindrical_n(R, θ + Δθ, y - Δy);
                        y -= Δy;
                }
                glVertex3f(0.0, -0.5, 0.0);
                glVertex3f(0.0, -0.5 - line_length, 0.0);
                θ += Δθ;
                y = R;
        }
        glEnd();
};
