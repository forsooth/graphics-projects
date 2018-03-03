#include "Camera.h"

Camera::Camera() {
        eye = Point(2.0, 2.0, 2.0);
        nearPlane = 0.001;
        farPlane = 30.0;
        depth = farPlane - nearPlane;

        viewAngle = 45.0;
        screenWidth = 500;
        screenHeight = 500;
        aspectRatio = 1.0;
        
        look = Vector(-2.0, -2.0, -2.0);
        up = Vector(0.0, 1.0, 0.0);
        W = -look / length(look);
        U = cross(up, W) / length(cross(up, W));
        V = cross(W, U);

        cam_trans = Matrix();
        cam_rot = Matrix();
        cam_scale = Matrix();
        unhinge = Matrix();
}

Camera::~Camera() {

}

void Camera::Orient(Point& eye, Point& focus, Vector& up) {
        look = Vector((focus.x() - eye.x()), (focus.y() - eye.y()), (focus.z() - eye.z()));
        Orient(eye, look, up);
}

void Camera::Orient(Point& eye, Vector& look, Vector& up) {
        this->up = up;
        this->look = look;
        this->eye = eye;
        W = -look / length(look);
        U = cross(up, W) / length(cross(up, W));
        V = cross(W, U);
}

Matrix Camera::GetModelViewMatrix() {
        cam_trans = trans_mat(-eye);
        cam_rot = cam_rot_mat(U, V, W);
        return cam_rot * cam_trans;
}

Matrix Camera::GetProjectionMatrix() {
        double thetaW = ((PI / 180.0) * viewAngle);
        double thetaH = thetaW / aspectRatio;
        Vector scale = Vector(1.0 / (tan(thetaW / 2.0) * farPlane),
                              1.0 / (tan(thetaH / 2.0) * farPlane),
                              1.0 / farPlane);
        cam_scale = scale_mat(scale);
        double c = -nearPlane / farPlane;
        unhinge = Matrix(1, 0, 0, 0,
                         0, 1, 0, 0,
                         0, 0, (1.0 / (c + 1)), (-c / (c + 1)),
                         0, 0, -1, 0);
        return unhinge * cam_scale;
}

void Camera::SetViewAngle (double viewAngle) {
        this->viewAngle = viewAngle;
}

void Camera::SetNearPlane(double nearPlane) {
        this->nearPlane = nearPlane;
        this->depth = (farPlane - nearPlane);
}

void Camera::SetFarPlane(double farPlane) {
        this->farPlane = farPlane;
        this->depth = (farPlane - nearPlane);
}

void Camera::SetScreenSize (int screenWidth, int screenHeight) {
        this->screenWidth = screenWidth;
        this->screenHeight = screenHeight;
        this->aspectRatio = ((double)screenWidth) / ((double)screenHeight);
}

void Camera::RotateV(double angle) {
        if (angle == 0) { return; }
        Matrix rot_m = rot_mat(eye, V, PI / 180.0 * angle);
        U = rot_m * U;
        W = rot_m * W;
}

void Camera::RotateU(double angle) {
        if (angle == 0) { return; }
        Matrix rot_m = rot_mat(eye, U, PI / 180.0 * angle);
        V = rot_m * V;
        W = rot_m * W;
}

void Camera::RotateW(double angle) {
        if (angle == 0) { return; }
        Matrix rot_m = rot_mat(eye, W, PI / 180.0 * angle);
        U = rot_m * U;
        V = rot_m * V;
}

void Camera::Translate(const Vector &v) {
        eye = trans_mat(v) * eye;
}

void Camera::Rotate(Point p, Vector axis, double degrees) {
        Matrix rot_m = rot_mat(p, axis, PI / 180.0 * degrees);
        U = rot_m * U;
        V = rot_m * V;
        W = rot_m * W;
}

Point Camera::GetEyePoint() {
        return eye;
}

Vector Camera::GetLookVector() {
        return look;
}

Vector Camera::GetUpVector() {
        return up;
}

double Camera::GetViewAngle() {
        return viewAngle;
}

double Camera::GetNearPlane() {
        return nearPlane;
}

double Camera::GetFarPlane() {
        return farPlane;
}

int Camera::GetScreenWidth() {
        return screenWidth;
}

int Camera::GetScreenHeight() {
        return screenHeight;
}

double Camera::GetFilmPlaneDepth() {
        return depth;
}

double Camera::GetScreenWidthRatio() {
        return aspectRatio;
}
