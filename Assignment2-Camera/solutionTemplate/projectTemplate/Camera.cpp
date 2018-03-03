#include "Camera.h"

Camera::Camera()
{
    eye = Point(2.0, 2.0, 2.0);
    nearPlane = 0.001;
    farPlane = 30.0;
    depth = farPlane - nearPlane;

    viewAngle = 45.0;
    screenWidth = 500.0;
    screenHeight = 500.0;
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

Camera::~Camera()
{

}

void Camera::Orient(Point& eye, Point& focus, Vector& up)
{
    look = Vector((focus.x() - eye.x()), (focus.y() - eye.y()), (focus.z() - eye.z()));
    Orient(eye, look, up);

}

void Camera::Orient(Point& eye, Vector& look, Vector& up)
{
    
    this->up = up;
    this->look = look;
    this->eye = eye;
    W = -look / length(look);
    U = cross(up, W) / length(cross(up, W));
    V = cross(W, U);
}

Matrix Camera::GetModelViewMatrix()
{

    cam_trans = trans_mat(-eye);
    cam_rot = cam_rot_mat(U, V, W);
    return cam_rot * cam_trans;

}

Matrix Camera::GetProjectionMatrix()
{

    double thetaH = ((PI / 180.0) * viewAngle * aspectRatio);
    double thetaW = ((PI / 180.0) * viewAngle / aspectRatio);
    Vector scale = Vector(1 / ((tan(thetaW/ 2)) * farPlane), 1 / ((tan(thetaH/ 2)) * farPlane), 1 / farPlane);
    cam_scale = scale_mat(scale);
    double c = -nearPlane / farPlane;
    unhinge = Matrix(1, 0, 0, 0,
                         0, 1, 0, 0,
                         0, 0, (1 / (c + 1)), (-c / (c + 1)),
                         0, 0, -1, 0);
    return unhinge * cam_scale;
}


void Camera::SetViewAngle (double viewAngle)
{
    this->viewAngle = viewAngle;

}

void Camera::SetNearPlane(double nearPlane)
{
    this->nearPlane = nearPlane;
    this->depth = (farPlane - nearPlane);
}

void Camera::SetFarPlane(double farPlane)
{
    this->farPlane = farPlane;
    this->depth = (farPlane - nearPlane);

}

void Camera::SetScreenSize (int screenWidth, int screenHeight)
{
    this->screenWidth = screenWidth;
    this->screenHeight = screenHeight;
    this->aspectRatio = (double)screenWidth / (double)screenHeight;
}


void Camera::RotateV(double angle)
{
    Rotate(eye, V, angle);
}

void Camera::RotateU(double angle)
{
    Rotate(eye, U, angle);
}

void Camera::RotateW(double angle)
{
    Rotate(eye, W, angle);
}

void Camera::Translate(const Vector &v)
{
    Matrix old_cam_trans = cam_trans;
    // TODO: order of matrix multiplication~?
    cam_trans = cam_trans * trans_mat(v);
}


void Camera::Rotate(Point p, Vector axis, double degrees)
{    
    Matrix old_cam_rot = cam_rot;
    // TODO: order of matrix multiplication~?
    cam_rot = cam_rot * rot_mat(p, axis, degrees);
}


Point Camera::GetEyePoint()
{
    return eye;
}

Vector Camera::GetLookVector()
{
    return look;
}

Vector Camera::GetUpVector()
{
    return up;
}

double Camera::GetViewAngle()
{
    return viewAngle;
}

double Camera::GetNearPlane()
{
    return nearPlane;
}

double Camera::GetFarPlane()
{
    return farPlane;
}

int Camera::GetScreenWidth()
{
    return screenWidth;
}

int Camera::GetScreenHeight()
{
    return screenHeight;
}

double Camera::GetFilmPlaneDepth()
{
    return depth;
}

double Camera::GetScreenWidthRatio()
{
    return aspectRatio;
}
