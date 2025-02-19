#define NUM_OPENGL_LIGHTS 8

//#include <string.h>
#include <iostream>
#include <fstream>
#include <string>
#include <GL/glui.h>
#include "Shape.h"
#include "Cube.h"
#include "Cylinder.h"
#include "Cone.h"
#include "Sphere.h"
#include "Special.h"
#include "SceneParser.h"
#include "Camera.h"

using namespace std;

/** These are the live variables passed into GLUI ***/
int wireframe = 0;
int fillObj = 1;
int segmentsX = 20;
int segmentsY = 20;

int camRotU = 0;
int camRotV = 0;
int camRotW = 0;
int viewAngle = 45;
float eyeX = 2;
float eyeY = 2;
float eyeZ = 2;
float lookX = -2;
float lookY = -2;
float lookZ = -2;

bool renormalize = false;

string shapes[] = {"SHAPE_CUBE", "SHAPE_CYLINDER", "SHAPE_CONE", "SHAPE_SPHERE", "SHAPE_SPECIAL1", "SHAPE_SPECIAL2", "SHAPE_SPECIAL3", "SHAPE_MESH"};

/** These are GLUI control panel objects ***/
int  main_window;
string filenamePath = "dragon.xml";
GLUI_EditText* filenameTextField = NULL;


/** these are the global variables used for rendering **/
Cube* cube = new Cube();
Cylinder* cylinder = new Cylinder();
Cone* cone = new Cone();
Sphere* sphere = new Sphere();
Shape* shape = NULL;
SceneParser* parser = NULL;
Camera* camera = new Camera();

typedef struct SceneOp {
        uint pops;
        Matrix mat;
        std::vector<ScenePrimitive *> objs;
} SceneOp;

std::vector<SceneOp *> flattened;

void setupCamera();
void flattenTree();
void flattenChild(SceneNode *node);
void drawFlattened(bool fill);

void callback_load(int id) {
        char curDirName [2048];
        (void) curDirName;
        (void) id;
        if (filenameTextField == NULL) {
                return;
        }
        printf ("%s\n", filenameTextField->get_text());

        if (parser != NULL) {
                delete parser;
        }
        parser = new SceneParser(filenamePath);
        bool success = parser->parse();

        cout << "success? " << success << endl;

        if (success == false) {
                delete parser;
                parser = NULL;
        }
        else {
                setupCamera();
                flattenTree();

        }
}

void renderShape (int shapeType) {
        switch (shapeType) {
        case SHAPE_CUBE:
                shape = cube;
                break;
        case SHAPE_CYLINDER:
                shape = cylinder;
                break;
        case SHAPE_CONE:
                shape = cone;
                break;
        case SHAPE_SPHERE:
                shape = sphere;
                break;
        case SHAPE_SPECIAL1:
                shape = cube;
                break;
        default:
                shape = cube;
        }
        shape->setSegments(segmentsX, segmentsY);
        shape->draw();
}

/***************************************** myGlutIdle() ***********/

void myGlutIdle(void)
{
        /* According to the GLUT specification, the current window is
        undefined during an idle callback.  So we need to explicitly change
        it if necessary */
        if (glutGetWindow() != main_window) {
                glutSetWindow(main_window);
        }

        glutPostRedisplay();
}


/**************************************** myGlutReshape() *************/

void myGlutReshape(int x, int y)
{
        float xy_aspect;

        xy_aspect = (float)x / (float)y;
        glViewport(0, 0, x, y);

        camera->SetScreenSize(x, y);

        glutPostRedisplay();
}


/***************************************** setupCamera() *****************/
void setupCamera()
{
        SceneCameraData cameraData;
        parser->getCameraData(cameraData);

        camera->Reset();  //note that this is a new function. Be sure to set values for near and far plane!
        camera->SetViewAngle(cameraData.heightAngle);
        if (cameraData.isDir == true) {
                camera->Orient(cameraData.pos, cameraData.look, cameraData.up);
        }
        else {
                camera->Orient(cameraData.pos, cameraData.lookAt, cameraData.up);
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

/***************************************** setLight() *****************/
void setLight(const SceneLightData &light)
{
    // Zero ambient light because the ambient light for the scene is
    // set with GL_LIGHT_MODEL_AMBIENT
    float zero[] = { 0, 0, 0, 0 };

    // There are 8 lights in OpenGL, GL_LIGHT0 to GL_LIGHT7, and
    // each GL_LIGHT* is one greater than the previous one.  This
    // means the nth light is GL_LIGHT0 + n.
    int id = GL_LIGHT0 + light.id;

    glLightfv(id, GL_AMBIENT, zero);
    glLightfv(id, GL_DIFFUSE, &light.color.r);
    glLightfv(id, GL_SPECULAR, &light.color.r);
    glLightf(id, GL_CONSTANT_ATTENUATION, light.function[0]);
    glLightf(id, GL_LINEAR_ATTENUATION, light.function[1]);
    glLightf(id, GL_QUADRATIC_ATTENUATION, light.function[2]);

    if (glIsEnabled(id))
        cout << "warning: GL_LIGHT" << light.id << " enabled more than once, not supposed to happen" << endl;

    switch (light.type)
    {
        case LIGHT_POINT:
        {
                //fprintf(stderr, "Point light created.\n");
                // Convert from double[] to float[] and make sure the w coordinate is correct 
                float position[] = {(float)light.pos[0], (float)light.pos[1], (float)light.pos[2], 1};
                glLightfv(id, GL_POSITION, position);
                glEnable(id);
                break;
        }
        case LIGHT_DIRECTIONAL:
        {
                //fprintf(stderr, "Directional light created.\n");
                // Convert from double[] to float[] and make sure the direction vector is normalized (it isn't for a lot of scene files)
                Vector direction = -light.dir;
                direction.normalize();
                float position[] = {(float)direction[0], (float)direction[1], (float)direction[2], (float)direction[3]};
                glLightfv(id, GL_POSITION, position);
                glEnable(id);
                break;
        }
        case LIGHT_SPOT:
                //fprintf(stderr, "Spot light created.\n");
                break;
        case LIGHT_AREA:
                //fprintf(stderr, "Area light created.\n");
                break;
    }
}


/***************************************** applyMaterial() *****************/
void applyMaterial(const SceneMaterial &material)
{
        SceneGlobalData globalData;
        parser->getGlobalData(globalData);

        SceneMaterial material_local = material;
        material_local.cAmbient.r *= globalData.ka;
        material_local.cAmbient.g *= globalData.ka;
        material_local.cAmbient.b *= globalData.ka;
        material_local.cDiffuse.r *= globalData.kd;
        material_local.cDiffuse.g *= globalData.kd;
        material_local.cDiffuse.b *= globalData.kd;
        material_local.cSpecular.r *= globalData.ks;
        material_local.cSpecular.g *= globalData.ks;
        material_local.cSpecular.b *= globalData.ks;
        material_local.cReflective.r *= globalData.ks;
        material_local.cReflective.g *= globalData.ks;
        material_local.cReflective.b *= globalData.ks;
        material_local.cTransparent.r *= globalData.kt;
        material_local.cTransparent.g *= globalData.kt;
        material_local.cTransparent.b *= globalData.kt;

    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, &material_local.cAmbient.r);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, &material_local.cDiffuse.r);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, &material_local.cSpecular.r);
    glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, &material_local.cEmissive.r);
}


/**********************************************************/
void flattenTree() {
        flattened.clear();
        fprintf(stderr, "Generated flattened scene tree\n");
        SceneNode* root = parser->getRootNode();
        flattenChild(root);
}

void flattenChild(SceneNode *node) {
        SceneOp *flat_elem = new SceneOp();
        flat_elem->objs = node->primitives;
        flat_elem->pops = 0;
        flat_elem->mat = Matrix();

        for (uint j = 0; j < node->transformations.size(); j++) {
                if (node->transformations[j]->type == TRANSFORMATION_TRANSLATE) {
                        flat_elem->mat = flat_elem->mat * trans_mat(node->transformations[j]->translate);
                } else if (node->transformations[j]->type == TRANSFORMATION_ROTATE) {
                        flat_elem->mat = flat_elem->mat * rot_mat(node->transformations[j]->rotate, node->transformations[j]->angle);
                } else if (node->transformations[j]->type == TRANSFORMATION_SCALE) {
                        flat_elem->mat = flat_elem->mat * scale_mat(node->transformations[j]->scale);
                } else if (node->transformations[j]->type == TRANSFORMATION_MATRIX) {
                        flat_elem->mat = flat_elem->mat * node->transformations[j]->matrix;
                }
        }

        if (node->children.size() == 0) {
                flat_elem->pops++;
                flattened.push_back(flat_elem);
                return;
        }

        flattened.push_back(flat_elem);

        for (uint i = 0; i < node->children.size(); i++) {
                flattenChild(node->children[i]);
                if (i == node->children.size() - 1) {
                        flattened[flattened.size() - 1]->pops++;
                }
        }
}

void drawFlattened(bool fill) {

        for (uint i = 0; i < flattened.size(); i++) {
                glPushMatrix();
                glMultMatrixd(flattened[i]->mat.unpack());

                for (uint j = 0; j < flattened[i]->objs.size(); j++) {
                        PrimitiveType type = flattened[i]->objs[j]->type;
                        if (fill == true) {
                                SceneMaterial material = flattened[i]->objs[j]->material;
                                applyMaterial(material);
                        }
                        renderShape(type);
                }

                for (uint j = 0; j < flattened[i]->pops; j++) {
                        glPopMatrix();
                }
        }

}

/***************************************** myGlutDisplay() *****************/

void myGlutDisplay(void)
{

        glClearColor(.9f, .9f, .9f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glLoadIdentity();
        glPushMatrix();

        if (parser == NULL) {
                return;
        }

        camera->Orient(Point(eyeX, eyeY, eyeZ), Vector(lookX, lookY, lookZ), Vector(0, 1, 0));
        camera->RotateV(camRotV);
        camera->RotateU(camRotU);
        camera->RotateW(camRotW);
        camera->SetViewAngle(viewAngle);
        glMatrixMode(GL_PROJECTION);
        Matrix projection = camera->GetProjectionMatrix();
        glLoadMatrixd(projection.unpack());

        glMatrixMode(GL_MODELVIEW);
        glLoadMatrixd(camera->GetModelViewMatrix().unpack());

        //disable all the lights, and then enable each one...
        for (int i = 0; i < NUM_OPENGL_LIGHTS; i++) {
                glDisable(GL_LIGHT0 + i);
        }

        Matrix compositeMatrix;

        //drawing the axes
        glEnable(GL_COLOR_MATERIAL);
        glDisable(GL_LIGHTING);
        glBegin(GL_LINES);
        glColor3f(1.0, 0.0, 0.0);
        glVertex3f(0, 0, 0); glVertex3f(1.0, 0, 0);
        glColor3f(0.0, 1.0, 0.0);
        glVertex3f(0, 0, 0); glVertex3f(0.0, 1.0, 0);
        glColor3f(0.0, 0.0, 1.0);
        glVertex3f(0, 0, 0); glVertex3f(0, 0, 1.0);
        glEnd();

        glColor3f(1.0, 0.0, 0.0);
        if (wireframe && parser != NULL) {
                glDisable(GL_POLYGON_OFFSET_FILL);
                glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
                glPushMatrix();
                drawFlattened(false);
                glPopMatrix();
        }

        glDisable(GL_COLOR_MATERIAL);
        int numLights = parser->getNumLights();
        for (int i = 0; i < numLights; i++) {
                SceneLightData lightData;
                parser->getLightData(i, lightData);
                setLight(lightData);
        }

        glEnable(GL_LIGHTING);
        if (fillObj == 1 && parser != NULL) {
                glEnable(GL_POLYGON_OFFSET_FILL);
                if (renormalize) {
                        glEnable(GL_NORMALIZE);
                }
                glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
                glPushMatrix();
                drawFlattened(true);
                glPopMatrix();
        }
        glPopMatrix();
        glDisable(GL_LIGHTING);
        
        camera->RotateV(-camRotV);
        camera->RotateU(-camRotU);
        camera->RotateW(-camRotW);

        glutSwapBuffers();
}

void onExit()
{
        delete cube;
        delete cylinder;
        delete cone;
        delete sphere;
        delete camera;
        if (parser != NULL) {
                delete parser;
        }
}

/**************************************** main() ********************/

int main(int argc, char* argv[])
{
        atexit(onExit);

        if (argc > 1) {
                renormalize = true;
                fprintf(stderr, "Enabling renormalization\n");
        }

        /****************************************/
        /*   Initialize GLUT and create window  */
        /****************************************/

        glutInit(&argc, argv);
        glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
        glutInitWindowPosition(50, 50);
        glutInitWindowSize(500, 500);

        main_window = glutCreateWindow("COMP 175 Assignment 3");
        glutDisplayFunc(myGlutDisplay);
        glutReshapeFunc(myGlutReshape);

        glShadeModel (GL_SMOOTH);

        glEnable(GL_DEPTH_TEST);

        // Specular reflections will be off without this, since OpenGL calculates
        // specular highlights using an infinitely far away camera by default, not
        // the actual location of the camera
        glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, 1);

        // Show all ambient light for the entire scene (not one by default)
        GLfloat one[] = { 0.5f, 0.5f, 0.5f, 1.0f };
        glLightModelfv(GL_LIGHT_MODEL_AMBIENT, one);

        glPolygonOffset(1, 1);



        /****************************************/
        /*         Here's the GLUI code         */
        /****************************************/

        GLUI* glui = GLUI_Master.create_glui("GLUI");

        filenameTextField = new GLUI_EditText( glui, "Filename:", filenamePath);
        filenameTextField->set_w(300);
        glui->add_button("Load", 0, callback_load);
        
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

        GLUI_Spinner* eyex_widget = glui->add_spinner_to_panel(camera_panel, "EyeX:", GLUI_SPINNER_FLOAT, &eyeX);
        eyex_widget->set_float_limits(-10, 10);
        GLUI_Spinner* eyey_widget = glui->add_spinner_to_panel(camera_panel, "EyeY:", GLUI_SPINNER_FLOAT, &eyeY);
        eyey_widget->set_float_limits(-10, 10);
        GLUI_Spinner* eyez_widget = glui->add_spinner_to_panel(camera_panel, "EyeZ:", GLUI_SPINNER_FLOAT, &eyeZ);
        eyez_widget->set_float_limits(-10, 10);

        GLUI_Spinner* lookx_widget = glui->add_spinner_to_panel(camera_panel, "LookX:", GLUI_SPINNER_FLOAT, &lookX);
        lookx_widget->set_float_limits(-10, 10);
        GLUI_Spinner* looky_widget = glui->add_spinner_to_panel(camera_panel, "LookY:", GLUI_SPINNER_FLOAT, &lookY);
        looky_widget->set_float_limits(-10, 10);
        GLUI_Spinner* lookz_widget = glui->add_spinner_to_panel(camera_panel, "LookZ:", GLUI_SPINNER_FLOAT, &lookZ);
        lookz_widget->set_float_limits(-10, 10);

        glui->add_column(true);

        GLUI_Panel *render_panel = glui->add_panel("Render");
        new GLUI_Checkbox(render_panel, "Wireframe", &wireframe);
        new GLUI_Checkbox(render_panel, "Fill", &fillObj);
        (new GLUI_Spinner(render_panel, "Segments X:", &segmentsX))
                ->set_int_limits(3, 60);
        (new GLUI_Spinner(render_panel, "Segments Y:", &segmentsY))
                ->set_int_limits(3, 60);


        glui->add_button("Quit", 0, (GLUI_Update_CB)exit);

        glui->set_main_gfx_window(main_window);

        /* We register the idle callback with GLUI, *not* with GLUT */
        GLUI_Master.set_glutIdleFunc(myGlutIdle);

        glutMainLoop();

        return EXIT_SUCCESS;
}



