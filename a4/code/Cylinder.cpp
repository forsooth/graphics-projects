#include "Cylinder.hpp"
#define R 0.5f

Cylinder::Cylinder() { type = SHAPE_CYLINDER; };

static inline void cylindrical_v(real r, real θ, real y) {
        real x, z;
        x = cosf(θ);
        z = sinf(θ);
        glNormal3f(x, 0.0f, z);
        x = r * cosf(θ);
        z = r * sinf(θ);
        glVertex3f(x, y, z);
}

static inline void cylindrical_n(real r, real θ, real y) {
        real x, z;
        x = r * cosf(θ);
        z = r * sinf(θ);
        glVertex3f(x, y, z);
        glVertex3f(NormL * x, y, NormL * z);
}

void Cylinder::draw() {
        glBegin(GL_TRIANGLES);
        real y = R;
        real Δy = 2.0f * R / real(m_segmentsY);
        real θ = 0.0f;
        real Δθ = 2.0f * PI / real(m_segmentsX);
        for (int slice = 0; slice < m_segmentsX; slice++) {
                glNormal3f(0.0f, 1.0f, 0.0f);
                glVertex3f(R * cosf(θ), y, R * sinf(θ));
                glVertex3f(0.0f, y, 0.0f);
                glVertex3f(R * cosf(θ + Δθ), y, R * sinf(θ + Δθ));
                for (int layer = 0; layer < m_segmentsY; layer++) {
                        cylindrical_v(R, θ, y);
                        cylindrical_v(R, θ, y - Δy);
                        cylindrical_v(R, θ + Δθ, y);
                        cylindrical_v(R, θ + Δθ, y - Δy);
                        cylindrical_v(R, θ + Δθ, y);
                        cylindrical_v(R, θ, y - Δy);
                        y -= Δy;
                }
                glNormal3f(0.0f, -1.0f, 0.0f);
                glVertex3f(R * cosf(θ), y, R * sinf(θ));
                glVertex3f(0.0f, y, 0.0f);
                glVertex3f(R * cosf(θ + Δθ), y, R * sinf(θ + Δθ));
                θ += Δθ;
                y = R;
        }
        glEnd();
}

void Cylinder::drawNormal() {
        glColor3f(1.0f, 0.0f, 0.0f);
        glBegin(GL_LINES);
        real y = R;
        real Δy = 2.0f * R / real(m_segmentsY);
        real line_length = 0.02f;
        real Δθ = 2.0f * PI / real(m_segmentsX);
        real θ = 0.0f;
        glVertex3f(0.0f, R, 0.0f);
        glVertex3f(0.0f, R + line_length, 0.0f);
        for (int pieslice = 0; pieslice < m_segmentsX; pieslice++) {
                for (int layer = 0; layer < m_segmentsY; layer++) {
                        cylindrical_n(R, θ, y);
                        cylindrical_n(R, θ, y - Δy);
                        cylindrical_n(R, θ + Δθ, y);
                        cylindrical_n(R, θ + Δθ, y - Δy);
                        y -= Δy;
                }
                glVertex3f(0.0f, -R, 0.0f);
                glVertex3f(0.0f, -R - line_length, 0.0f);
                θ += Δθ;
                y = R;
        }
        glEnd();
}

real Cylinder::Intersect(Point const eye, Vector const ray) {
        Vector veye = Vector(-eye);
        Vector vray = normalize(ray);
        real dotprod = dot(veye, vray);

        Vector closest = veye - dotprod * vray;
        real dist = SQR((closest[0])) + SQR((closest[1])) + SQR((closest[2]));
        if (dist > 0.75f) {
                return std::numeric_limits<real>::max();
        }

        std::vector<real> ts;
        real A = ray.x() * ray.x() + ray.z() * ray.z();
        real B = 2.0f * eye.x() * ray.x() + 2.0f * eye.z() * ray.z();
        real C = eye.x() * eye.x() +eye.z() * eye.z() - R * R;
        real disc = (B * B) - (4.0f * A * C);

        if (disc < 0.0f) {
                return std::numeric_limits<real>::max();
        }

        real t1 = (-B + sqrtf(disc)) / (2.0f * A);
        real t2 = (-B - sqrtf(disc)) / (2.0f * A);
        real y1 = eye[1] + t1 * ray[1];
        real y2 = eye[1] + t2 * ray[1];
        if (fabs(y1) <= R) {
                ts.emplace_back(t1);
        }
        if (fabs(y2) <= R) {
                ts.emplace_back(t2);
        }

        if (ray.y() != 0.0f) {
                real t_top = (R - eye.y()) / ray.y();
                real x_top = eye.x() + (ray.x() * t_top);
                real z_top = eye.z() + (ray.z() * t_top);
                if (x_top * x_top + z_top * z_top <= R * R) {
                        ts.emplace_back(t_top);
                }
                real t_bot = (-R - eye.y()) / ray.y();
                real x_bot = eye.x() + (ray.x() * t_bot);
                real z_bot = eye.z() + (ray.z() * t_bot);
                if (x_bot * x_bot + z_bot * z_bot <= R * R) {
                        ts.emplace_back(t_bot);
                }
        }

        if (ts.size() == 1) {
                return ts[0];
        } else if (!ts.empty()) {
                return fmin(ts[0], ts[1]);
        }
        return std::numeric_limits<real>::max();
}

Vector Cylinder::findIsectNormal(Point const eye, Vector const ray, real t) {
        real ε = 1e-4f;
        Vector isect = eye + (ray * t);

        if (isect.y() > R - ε) {
                isect[0] = 0.0f;
                isect[1] = 1.0f;
                isect[2] = 0.0f;
        } else if (isect.y() < -R + ε) {
                isect[0] = 0.0f;
                isect[1] = -1.0f;
                isect[2] = 0.0f;
        } else {
                isect[1] = 0.0f;
        }

        isect.normalize();
        return isect;
}

