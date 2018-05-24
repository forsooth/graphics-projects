#include "Cone.hpp"

#define R 0.5f

Cone::Cone() { type = SHAPE_CONE; };

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
        glVertex3f(x + 2.0f * sqrtf(5)/5 * 0.1f * cosf(θ),
                   y + (sqrtf(5)/5 * 0.1f),
                   z + 2.0f * sqrtf(5)/5 * 0.1f * sinf(θ));
}

void Cone::draw() {
        glBegin(GL_TRIANGLES);
        real y = R;
        real Δy = 2.0f * R / real(m_segmentsY);
        real Δθ = 2.0f * PI / real(m_segmentsX);
        real θ = 0.0f;
        for (int pieslice = 0; pieslice < m_segmentsX; pieslice++) {
                real r = 0;
                real Δr = R / m_segmentsY;
                for (int layer = 0; layer < m_segmentsY; layer++) {
                        cylindrical_v(r, θ, y);
                        cylindrical_v(r + Δr, θ, y - Δy);
                        cylindrical_v(r, θ + Δθ, y);
                        cylindrical_v(r + Δr, θ + Δθ, y - Δy);
                        cylindrical_v(r, θ + Δθ, y);
                        cylindrical_v(r + Δr, θ, y - Δy);
                        y -= Δy;
                        r += Δr;
                }
                glNormal3f(0.0f, -1.0f, 0.0f);
                glVertex3f(R * cosf(θ), y, R * sinf(θ));
                glVertex3f(0.0f, y, 0.0f);
                glVertex3f((R * cosf(θ) + Δθ), y, R * sinf(θ + Δθ));
                θ += Δθ;
                y = R;        }
        glEnd();
}

void Cone::drawNormal() {
        glColor3f(1.0f, 0.0f, 0.0f);
        glBegin(GL_LINES);
        real y = R;
        real Δy = 2.0f * R / real(m_segmentsY);
        real Δθ = 2.0f * PI / real(m_segmentsX);
        real θ = 0.0f;
        glVertex3f(0.0f, -R, 0.0f);
        glVertex3f(0.0f, -R - NormL, 0.0f);
        for (int pieslice = 0; pieslice < m_segmentsX; pieslice++) {
                real r = 0.0f;
                real Δr = R / m_segmentsY;
                for (int layer = 0; layer < m_segmentsY; layer++) {
                        if (r != 0) {
                                cylindrical_n(r, θ, y);
                                cylindrical_n(r, θ + Δθ, y);
                                cylindrical_n(r + Δr, θ + Δθ, y - Δy);
                                cylindrical_n(r + Δr, θ, y - Δy);

                        }
                        y -= Δy;
                        r += Δr;
                }
                θ += Δθ;
                y = R;
        }
        glEnd();
}


static inline real planeIntersect(Point eye, Vector ray,
                                    int dimension, real side) {
        int dim1 = dimension == 0 ? 1 : dimension == 1 ? 2 : 1;
        int dim2 = dimension == 0 ? 2 : 0;
        if (ray[dimension] != 0.0f) {
                real t = (side - eye[dimension]) / ray[dimension];
                real x = eye.x() + (ray.x() * t);
                real y = eye.y() + (ray.y() * t);
                real z = eye.z() + (ray.z() * t);

                real coords[3] = {x, y, z};

            // check if t is valid
            if (coords[dim1] <= R && coords[dim1] >= -R &&
                    coords[dim2] <= R && coords[dim2] >= -R) {
                    return t;
            } else {
                    return std::numeric_limits<real>::max();
            }
        }

        return std::numeric_limits<real>::max();
}



real Cone::Intersect(const Point eye, const Vector ray) {
        Vector veye = Vector(-eye);
        Vector vray = normalize(ray);
        real dotprod = dot(veye, vray);

        Vector closest = veye - dotprod * vray;
        real dist = SQR((closest[0])) + SQR((closest[1])) + SQR((closest[2]));
        if (dist > 0.75f) {
                return std::numeric_limits<real>::max();
        }

        real A = (4.0f * ray.x() * ray.x()) + (4.0f * ray.z() * ray.z()) - (ray.y() * ray.y());
        real B = (8.0f * eye.x() * ray.x()) + (8.0f * eye.z() * ray.z()) - (2.0f * eye.y() * ray.y()) + (ray.y());
        real C = (4.0f * eye.x() * eye.x()) + (4.0f * eye.z() * eye.z()) - (eye.y() * eye.y()) + (eye.y()) - 0.25f;
        real disc = (B * B) - (4.0f * A * C);

        if (disc < 0.0f) {
                // if we missed the real cone we definitely didn't hit the bottom
                return std::numeric_limits<real>::max();
        }

        real t1 = (-B + sqrtf(disc)) / (2.0f * A);
        real t2 = (-B - sqrtf(disc)) / (2.0f * A);

        Point p1 = eye + (t1 * ray);
        Point p2 = eye + (t2 * ray);

        // t1 didn't hit the cones
        if (!(-0.5f <= p1.y() && p1.y() <= 0.5f)) {
                t1 = std::numeric_limits<real>::max();
        }

        // t2 didn't hit the cone
        if (!(-0.5f <= p2.y() && p2.y() <= 0.5f)) {
            t2 = std::numeric_limits<real>::max();
        }

        real t3 = planeIntersect(eye, ray, 1.0f, -0.5f);

        if (t3 != std::numeric_limits<real>::max()) {
                Point on_bottom = eye + t3 * ray;
                // not inside base
                if (on_bottom.x() * on_bottom.x() + on_bottom.z() * on_bottom.z() > (R * R)) {
                        t3 = std::numeric_limits<real>::max();
                }
        }

        return fmin(fmin(t1, t2), t3);
}

Vector Cone::findIsectNormal(const Point eye, const Vector ray, real t) {
        real ε = EPSILON;

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
                real at = atan2f(isect[2], isect[0]);
                isect[0] = 2.0f / sqrtf(5.0f) * cosf(at);
                isect[1] = 1.0f / sqrtf(5.0f);
                isect[2] = 2.0f / sqrtf(5.0f) * sinf(at);
        }

        isect.normalize();
        return isect;
}
