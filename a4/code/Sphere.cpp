#include "Sphere.hpp"

#define R 0.5f

Sphere::Sphere() { type = SHAPE_SPHERE; };

static inline void spherical_v(real r, real θ, real φ) {
        real x, y, z;
        x = 1.0f / r * sinf(φ) * cosf(θ);
        y = 1.0f / r * cosf(φ);
        z = 1.0f / r * sinf(φ) * sinf(θ);
        glNormal3f(x, y, z);
        x = r * sinf(φ) * cosf(θ);
        y = r * cosf(φ);
        z = r * sinf(φ) * sinf(θ);
        glVertex3f(x, y, z);
}

static inline void spherical_n(real r, real θ, real φ) {
        real x, y, z;
        x = r * sinf(φ) * cosf(θ);
        y = r * cosf(φ);
        z = r * sinf(φ) * sinf(θ);
        glVertex3f(x, y, z);
        glVertex3f(NormL * x, NormL * y, NormL * z);
}

void Sphere::draw() {
        glBegin(GL_TRIANGLES);
        real Δθ = 2.0f * PI / real(m_segmentsX);
        real θ = 0.0f;
        real Δφ = PI / real(m_segmentsY);
        real φ = 0.0f;

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
                φ = 0.0f;
                θ += Δθ;
        }
        glEnd();
}

void Sphere::drawNormal() {
        glColor3f(1.0f, 0.0f, 0.0f);
        glBegin(GL_LINES);
        real Δθ = 2.0f * PI / real(m_segmentsX);
        real θ = 0.0f;
        real Δφ = PI / real(m_segmentsY);
        real φ = 0.0f;

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
                φ = 0.0f;
                θ += Δθ;
        }
        glEnd();
}

real Sphere::Intersect(Point const eye, Vector const ray) {
        Vector veye = Vector(-eye);
        Vector vray = normalize(ray);
        real dotprod = dot(veye, vray);

        Vector closest = veye - dotprod * vray;
        real dist = SQR((closest[0])) + SQR((closest[1])) + SQR((closest[2]));
        if (dist > 0.75f) {
               return std::numeric_limits<real>::max();
        }

        real A = dot(ray, ray);
        real B = 2.0f * dot(eye, ray);
        real C = dot(eye, eye) - (R * R);

        real disc = (B * B) - (4.0f * A * C);

        if (disc < 0.0f) {
                return std::numeric_limits<real>::max();
        }
        real denom = (2.0f * A);
        real bdiv = -B / denom;
        real ddiv = sqrtf(disc) / denom;
        real t1 = bdiv + ddiv;
        real t2 = bdiv - ddiv;

        return fmin(t1, t2);
}

Vector Sphere::findIsectNormal(Point const eye, Vector const ray, real t) {
        Vector isect = eye + (ray * t);
        isect.normalize();
        return isect;
}

