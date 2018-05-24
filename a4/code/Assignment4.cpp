#include <iostream>
#include <fstream>
#include <string>
#include <thread>
#include <limits>
#include <chrono>
#include <GL/glx.h>
#include <GL/glui.h>
#include <mutex>
#include "Flattener.hpp"
#include "Raycaster.hpp"
#include "Shape.hpp"
#include "Cube.hpp"
#include "Cylinder.hpp"
#include "Cone.hpp"
#include "Sphere.hpp"
#include "SceneParser.hpp"
#include "Camera.hpp"
#include "KDTree.hpp"
#define NUM_OPENGL_LIGHTS 8
#define NUM_THREADS 12
/* Compiler Flags */
//#define CONCURRENT
//#define FISH_EYE
//#define ORTHO
//#define SUPERSAMPLE
//#define FLOATALG
//#define PROGRESS
//#define OBJRAY
#define PERSECTIVE 0
#define FISH_EYE 1
#define ORTHO 2

#ifdef PROGRESS
std::mutex progress;
#endif

std::string filenamePath = "data/general/chess.xml";

/* These are the live variables passed into GLUI */
int isectOnly = 1;
/* Camera defaults */
int camRotU = 0;
int camRotV = 0;
int camRotW = 0;
float viewAngle = 45;
float eyeX = 2;
float eyeY = 2;
float eyeZ = 2;
float lookX = -2;
float lookY = -2;
float lookZ = -2;

#define PERSECTIVE 0
#define FISH_EYE 1
#define ORTHO 2

int supersample_box; // 0 - Don't 1 Do
int lens;

SceneParser *parser = nullptr;
Camera *camera = new Camera();

/* These are GLUI control panel objects */
int main_window;
GLUI_EditText *filenameTextField = nullptr;
GLubyte *pixels = nullptr;
int pixW = 0, pixH = 0;
int screenW = 0, screenH = 0;

Flattener *flattener = new Flattener();
KDTree *tree = nullptr;

void display(void);
void idle(void);

void set_up_camera() {
        SceneCameraData cameraData;
        parser->getCameraData(cameraData);
        camera->Reset();
        camera->SetScreenSize(pixW, pixH);
        camera->SetViewAngle(cameraData.heightAngle);
        if (cameraData.isDir) {
                camera->Orient(cameraData.pos, cameraData.look,
                               cameraData.up);
        }
        else {
                camera->Orient(cameraData.pos, cameraData.lookAt,
                               cameraData.up);
        }

        viewAngle = camera->GetViewAngle();
        Point eyeP = camera->GetEyePoint();
        Vector lookV = camera->GetLookVector();
        eyeX = eyeP[0];
        eyeY = eyeP[1];
        eyeZ = eyeP[2];
        lookX = lookV[0];
        lookY = lookV[1];
        lookZ = lookV[2];
        camRotU = 0;
        camRotV = 0;
        camRotW = 0;
        GLUI_Master.sync_live_all();
        
}

void update_camera() {
        camera->Reset();
        Point guiEye (eyeX, eyeY, eyeZ);
        Vector guiLook(lookX, lookY, lookZ);
        camera->SetScreenSize(pixW, pixH);
        camera->SetViewAngle(viewAngle);
        Vector guiUp = camera->GetUpVector();
        camera->Orient(guiEye, guiLook, guiUp);
        camera->RotateU(camRotU);
        camera->RotateV(camRotV);
        camera->RotateW(camRotW);

}

void set_pixel(GLubyte *buf, int x, int y, int r, int g, int b) {
        buf[(y * pixW + x) * 3 + 0] = (GLubyte) r;
        buf[(y * pixW + x) * 3 + 1] = (GLubyte) g;
        buf[(y * pixW + x) * 3 + 2] = (GLubyte) b;
}


inline SceneColor supersample(SceneColor *cs, SceneColor c,
                                        int rowsize, int last_row,
                                        int col, int row) {
        return c * 0.5f + 0.125f * *(cs + rowsize * col
                                     + (last_row - 1 - row))
                        + 0.125f * *(cs + rowsize * col
                                     + (last_row - 1 - row + 1))
                        + 0.125f * *(cs + rowsize * (col + 1)
                                     + (last_row - 1 - row))
                        + 0.125f * *(cs + rowsize * (col + 1)
                                     + (last_row - 1 - row + 1));
}

void color_pixel_block(int i) {
        int inital_row = pixH / NUM_THREADS * i;
        int last_row = (i == NUM_THREADS - 1 ? pixH : 
                        pixH / NUM_THREADS * (i + 1));
        SceneColor cs[pixW + 1][last_row - inital_row + 1];

        for (int row = inital_row; row < last_row + 1; row++) {
                for (int col = 0; col < pixW + 1; col++) {
                        SceneColor c = raycast(col, row, pixW, pixH,
                                               camera, parser, tree, lens);
                        if (row < last_row) {
                                set_pixel(pixels, col, (pixH - 1) - row,
                                          (int)(c.r * 255.0f),
                                          (int)(c.g * 255.0f),
                                          (int)(c.b * 255.0f));
                        }
                        cs[col][last_row - row] = c;
                }
#ifdef PROGRESS
                progress.lock();
                display();
                progress.unlock();
#endif
        }

        if (supersample_box) {
                for (int row = inital_row; row < last_row; row++) {
                        for (int col = 0; col < pixW; col++) {
                                SceneColor c = raycast((real)col + 0.5f,
                                                       (real)row + 0.5f,
                                                        pixW, pixH, camera,
                                                        parser, tree, lens);

                                SceneColor avg = supersample((SceneColor *)cs, c,
                                                             last_row - inital_row + 1,
                                                             last_row, col, row);

                                set_pixel(pixels, col, (pixH - 1) - row,
                                          (int)(avg.r * 255.0f),
                                          (int)(avg.g * 255.0f),
                                          (int)(avg.b * 255.0f));
                        }
                }
        }
}

void callback_start(int id) {
        (void) id;
        std::cerr << "start button clicked!" << std::endl;
        if (parser == nullptr) {
                std::cerr << "no scene loaded yet" << std::endl;
                return;
        }

        pixW = screenW;
        pixH = screenH;

        delete pixels;
        update_camera();

        pixels = new GLubyte[(unsigned int)(pixW * pixH * 3)];
        std::cerr << "(w, h): " << pixW << ", " << pixH << std::endl;

        std::chrono::high_resolution_clock::time_point t1 = std::chrono::high_resolution_clock::now();

        std::vector<std::thread> threads;

#pragma unroll NUM_THREADS
        for (int i = 0; i < NUM_THREADS; i++) {
                #ifdef CONCURRENT
                        threads.emplace_back(std::thread(color_pixel_block,
                                                         i));
                #else
                        color_pixel_block(i);
                #endif
        }

        for (auto &th : threads) {
                th.join();
        }

        std::chrono::high_resolution_clock::time_point t2 = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>( t2 - t1 ).count();
        double secdur = duration / 1e6;
        std::cerr << "Duration of screen calculation: " << secdur << std::endl;
        fprintf(stderr, "Done drawing screen\n");
        glutPostRedisplay();
}

void callback_load(int id) {
        (void) id;
        if (filenameTextField == nullptr) {
                return;
        }
        printf("%s\n", filenameTextField->get_text());
        
        if (parser != nullptr) {
                delete parser;
        }
        parser = new SceneParser (filenamePath);

        bool success = parser->parse();
        std::cerr << "success? " << success << std::endl;
        if (!success) {
                delete parser;
                parser = nullptr;
        }
        else {
                flattener->flattenTree(parser);
                if (tree != nullptr) {
                //        TODO: delete tree;
                }
                tree = new KDTree(flattener->flattened);
                set_up_camera();
        }
}

void idle(void) {
        /* According to the GLUT specification, the current window is
        undefined during an idle callback. So we need to explicitly change
        it if necessary */
        if (glutGetWindow() != main_window) {
                glutSetWindow(main_window);
        }
        glutPostRedisplay();
}

void reshape(int x, int y) {
        float xy_aspect;

        xy_aspect = (float)x / (float)y;
        glViewport(0, 0, x, y);
        camera->SetScreenSize(x, y);
        
        screenW = x;
        screenH = y;

        glutPostRedisplay();
}

void display(void) {
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        if (parser == nullptr || pixels == nullptr) {
                return;
        }

        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
        glDrawPixels(pixW, pixH, GL_RGB, GL_UNSIGNED_BYTE, pixels);
        glutSwapBuffers();
}

void on_exit() {
        delete camera;
        if (parser != nullptr) delete parser;
        if (pixels != nullptr) delete pixels;
        delete flattener;
}

/**************************************** main() ********************/

int main(int argc, char *argv[]) {

        atexit(on_exit);

        glutInit(&argc, argv);
        glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);
        glutInitWindowPosition(50, 50);
        glutInitWindowSize(500, 500);

        main_window = glutCreateWindow("COMP 175 Assignment 4");
        glutDisplayFunc(display);
        glutReshapeFunc(reshape);

        GLUI *glui = GLUI_Master.create_glui("GLUI");

        filenameTextField = new GLUI_EditText(glui, "Filename:", filenamePath);
        filenameTextField->set_w(300);
        glui->add_button("Load", 0, callback_load);
        glui->add_button("Start!", 0, callback_start);
        glui->add_checkbox("Isect Only", &isectOnly);

        glui->add_checkbox("Super Sample", &supersample_box);

        GLUI_RadioGroup *group1 = glui->add_radiogroup(&lens);
        glui->add_radiobutton_to_group( group1, "Perspective" );
        glui->add_radiobutton_to_group( group1, "Fish Eye"    );
        glui->add_radiobutton_to_group( group1, "Orthogonal"  );

        GLUI_Panel *camera_panel = glui->add_panel("Camera");
        (new GLUI_Spinner(camera_panel, "RotateV:", &camRotV))
                          ->set_int_limits(-179, 179);
        (new GLUI_Spinner(camera_panel, "RotateU:", &camRotU))
                          ->set_int_limits(-179, 179);
        (new GLUI_Spinner(camera_panel, "RotateW:", &camRotW))
                          ->set_int_limits(-179, 179);
        (new GLUI_Spinner(camera_panel, "Angle:", &viewAngle))
                          ->set_int_limits(1, 179);

        glui->add_column_to_panel(camera_panel, true);

        GLUI_Spinner *eyex_widget =
               glui->add_spinner_to_panel(camera_panel, "EyeX:",
                                          GLUI_SPINNER_FLOAT, &eyeX);
        eyex_widget->set_float_limits(-20, 20);

        GLUI_Spinner *eyey_widget =
                glui->add_spinner_to_panel(camera_panel, "EyeY:",
                                           GLUI_SPINNER_FLOAT, &eyeY);
        eyey_widget->set_float_limits(-20, 20);

        GLUI_Spinner *eyez_widget =
                glui->add_spinner_to_panel(camera_panel, "EyeZ:",
                                           GLUI_SPINNER_FLOAT, &eyeZ);
        eyez_widget->set_float_limits(-20, 20);

        GLUI_Spinner *lookx_widget =
                glui->add_spinner_to_panel(camera_panel, "LookX:",
                                           GLUI_SPINNER_FLOAT, &lookX);
        lookx_widget->set_float_limits(-20, 20);

        GLUI_Spinner *looky_widget =
                glui->add_spinner_to_panel(camera_panel, "LookY:",
                                           GLUI_SPINNER_FLOAT, &lookY);
        looky_widget->set_float_limits(-20, 20);

        GLUI_Spinner *lookz_widget =
                glui->add_spinner_to_panel(camera_panel, "LookZ:",
                                           GLUI_SPINNER_FLOAT, &lookZ);
        lookz_widget->set_float_limits(-20, 20);

        glui->add_button("Quit", 0, (GLUI_Update_CB) exit);
        glui->set_main_gfx_window(main_window);

        /* We register the idle callback with GLUI, *not* with GLUT */
        GLUI_Master.set_glutIdleFunc(idle);

        glutMainLoop();

        return EXIT_SUCCESS;
}



