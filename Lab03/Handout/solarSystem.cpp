#include <GL/glui.h>
#include "solarSystem.h"
#include <math.h>

solarSystem::solarSystem() {

}

solarSystem::~solarSystem() {

}

// Render this with push and pop operations
// or alternatively implement helper functions
// for each of the planets with parameters.
void solarSystem::render() {
        
        // Some ideas for constants that can be used for
        // rotating the planets.
        static float rotation = 0.1;
        static float rotation2 = 0.0;
        static double mrots[36] = {((double)rand()) / RAND_MAX, ((double)rand()) / RAND_MAX, ((double)rand()) / RAND_MAX,
                                   ((double)rand()) / RAND_MAX, ((double)rand()) / RAND_MAX, ((double)rand()) / RAND_MAX,
                                   ((double)rand()) / RAND_MAX, ((double)rand()) / RAND_MAX, ((double)rand()) / RAND_MAX,
                                   ((double)rand()) / RAND_MAX, ((double)rand()) / RAND_MAX, ((double)rand()) / RAND_MAX,
                                   ((double)rand()) / RAND_MAX, ((double)rand()) / RAND_MAX, ((double)rand()) / RAND_MAX,
                                   ((double)rand()) / RAND_MAX, ((double)rand()) / RAND_MAX, ((double)rand()) / RAND_MAX,
                                   ((double)rand()) / RAND_MAX, ((double)rand()) / RAND_MAX, ((double)rand()) / RAND_MAX,
                                   ((double)rand()) / RAND_MAX, ((double)rand()) / RAND_MAX, ((double)rand()) / RAND_MAX,
                                   ((double)rand()) / RAND_MAX, ((double)rand()) / RAND_MAX, ((double)rand()) / RAND_MAX,
                                   ((double)rand()) / RAND_MAX, ((double)rand()) / RAND_MAX, ((double)rand()) / RAND_MAX,
                                   ((double)rand()) / RAND_MAX, ((double)rand()) / RAND_MAX, ((double)rand()) / RAND_MAX,
                                   ((double)rand()) / RAND_MAX, ((double)rand()) / RAND_MAX, ((double)rand()) / RAND_MAX};
        static double mincs[36] = {((double)rand()) / RAND_MAX, ((double)rand()) / RAND_MAX, ((double)rand()) / RAND_MAX,
                                   ((double)rand()) / RAND_MAX, ((double)rand()) / RAND_MAX, ((double)rand()) / RAND_MAX,
                                   ((double)rand()) / RAND_MAX, ((double)rand()) / RAND_MAX, ((double)rand()) / RAND_MAX,
                                   ((double)rand()) / RAND_MAX, ((double)rand()) / RAND_MAX, ((double)rand()) / RAND_MAX,
                                   ((double)rand()) / RAND_MAX, ((double)rand()) / RAND_MAX, ((double)rand()) / RAND_MAX,
                                   ((double)rand()) / RAND_MAX, ((double)rand()) / RAND_MAX, ((double)rand()) / RAND_MAX,
                                   ((double)rand()) / RAND_MAX, ((double)rand()) / RAND_MAX, ((double)rand()) / RAND_MAX,
                                   ((double)rand()) / RAND_MAX, ((double)rand()) / RAND_MAX, ((double)rand()) / RAND_MAX,
                                   ((double)rand()) / RAND_MAX, ((double)rand()) / RAND_MAX, ((double)rand()) / RAND_MAX,
                                   ((double)rand()) / RAND_MAX, ((double)rand()) / RAND_MAX, ((double)rand()) / RAND_MAX,
                                   ((double)rand()) / RAND_MAX, ((double)rand()) / RAND_MAX, ((double)rand()) / RAND_MAX,
                                   ((double)rand()) / RAND_MAX, ((double)rand()) / RAND_MAX, ((double)rand()) / RAND_MAX};


        glPushMatrix();
        glTranslatef(0, 0, 0.1);
        glRotatef(rotation, 0, 1, 0);

        // The Sun
        glPushMatrix();
        glRotatef(rotation, 0, 1, 0);
        glColor3f(1.0f, fmod(rotation2 / 20, (2*3.14159265)), fmod(rotation / 1.5, (2*3.14159265)));
        glutSolidSphere(1.2, 50, 50);
        glPopMatrix();

        // Mercury
        glPushMatrix();
        glRotatef(rotation, 0, 1, 0);
        glTranslatef(2, 0, 0);
        glColor3f(0.26f, 0.85f, 0.26f);
        glutSolidSphere(0.05, 50, 50);
        glPopMatrix();

        // Venus
        glPushMatrix();
        glRotatef(rotation, 0, 1, 0);
        glTranslatef(0, 0, 3);
        glColor3f(0.26f, 0.85f, 0.26f);
        glutSolidSphere(0.10, 50, 50);
        glPopMatrix();

        // Eaerth
        glPushMatrix();
        glRotatef(rotation, 0, 1, 0);
        glTranslatef(0, 0, -4);
        glColor3f(0.26f, 0.85f, 0.26f);
        glutSolidSphere(0.08, 50, 50);
        //Moon
        glPushMatrix();
        glRotatef(rotation2, 0, 1, 0);
        glTranslatef(0, 0, 0.2);
        glColor3f(1.0f, 0.85f, 0.26f);
        glutSolidSphere(0.03, 50, 50);
        glPopMatrix();
        glPopMatrix();

        // /???
        glPushMatrix();
        glRotatef(rotation, 0, 1, 0);
        glTranslatef(-5, 0, 0);
        glColor3f(0.26f, 0.85f, 0.26f);
        glutSolidSphere(0.08, 50, 50);
        // Moon1
        glPushMatrix();
        glRotatef(rotation2, 0, 1, 0);
        glTranslatef(0, 0, 0.2);
        glColor3f(1.0f, 0.85f, 0.26f);
        glutSolidSphere(0.03, 50, 50);
        // Submoon 1
        glPushMatrix();
        glRotatef(rotation2, 0, 1, 0);
        glTranslatef(0, 0, 0.05);
        glColor3f(1.0f, 0.85f, 0.26f);
        glutSolidSphere(0.01, 50, 50);
        glPopMatrix();
        glPopMatrix();
        // Moon 2
        glPushMatrix();
        glRotatef(rotation2, 0, 1, 0);
        glTranslatef(0.2, 0, 0);
        glColor3f(1.0f, 0.85f, 0.26f);
        glutSolidSphere(0.03, 50, 50);
        glPopMatrix();
        glPopMatrix();

        //Jupiter
        glPushMatrix();
        glRotatef(rotation, 0, 1, 0);
        glTranslatef(-7, 0, 0);
        glColor3f(0.26f, 0.85f, 0.26f);
        glutSolidSphere(0.4, 50, 50);
        for (float i = 0.0; i < 36; i+=1.0) {
                glPushMatrix();
                glRotatef(mrots[int(roundf(i))], 0, 0, 1);
                glTranslatef(1.0 / 36.0 * i + 0.4, 0, 0);
                glColor3f(1.0f, 1.0/36.0 * i, 1.0f);
                glutSolidSphere(0.03, 50, 50);
                glPopMatrix();
        }
        glPopMatrix();

        glPushMatrix();
        glRotatef(rotation, 0, 1, 0);
        glTranslatef(-9, 0, 0);
        glColor3f(0.26f, 0.85f, 0.26f);
        glutSolidSphere(0.2, 50, 50);
        glPopMatrix();
        glPopMatrix();

        for (int i = 0; i < 36; i++) {
                mrots[i] += mincs[i];
        }
        rotation += 0.1;
        rotation2 += 1;
}
