#ifndef CAMERA_H
#define CAMERA_H

#include "Algebra.h"

class Camera {

private:
// model matrix
// Matrix model_trans, model_rot, model_scale;
// view matrix
Matrix cam_trans, cam_rot;
// projection matrix
Matrix cam_scale, unhinge;
double viewAngle;
double nearPlane;
double farPlane;
double depth;
int screenWidth;
int screenHeight;
double aspectRatio;
Vector look;
Vector up;
Point eye;
Vector U, V, W;

public:
Camera();
~Camera();
void Orient(Point& eye, Point& focus, Vector& up);
void Orient(Point& eye, Vector& look, Vector& up);
void SetViewAngle (double viewAngle);
void SetNearPlane (double nearPlane);
void SetFarPlane (double farPlane);
void SetScreenSize (int screenWidth, int screenHeight);

Matrix GetProjectionMatrix();
Matrix GetModelViewMatrix();

void RotateV(double angle);
void RotateU(double angle);
void RotateW(double angle);
void Rotate(Point p, Vector axis, double degree);
void Translate(const Vector &v);

Point GetEyePoint();
Vector GetLookVector();
Vector GetUpVector();
double GetViewAngle();
double GetNearPlane();
double GetFarPlane();
int GetScreenWidth();
int GetScreenHeight();

double GetFilmPlaneDepth();
double GetScreenWidthRatio();
};
#endif

