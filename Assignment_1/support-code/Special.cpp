#include "Special.h"

Special::Special() {};

static inline void cylinrical_v(float r, float θ, float y) {
        float x, z;
        x = cos(θ);
        z = sin(θ);
        glNormal3f(-x, 0.0, -z);
        x = r * cos(θ);
        z = r * sin(θ);
        glVertex3f(x, y, z);
}

static inline void cylindrical_n(float r, float θ, float y) {
        float x, z;
        x = r * cos(θ);
        z = r * sin(θ);
        glVertex3f(x, y, z);
        glVertex3f(x + 2 * sqrt(5)/5 * 0.1 * cos(θ), y + (sqrt(5)/5 * 0.1), z + 2 * sqrt(5)/5 * 0.1 * sin(θ));
}

void Special::draw() {
        glBegin(GL_TRIANGLES);
        float R = 0.5;
        float y = R;
        float Δy = 2 * R / float(m_segmentsY);
        float Δθ = 2 * PI / float(m_segmentsX);
        float θ = 0.0f;
        for (int pieslice = 0; pieslice < m_segmentsX; pieslice++) {
                float r = 0;
                float Δr = R / m_segmentsY;
                for (int layer = 0; layer < m_segmentsY; layer++) {
                        cylinrical_v(r, θ, y);
                        cylinrical_v(r + Δr, θ, y - Δy);
                        cylinrical_v(r, θ + Δθ, y);
                        cylinrical_v(r + Δr, θ + Δθ, y - Δy);
                        cylinrical_v(r, θ + Δθ, y);
                        cylinrical_v(r + Δr, θ, y - Δy);
                        glNormal3f(0.0, 1, 0.0);
                        glVertex3f(r * cos(θ), y, r * sin(θ));
                        glVertex3f(0.0, y, 0.0);
                        glVertex3f(r * cos(θ + Δθ), y, r * sin(θ + Δθ));
                        glNormal3f(0.0, -1, 0.0);
                        glVertex3f((r + Δr) * cos(θ), y - Δy, (r + Δr) * sin(θ));
                        glVertex3f(0.0, y - Δy, 0.0);
                        glVertex3f((r + Δr) * cos(θ + Δθ), y - Δy, (r + Δr) * sin(θ + Δθ));
                        y -= Δy;
                        r -= Δr;
                }
                θ += Δθ;
                y = R;

        }
        glEnd();

};

void Special::drawNormal() {
        glColor3f(1.0, 0.0, 0.0);
        glBegin(GL_LINES);
        float R = 0.5;
        float y = R;
        float Δy = 2 * R / float(m_segmentsY);
        float Δθ = 2 * PI / float(m_segmentsX);
        float θ = 0.0f;
        glVertex3f(0.0, -R, 0.0);
        glVertex3f(0.0, -R - NormL, 0.0);
        for (int pieslice = 0; pieslice < m_segmentsX; pieslice++) {
                float r = 0;
                float Δr = R / m_segmentsY;
                for (int layer = 0; layer < m_segmentsY; layer++) {
                        if (r != 0) {
                                cylindrical_n(r, θ, y);
                                cylindrical_n(r, θ + Δθ, y);
                        }
                        y -= Δy;
                        r += Δr;
                }
                θ += Δθ;
                y = R;
        }
        glEnd();
};
