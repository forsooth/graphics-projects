/*  =================== File Information =================
        File Name: ply.h
        Description:
        Author: Michael Shah

        Purpose:        Specification for using
        Examples:       See example below for using PLY class
        ===================================================== */
#ifndef PLY_H
#define PLY_H

#include <string>
#include "geometry.h"
#include "entity.h"

using namespace std;

/*  ============== ply ==============
        Purpose: Load a PLY File

        Note that the ply file inherits from a base class called 'entity'
        Some important data structures are described in geometry.h
        This class stores common transformations that can be applied to 3D entities(such as mesh files, lights, or cameras)

        Example usage: 
        1.) ply* myPLY = new ply (filenamePath);
        2.) myPLY->render();
        3.) delete myPLY;
        ==================================== */ 
class ply : public entity{

        public:
                /*      ===============================================
                        Desc: Default constructor for a ply object
                        =============================================== */ 
                ply(string _filePath);

                /*      ===============================================
                        Desc: Destructor for a ply object
                        =============================================== */ 
                void deconstruct();
                ~ply();
                /*      ===============================================
                        Desc: reloads the geometry for a 3D object
                        (usually to see a new .ply file)
                =============================================== */ 
                void reload(string _filePath);
                /*      ===============================================
                        Desc: Draws a filled 3D object
                =============================================== */  
                void render();
                //iterates through the geometry to fill in the edgeList
                void findEdges();
                //draws the silhouette around the ply object
                void renderSilhouette();

                /*      ===============================================
                        Desc: Prints some statistics about the file you have read in
                =============================================== */  
                void printAttributes();
                /*  ===============================================
                        Desc: Helper function for you to debug if 
                        you are reading in the correct data.
                        (Generally these would not be public functions,
                        they are here to help you understand the interface)
                        =============================================== */
                void printVertexList();
                void printFaceList();
                
                //components of look vector (changeable by rotation around Y)
                float lookX;//0.0 when Y-rotation = 0
                float lookZ;//1.0 when Y-rotation = 0

        private:
                /*      ===============================================
                        Desc: Helper function used in the constructor
                        =============================================== */ 
                void loadGeometry();
                //makes the points fit in the window
                void scaleAndCenter();
                //calculates the normal, sends it to graphics card, 
                //NOTE and stores it
                void setNormal(int facenum, float x1, float y1, float z1,
                                                float x2, float y2, float z2,
                                                float x3, float y3, float z3);

                /*      ===============================================
                        Data
                        These variables are useful to store information
                        about the mesh we are loading.  Often these values
                        are stored in the header, or can be useful for
                        debugging.
                        =============================================== */
                // Store the path to our file
                string filePath;
                // Stores the number of vertics loaded
                int vertexCount;
                // Stores the number of faces loaded
                int faceCount;
				// Stores the number of edges loaded
				int edgeCount;
				// Tells us how many properites exist in the file
                int properties;
                // A dynamically allocated array that stores
                // a vertex
                vertex* vertexList;
                // A dynamically allocated array that stores
                // a list of faces (essentially integers that will
                // be looked up from the vertex list)
                face* faceList;
                //NOTE EDGE LIST IS NEW
                //an array of linked lists representing edges
                //the array is indexed by the lower-numbered vertex in the edge.
                edge* edgeList;
};

#endif