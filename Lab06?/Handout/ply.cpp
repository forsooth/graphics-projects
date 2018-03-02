/*  =================== File Information =================
  File Name: ply.cpp
  Description:
  Author: Matt Asnes & Fury Sheron

(Hot pizza is better, of course)

  ===================================================== */
#define _CRT_SECURE_NO_WARNINGS
#include <string>
#include <stdio.h>
#include <cstdlib>
#include <math.h>
#include <limits>
#include <GL/glui.h>
#include "ply.h"
#include "geometry.h"
#include "entity.h"


using namespace std;
ifstream myfile;
string filePath;
int vertexCount;
int faceCount;
/*  ===============================================
          Desc: Default constructor for a ply object
          Precondition:
          Postcondition:
          =============================================== */
ply::ply(string _filePath) {
        filePath = _filePath;
        vertexList = NULL;
        faceList = NULL;
        properties = 0;
        // Call helper function to load geometry
        loadGeometry();
}

/*  ===============================================
          Desc: Destructor for a ply object
          Precondition: Memory has been already allocated
          Postcondition:
          =============================================== */
ply::~ply(){
        // Delete the allocated faceList and vertexList arrays
        // and set both pointers to NULL
        delete[] vertexList;

        for (int i = 0; i < faceCount; i++) {
                delete[] faceList[i].vertexList;
        }

        delete[] faceList;
        vertexList = NULL;
        faceList = NULL;
}

/*  ===============================================
          Desc: reloads the geometry for a 3D object
          Precondition: Memory has been allocated and loaded, 
                                        _filePath is a .ply file.
          Postcondition: Memory represents the ply file given by
                                        _filePath.  
          =============================================== */
void ply::reload(string _filePath){
        filePath = _filePath;
        // reclaim memory allocated in each array
        delete[] vertexList;

        for (int i = 0; i < faceCount; i++) {
                delete[] faceList[i].vertexList;
        }

        delete[] faceList;
        // Set pointers to array
        vertexList = NULL;
        faceList = NULL;
        // Call our function again to load new vertex and face information.
        loadGeometry();
}


/*  ===============================================
          Desc: Parses a file and loads vertices into vertexList and faceList.
          Precondition:
          Postcondition:
          =============================================== */


void ply::loadGeometry(){

        /* You will implement this section of code
        1. Parse the header
        2.) Update any private or helper variables in the ply.h private section
        3.) allocate memory for the vertexList
        3a.) Populate vertices
        4.) allocate memory for the faceList
        4a.) Populate faceList
        */


        ifstream myfile(filePath.c_str()); // load the file
        if (myfile.is_open()) { // if the file is accessable
                properties = -2; // set the properties because there are extras labeled

                string line;
                char * token_pointer;
                char * lineCopy = new char[80];
                int count;
                bool reading_header = true;
                // loop for reading the header 
                while (reading_header && getline(myfile, line)) {

                        // get the first token in the line, this will determine which
                        // action to take. 
                        strcpy(lineCopy, line.c_str());
                        token_pointer = strtok(lineCopy, " \r");
                        // case when the element label is spotted:
                        if (strcmp(token_pointer, "element") == 0){
                                token_pointer = strtok(NULL, " ");

                                // When the vertex token is spotted read in the next token
                                // and use it to set the vertexCount and initialize vertexList
                                if (strcmp(token_pointer, "vertex") == 0){
                                        token_pointer = strtok(NULL, " ");
                                        vertexCount = atoi(token_pointer);
                                        vertexList = new vertex[vertexCount];
                                        vertexList_bak = new vertex[vertexCount];
                                }

                                // When the face label is spotted read in the next token and 
                                // use it to set the faceCount and initialize faceList.
                                else if (strcmp(token_pointer, "face") == 0){
                                        token_pointer = strtok(NULL, " ");
                                        faceCount = atoi(token_pointer);
                                        faceList = new face[faceCount];
                                }
                        }
                        // if property label increment the number of properties.
                        else if (strcmp(token_pointer, "property") == 0) { properties++; }
                        // if end_header break the header loop and move to reading vertices.
                        else if (strcmp(token_pointer, "end_header") == 0) { reading_header = false; }
                }

                // Read in exactly vertexCount number of lines after reading the header
                // and set the appropriate vertex in the vertexList.
                for (int i = 0; i < vertexCount; i++){

                        getline(myfile, line);
                        //TODO
                        //if (chars == -1) {
                        //      break;
                        //}
                        strcpy(lineCopy, line.c_str());

                        // depending on how many properties there are set that number of 
                        // elements (x, y, z, confidence, intensity, r, g, b) (max 7) with
                        // the input given
                        if (properties >= 0){
                                vertexList[i].x = atof(strtok(lineCopy, " \r"));
                        }
                        if (properties >= 1){
                                vertexList[i].y = atof(strtok(NULL, " \r"));
                        }
                        if (properties >= 2){
                                vertexList[i].z = atof(strtok(NULL, " \r"));
                        }
                        if (properties >= 3){
                                vertexList[i].confidence = atof(strtok(NULL, " \r"));
                        }
                        if (properties >= 4){
                                vertexList[i].intensity = atof(strtok(NULL, " \r"));
                        }
                        if (properties >= 5){
                                vertexList[i].r = atof(strtok(NULL, " \r"));
                        }
                        if (properties >= 6) {
                                vertexList[i].g = atof(strtok(NULL, " \r"));
                        }
                        if (properties >= 7) {
                                vertexList[i].b = atof(strtok(NULL, " "));
                        }
                }

                // Read in the faces (exactly faceCount number of lines) and set the 
                // appropriate face in the faceList
                for (int i = 0; i < faceCount; i++){

                        getline(myfile, line);

                        strcpy(lineCopy, line.c_str());
                        count = atoi(strtok(lineCopy, " "));
                        faceList[i].vertexCount = count; // number of vertices stored 
                        faceList[i].vertexList = new int[count]; // initialize the vertices

                        // set the vertices from the input, reading only the number of 
                        // vertices that are specified
                        for (int j = 0; j < count; j++){
                                faceList[i].vertexList[j] = atoi(strtok(NULL, " "));
                        }
                }
                delete[] lineCopy;
                myfile.close();
                scaleAndCenter();
        }
        // if the path is invalid, report then exit.
        else {
                cout << "cannot open file " << filePath.c_str() << "\n";
                //exit(1);
        }

        for (int i = 0; i < vertexCount; i++) {
            vertexList_bak[i] = vertexList[i];
        }
};

/*  ===============================================
Desc: Moves all the geometry so that the object is centered at 0, 0, 0 and 
    scaled to be between 0.5 and -0.5 
Precondition: after all the vetices and faces have been loaded in
Postcondition:
=============================================== */
void ply::scaleAndCenter() {
        float max_x = std::numeric_limits<float>::min();
        float max_y = std::numeric_limits<float>::min();
        float max_z = std::numeric_limits<float>::min();
        float min_x = std::numeric_limits<float>::max();
        float min_y = std::numeric_limits<float>::max();
        float min_z = std::numeric_limits<float>::max();

        for (int i = 0; i < vertexCount; i++) {
                vertex v = vertexList[i];
                if (v.x > max_x) max_x = v.x;
                if (v.y > max_y) max_y = v.y;
                if (v.z > max_z) max_z = v.z;
                if (v.x < min_x) min_x = v.x;
                if (v.y < min_y) min_y = v.y;
                if (v.z < min_z) min_z = v.z;
        }

        float center_x = (max_x + min_x) / 2;
        float center_y = (max_y + min_y) / 2;
        float center_z = (max_z + min_z) / 2;

        for (int i = 0; i < vertexCount; i++) {
                vertexList[i].x -= center_x;
                vertexList[i].y -= center_y;
                vertexList[i].z -= center_z;
        }

        float max = fmax(max_x - min_x, max_y - min_y);
        max = fmax(max, max_z - min_z);

        float scale_factor = 0.5 / max;

        for (int i = 0; i < vertexCount; i++) {
                vertexList[i].x *= scale_factor;
                vertexList[i].y *= scale_factor;
                vertexList[i].z *= scale_factor;
        }
}

/*  ===============================================
Desc:   Finds the normal to three vertices(1 face making up a triangle)
Precondition:
Postcondition:  Calls 'glNormal3f()' which takes in 3 parameters
=============================================== */
void ply::setNormal(float x1, float y1, float z1,
        float x2, float y2, float z2,
        float x3, float y3, float z3) {


        float v1x, v1y, v1z;
        float v2x, v2y, v2z;
        float cx, cy, cz;

        //find vector between x2 and x1
        v1x = x1 - x2;
        v1y = y1 - y2;
        v1z = z1 - z2;

        //find vector between x3 and x2
        v2x = x2 - x3;
        v2y = y2 - y3;
        v2z = z2 - z3;

        //cross product v1xv2

        cx = v1y * v2z - v1z * v2y;
        cy = v1z * v2x - v1x * v2z;
        cz = v1x * v2y - v1y * v2x;

        //normalize

        float length = sqrt(cx * cx + cy * cy + cz * cz);
        cx = cx / length;
        cy = cy / length;
        cz = cz / length;

        glNormal3f(cx, cy, cz);
}

void ply::skew(float *stretches){
        for (int i = 0; i < vertexCount; i++) {
            vertexList[i].x = vertexList_bak[i].x * stretches[0];
            vertexList[i].y = vertexList_bak[i].y * stretches[1];
            vertexList[i].z = vertexList_bak[i].z * stretches[2];
        }
}

/*  ===============================================
          Desc: Draws a filled 3D object
          Precondition:
          Postcondition:
          Error Condition: If we haven't allocated memory for our
          faceList or vertexList then do not attempt to render.
          =============================================== */
void ply::render(){
        // Given a triangle so that you see something on the scene
        glBegin(GL_TRIANGLES);
        for (int i = 0; i < faceCount; i++) {
                vertex v1 = vertexList[faceList[i].vertexList[0]];
                vertex v2 = vertexList[faceList[i].vertexList[1]];
                vertex v3 = vertexList[faceList[i].vertexList[2]];
                setNormal(v1.x, v1.y, v1.z, v2.x, v2.y, v2.z, v3.x, v3.y, v3.z);
                glVertex3f(v1.x, v1.y, v1.z);
                glVertex3f(v2.x, v2.y, v2.z);
                glVertex3f(v3.x, v3.y, v3.z);
        }
        glEnd();

        if(vertexList==NULL || faceList==NULL){
        return;
        }

}

/*  ===============================================
  Desc: Draws the wireframe(edges) of a 3D object.
  We could alternatively use 'glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);'

  Note, having two rendering functions is too much, but think
  about how you could modularize this, or other interseting
  ways of rendering. Could you render an object as points?
  What about rendering quads versus triangles, or using triangle
  strips if you know something about the data?

  Note that here we also do not set the normals for lighting. This way our
  geometry could load faster, perhaps different functions could
  have different special effects.

  Precondition:
  Postcondition:
  =============================================== */
void ply::renderWireFrame(){
        /* This is optional */
}


/*  ===============================================
          Desc: Prints some statistics about the file you have read in
          This is useful for debugging information to see if we parse our file
      correctly.

          Precondition:
          Postcondition:
          =============================================== */
void ply::printAttributes(){
        cout << "==== ply Mesh Attributes=====" << endl;
        cout << "vertex count:" << vertexCount << endl;
        cout << "face count:" << faceCount << endl;
        cout << "properties:" << properties << endl;
}

/*  ===============================================
          Desc: Iterate through our array and print out each vertex.

          Precondition:
          Postcondition:
          =============================================== */
void ply::printVertexList(){
        if (vertexList == NULL){
                return;
        }
        else{
                for (int i = 0; i < vertexCount; i++){
                        cout << vertexList[i].x << "," << vertexList[i].y;
            cout << "," << vertexList[i].z << endl;
                }
        }
}

/*  ===============================================
          Desc: Iterate through our array and print out each face.

          Precondition:
          Postcondition:
          =============================================== */
void ply::printFaceList(){
        if (faceList == NULL){
                return;
        }
        else{
                // For each of our faces
                for (int i = 0; i < faceCount; i++){
                        // Get the vertices that make up each face from
                        // the face list
                        for (int j = 0; j < faceList[i].vertexCount; j++){
                                // Print out the vertex
                                int index = faceList[i].vertexList[j];
                                cout << vertexList[index].x 
                                     << "," << vertexList[index].y;
                cout << "," << vertexList[index].z << endl;
                        }
                }
        }
}
