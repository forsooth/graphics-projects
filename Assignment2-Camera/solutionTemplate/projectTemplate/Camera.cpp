#include "Camera.h"

Camera::Camera()
{
}

Camera::~Camera()
{
}

void Camera::Orient(Point& eye, Point& focus, Vector& up)
{
    this.up = up;
    this.look = new Vector((focus.x() - eye.x()), (focus.y() - eye.y()), (focus.z() - eye.z()));
    this.eye = eye;

}

void Camera::Orient(Point& eye, Vector& look, Vector& up)
{
    this.up = up;
    this.look = look;
    this.eye = eye;
}

Matrix Camera::GetModelViewMatrix()
{
    // model_trans = trans_mat()
    // model_rot = 
    // model_scale = 
    cam_trans = trans_mat(-eye);
    cam_rot = cam_rot_mat(up, look);
}

Matrix Camera::GetProjectionMatrix()
{
    cam_scale = scale_mat();
    unhinge = ;
}


void Camera::SetViewAngle (double viewAngle)
{
    this.viewAngle = viewAngle;

}

void Camera::SetNearPlane(double nearPlane)
{
    this.nearPlane = nearPlane;
    this.depth = (nearPlane - farPlane);
}

void Camera::SetFarPlane(double farPlane)
{
    this.farPlane = farPlane;
    this.depth = (nearPlane - farPlane);

}

void Camera::SetScreenSize (int screenWidth, int screenHeight)
{
    this.screenWidth = screenWidth;
    this.screenHeight = screenHeight;
    this.aspectRatio = (double)screenWidth / (double)screenHeight;
}


void Camera::RotateV(double angle)
{
}

void Camera::RotateU(double angle)
{
}

void Camera::RotateW(double angle)
{
}

void Camera::Translate(const Vector &v)
{
    cam_trans = cam_trans * trans_mat(v);
}


void Camera::Rotate(Point p, Vector axis, double degrees)
{
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
