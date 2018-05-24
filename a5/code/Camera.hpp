#ifndef CAMERA_H
#define CAMERA_H

#include "Algebra.hpp"

class Camera {

public:
        Camera();
        ~Camera();
        void Orient(const Point &eye, const Point &focus, const Vector &up);
        void Orient(const Point &eye, const Vector &look, const Vector &up);
        void SetViewAngle (real viewAngle);
        void SetNearPlane (real nearPlane);
        void SetFarPlane (real farPlane);
        void SetScreenSize (int screenWidth, int screenHeight);
        
        void Reset();

        Matrix GetProjectionMatrix() const;
        Matrix GetModelViewMatrix() const;
        Matrix GetFilmToWorldMatrix() const;

        void RotateV(real angle);
        void RotateU(real angle);
        void RotateW(real angle);
        void Rotate(Point &p, Vector &axis, real degree);
        void Translate(const Vector &v);

        Point GetEyePoint() const;
        Vector GetLookVector() const;
        Vector GetUpVector() const;
        real GetViewAngle() const;
        real GetNearPlane() const;
        real GetFarPlane() const;
        int GetScreenWidth() const;
        int GetScreenHeight() const;

        real GetFilmPlaneDepth() const;
        real GetScreenWidthRatio() const;
        Vector U, V, W;

private:
        real viewAngle, nearPlane, farPlane, depth;
        int screenWidth, screenHeight;
        real aspectRatio;
        Vector look, up;
        Point eye;

};
#endif

