#include "Camera.hpp"

Camera::Camera() {
        eye = Point(2.0f, 2.0f, 2.0f);
        nearPlane = 0.001f;
        farPlane = 30.0f;
        depth = farPlane - nearPlane;

        viewAngle = 45.0f;
        screenWidth = 500;
        screenHeight = 500;
        aspectRatio = 1.0f;
        
        look = Vector(-2.0f, -2.0f, -2.0f);
        up = Vector(0.0f, 1.0f, 0.0f);
        W = -look / length(look);
        U = cross(up, W) / length(cross(up, W));
        V = cross(W, U);
}

Camera::~Camera() {

}

void Camera::Reset() {
        eye = Point(2.0f, 2.0f, 2.0f);
        nearPlane = 0.001f;
        farPlane = 30.0f;
        depth = farPlane - nearPlane;
        viewAngle = 45.0f;
        look = Vector(-2.0f, -2.0f, -2.0f);
        up = Vector(0.0f, 1.0f, 0.0f);
        W = -look / length(look);
        U = cross(up, W) / length(cross(up, W));
        V = cross(W, U);
}

void Camera::Orient(const Point &eye, const Point &focus, const Vector &up) {
        look = Vector(focus - eye);
        Orient(eye, look, up);
}

void Camera::Orient(const Point &eye, const Vector &look, const Vector &up) {
        this->up = up;
        this->look = look;
        this->eye = eye;
        W = -look / length(look);
        U = cross(up, W) / length(cross(up, W));
        V = cross(W, U);
}

Matrix Camera::GetModelViewMatrix() const {
        Matrix cam_trans = trans_mat(-eye);
        Matrix cam_rot = cam_rot_mat(U, V, W);
        return cam_rot * cam_trans;
}

Matrix Camera::GetProjectionMatrix() const {
        real thetaH = RAD * viewAngle;
        real thetaW = thetaH * aspectRatio;
        Vector scale = Vector(1.0f / (tanf(thetaW / 2.0f) * farPlane),
                              1.0f / (tanf(thetaH / 2.0f) * farPlane),
                              1.0f / farPlane);
        Matrix cam_scale = scale_mat(scale);
        real c = -nearPlane / farPlane;
        Matrix unhinge = Matrix(1.0f, 0.0f, 0.0f, 0.0f,
                         0.0f, 1.0f, 0.0f, 0.0f,
                         0.0f, 0.0f, (-1.0f / (c + 1.0f)), (c / (c + 1.0f)),
                         0.0f, 0.0f, -1.0f, 0.0f);
        return unhinge * cam_scale;
}

Matrix Camera::GetFilmToWorldMatrix() const
{
        real thetaH = RAD * viewAngle;
        real thetaW = thetaH * aspectRatio;
        Matrix m1 = inv_trans_mat(Point() - eye);
        Matrix m2(U[0], V[0], W[0], 0.0f,
                  U[1], V[1], W[1], 0.0f,
                  U[2], V[2], W[2], 0.0f,
                  0.0f,    0.0f,    0.0f,    1.0f);
        Matrix m3 = scale_mat(Vector(farPlane * tan(thetaW / 2.0f),
                                     farPlane * tan(thetaH / 2.0f),
                                     farPlane));
        return m1 * m2 * m3;
}

void Camera::SetViewAngle (real viewAngle) {
        this->viewAngle = viewAngle;
}

void Camera::SetNearPlane(real nearPlane) {
        this->nearPlane = nearPlane;
        this->depth = (farPlane - nearPlane);
}

void Camera::SetFarPlane(real farPlane) {
        this->farPlane = farPlane;
        depth = (farPlane - nearPlane);
}

void Camera::SetScreenSize (int screenWidth, int screenHeight) {
        this->screenWidth = screenWidth;
        this->screenHeight = screenHeight;
        aspectRatio = ((real)screenWidth) / ((real)screenHeight);
}

void Camera::RotateV(real angle) {
        if (angle == 0) { return; }
        Matrix rot_m = rot_mat(V, DEG_TO_RAD(angle));
        look = rot_m * look;
        up = rot_m * up;
        U = rot_m * U;
        W = rot_m * W;
}

void Camera::RotateU(real angle) {
        if (angle == 0) { return; }
        Matrix rot_m = rot_mat(U, DEG_TO_RAD(angle));
        look = rot_m * look;
        up = rot_m * up;
        V = rot_m * V;
        W = rot_m * W;
}

void Camera::RotateW(real angle) {
        if (angle == 0) { return; }
        Matrix rot_m = rot_mat(W, DEG_TO_RAD(angle));
        look = rot_m * look;
        up = rot_m * up;
        U = rot_m * U;
        V = rot_m * V;
}

void Camera::Translate(const Vector &v) {
        eye = trans_mat(v) * eye;
}

void Camera::Rotate(Point p, Vector axis, real degrees) {
        if (degrees == 0) { return; }
        Matrix rot_m = rot_mat(p, axis, RAD * degrees);
        U = rot_m * U;
        V = rot_m * V;
        W = rot_m * W;
}

Point Camera::GetEyePoint() const {
        return eye;
}

Vector Camera::GetLookVector() const {
        return look;
}

Vector Camera::GetUpVector() const {
        return up;
}

real Camera::GetViewAngle() const {
        return viewAngle;
}

real Camera::GetNearPlane() const {
        return nearPlane;
}

real Camera::GetFarPlane() const {
        return farPlane;
}

int Camera::GetScreenWidth() const {
        return screenWidth;
}

int Camera::GetScreenHeight() const {
        return screenHeight;
}

real Camera::GetFilmPlaneDepth() const {
        return depth;
}

real Camera::GetScreenWidthRatio() const {
        return aspectRatio;
}
