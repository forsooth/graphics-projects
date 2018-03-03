#include "Sphere.h"

Sphere::Sphere() {};

static inline void spherical_v(float r, float θ, float φ) {
        float x, y, z;
        x = 1/r * sin(φ) * cos(θ);
        y = 1/r * cos(φ);
        z = 1/r * sin(φ) * sin(θ);
        glNormal3f(x, y, z);
        x = r * sin(φ) * cos(θ);
        y = r * cos(φ);
        z = r * sin(φ) * sin(θ);
        glVertex3f(x, y, z);
}

static inline void spherical_n(float r, float θ, float φ) {
        float x, y, z;
        x = r * sin(φ) * cos(θ);
        y = r * cos(φ);
        z = r * sin(φ) * sin(θ);
        glVertex3f(x, y, z);
        glVertex3f(NormL * x, NormL * y, NormL * z);
}

void Sphere::draw() {
        glBegin(GL_TRIANGLES);
        float R = 0.5;
        float Δθ = 2 * PI / float(m_segmentsX);
        float θ = 0.0f;
        float Δφ = PI / float(m_segmentsY);
        float φ = 0.0f;

        for (int slice = 0; slice < m_segmentsX; slice++) {
                for (int layer = 0; layer < m_segmentsY; layer++) {
                        spherical_v(R, θ, φ);
                        spherical_v(R, θ + Δθ, φ);
                        spherical_v(R, θ + Δθ, φ + Δφ);
                        spherical_v(R, θ, φ);
                        spherical_v(R, θ, φ + Δφ);
                        spherical_v(R, θ + Δθ, φ + Δφ);
                        φ += Δφ;
                }
                φ = 0;
                θ += Δθ;
        }


        glEnd();

};

void Sphere::drawNormal() {
        glColor3f(1.0, 0.0, 0.0);
        glBegin(GL_LINES);
        float R = 0.5;
        float Δθ = 2 * PI / float(m_segmentsX);
        float θ = 0.0f;
        float Δφ = PI / float(m_segmentsY);
        float φ = 0.0f;

        for (int slice = 0; slice < m_segmentsX; slice++) {
                for (int layer = 0; layer < m_segmentsY; layer++) {
                        spherical_n(R, θ, φ);
                        spherical_n(R, θ + Δθ, φ);
                        spherical_n(R, θ + Δθ, φ + Δφ);
                        spherical_n(R, θ, φ);
                        spherical_n(R, θ, φ + Δφ);
                        spherical_n(R, θ + Δθ, φ + Δφ);
                        φ += Δφ;
                }
                φ = 0;
                θ += Δθ;
        }
        glEnd();
};
