/*  =================== File Information =================
	File Name: spline.cpp
	Description:
	Author: Michael Shah
	Last Modified: 3/06/14

	Purpose: Create a cubic spline 
	Usage:	

	Further Reading resources: 
	http://mathworld.wolfram.com/CubicSpline.html
	http://www.geos.ed.ac.uk/~yliu23/docs/lect_spline.pdf
	===================================================== */
#include <GL/glui.h>
#include <iostream>
#include <cmath>
#include "spline.h"

/*	===============================================
Desc:	Default constructor for spline
Precondition: 	_resolution specifies how many points will be drawn between each control
				point for the spline.
Postcondition:
=============================================== */
spline::spline(int _resolution) {
    // Set to Null
    head = nullptr;
    tail = nullptr;

    resolution = _resolution;
    points = 0;
    computed_x_y_z = nullptr;
}

/*	===============================================
Desc:			Deallocate memory
Precondition:
Postcondition:
=============================================== */
spline::~spline() {
    // Delete all of our control points
    controlPoint *iter;
    iter = head;
    while (iter != nullptr) {
        controlPoint *next = iter->next;
        delete iter;
        iter = next;
    }

    delete[] computed_x_y_z;
}

/*	===============================================
Desc:			Adds a point to the linked list of control points
Precondition:	If it is the first point, make sure it is the head nad tail.
				If there is no memory available, terminate the program.
Postcondition:	
=============================================== */
void spline::addPoint(float x, float y, float z) {
    // Create a new point
    auto *temp = new controlPoint();
    temp->setValues(x, y, z);
    temp->next = nullptr;    // remember to set this to NULL since this is our last point in our list.
    std::cout << "Roller Coaster Route: (" << x << "," << y << "," << z
              << ")" << std::endl;
    points++;            // enumerate our point

    // If this is our first point, then it is our head and tail
    if (head == nullptr && tail == nullptr) {
        head = temp;
        head->next = tail;
        tail = temp;
        tail->next = nullptr;
    } else {
        tail->next = temp;
        tail = temp;
    }

    update(resolution);

}

/*	===============================================
Desc:	Reallocates memory for our resolution table.
		Note that as many possible resolution points are copied
		over to the new array.
Precondition:
Postcondition:
=============================================== */
void spline::update(int _resolution) {

    resolution = _resolution;
    // resize our array
    if (computed_x_y_z == nullptr) {
        computed_x_y_z = new float[resolution * points * 3 + 3];
    } else {    // resize! ewww, this is ugly though. We could speed up by doubling our size
        // everytime and keep track of the capacity of our array versus memory taken.
        delete[] computed_x_y_z;
        computed_x_y_z = new float[resolution * points * 3 + 3];
    }
    std::cout << "resolution: " << resolution << " points: " << points
              << " Array Table Size:" << resolution * points * 3 + 3
              << std::endl;
}

/*	===============================================
		Desc:			This function draws a spline by making opengl calls of the form
							glPointSize(1.0);
							glBegin(GL_POINTS);
								glVertex(...)
							glEnd();

						Within this function we could compute a table of of values
						(i.e. computed_x_y_z) and then never have to call this function again,
						so long as the resolution is not updated.

		Precondition: 	resolution(in my implementation), specifies how many points to draw between
						each control point.

						output is used as a flag to output the points along the spline to the console.
						A value of 1 will output each interpolated point, while a value of 0 will output
						nothing to standard IO.
						You may remove this is parameter if you think it is unnecessary.
		Postcondition:
=============================================== */
void spline::draw_spline(int resolution, int output) {

    glPointSize(5.0);
    glColor3f(1.0, 0.0, 0.0);
    glBegin(GL_POINTS);

    float timeStep;
    float Vs = 0.2;
    float Vg = 0.2;

    controlPoint *pt = this->head;
    if (pt == nullptr) {
        std::cerr << "Point list is empty" << std::endl;
        exit(12);
    }
    while (pt->next != nullptr) {
        for (int i = 0; i < resolution; i++) {
            timeStep = i * (1.0f / resolution);
            float x = calculate_Spline(timeStep, pt->x, pt->next->x, 1.5, 1.5);
            float y = calculate_Spline(timeStep, pt->y, pt->next->y, 1.5, 1.5);
            float z = calculate_Spline(timeStep, pt->z, pt->next->z, 1.5, 1.5);

            if (output == 1) {
                std::cerr << "Interpolated point: (" << x << ", " << y << ", " << z << ")" << std::endl;
            }

            glVertex3f(x, y, z);
        }

        pt = pt->next;
    }

    glEnd();

}

/*	===============================================
		Desc: 	Function for returning an interpolated point between and sPos(starting position) and
				an gPos (goal/ending) position.
				The velocity at the starting point (Vs) and goal point(Vg) can be user specified.
		
				S = starting position of your interval
				G = ending(or "goal") position of your interval
				Vs = velocity your points grow from from your starting point
				Vg = velocity your point ends on from your goal position

		Precondition:
		Postcondition:
=============================================== */
float spline::calculate_Spline(float t, float S, float G, float Vs, float Vg) {

    return (float) (
            S + (Vs * t) +
            (((-3.0 * S) + (3.0 * G) - (2.0 * Vs) - Vg) * (t * t)) +
            (((2.0 * S) - (2.0 * G) + Vs + Vg) * (t * t * t))
    );
}

/*	===============================================
Desc:	This function is responsible for drawing the original control points
		Use this as an example of how to iterate through the data structure.

		This function renders the control points and draws straight lines between them.
		You can use this function to compare with the draw_spline function.
Precondition:
Postcondition:
=============================================== */
void spline::render() {
    glPushMatrix();
    // Set our point sizes, these serve to highlight
    // the points that we added in our program.
    glPointSize(5.0);

    // Create an iterator to move through all of our control points
    controlPoint *iter;
    // Set the iterator to the head of our list.
    iter = head;

    // Disable lighting, so that lines and points show up.
    glDisable(GL_LIGHTING);
    // Iterate through each of the points until our iter reaches the end.
    while (iter != nullptr) {
        // Set the draw mode so that each vertex is drawn as a point.
        glBegin(GL_POINTS);
        // Our points will be drawn as red boxes
        glColor3f(1, 1, 1);
        // Color our start and stop positions different colors.
        if (iter == head) { glColor3f(0, 0, 1); }
        if (iter->next == nullptr) { glColor3f(0, 1, 1); }
        // Finally draw a regular control point.
        glVertex3f(iter->x, iter->y, iter->z);
        glEnd();
        // Continue iterating through our list.
        iter = iter->next;
    }

    // Reset our iter to the head
    // The purpose of this loop is to
    // draw lines between each of the control points.
    iter = head;
    while (iter->next != nullptr) {
        glBegin(GL_LINES);
        glColor3f(0, 1, 0);
        glVertex3f(iter->x, iter->y, iter->z);
        glVertex3f(iter->next->x, iter->next->y, iter->next->z);
        glEnd();
        iter = iter->next;
    }

    // Renable lighting now that we are done drawing lines and points.
    glEnable(GL_LIGHTING);
    glPopMatrix();
}

/*	===============================================
Desc:	renderAsRollerCoaster is a hacked together function.
		For this you get to be creative with how you could render two or more splines
		together in order to create cool shapes. 
		Perhaps you could draw a tunnel to go through?
		Perhaps you create an overhead roller coaster?
		Perhaps you have other ideas?
Precondition:
Postcondition:
=============================================== */
void spline::renderAsRollerCoaster(int resolution, float width) {
    /*


                   (Optional) You Implement this Code

   */

}

