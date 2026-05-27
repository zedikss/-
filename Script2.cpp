#include <stdio.h>
#include <GL/freeglut.h>
#include <GL/GLU.h>

void drawCube(float s) {
    float h = s * 0.5f;

    glBegin(GL_TRIANGLES);
    glColor3f(1.0f, 0.0f, 0.0f);
    glVertex3f(-h, -h, h); glVertex3f(h, -h, h); glVertex3f(h, h, h);
    glVertex3f(-h, -h, h); glVertex3f(h, h, h); glVertex3f(-h, h, h);

    glColor3f(0.0f, 1.0f, 0.0f);
    glVertex3f(h, -h, -h); glVertex3f(-h, -h, -h); glVertex3f(-h, h, -h);
    glVertex3f(h, -h, -h); glVertex3f(-h, h, -h); glVertex3f(h, h, -h);

    glColor3f(0.0f, 0.0f, 1.0f);
    glVertex3f(h, -h, h); glVertex3f(h, -h, -h); glVertex3f(h, h, -h);
    glVertex3f(h, -h, h); glVertex3f(h, h, -h); glVertex3f(h, h, h);

    glColor3f(1.0f, 1.0f, 0.0f);
    glVertex3f(-h, -h, -h); glVertex3f(-h, -h, h); glVertex3f(-h, h, h);
    glVertex3f(-h, -h, -h); glVertex3f(-h, h, h); glVertex3f(-h, h, -h);

    glColor3f(1.0f, 0.0f, 1.0f);
    glVertex3f(-h, h, h); glVertex3f(h, h, h); glVertex3f(h, h, -h);
    glVertex3f(-h, h, h); glVertex3f(h, h, -h); glVertex3f(-h, h, -h);

    glColor3f(0.0f, 1.0f, 1.0f);
    glVertex3f(-h, -h, -h); glVertex3f(h, -h, -h); glVertex3f(h, -h, h);
    glVertex3f(-h, -h, -h); glVertex3f(h, -h, h); glVertex3f(-h, -h, h);

    glEnd();
}

float xangle = 0.0;
float yangle = 0.0;
float xpos = 0.0;
float ypos = 0.0;
float zpos = 0.0;
void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glPushMatrix();
    if (GetAsyncKeyState('W') & 0x8000) {
        zpos += 0.005;
    }
    if (GetAsyncKeyState('S') & 0x8000) {
        zpos -= 0.005;
    }
    if (GetAsyncKeyState('A') & 0x8000) {
        xpos += 0.005;
    }
    if (GetAsyncKeyState('D') & 0x8000) {
        xpos -= 0.005;
    }
    if (GetAsyncKeyState(VK_SPACE) & 0x8000) {
        ypos -= 0.005;
    }
    if (GetAsyncKeyState(VK_LSHIFT) & 0x8000) {
        ypos += 0.005;
    }
    if (GetAsyncKeyState(VK_RIGHT) & 0x8000) {
        yangle += 0.05;
    }
    if (GetAsyncKeyState(VK_LEFT) & 0x8000) {
        yangle -= 0.05;
    }
    if (GetAsyncKeyState(VK_UP) & 0x8000) {
        xangle += 0.05;
    }
    if (GetAsyncKeyState(VK_DOWN) & 0x8000) {
        xangle -= 0.05;
    }

    glTranslatef(xpos, ypos, zpos);
    glRotatef(yangle, 0, 1, 0);
    glRotatef(xangle, 1, 0, 0);
    drawCube(0.5);
    glPopMatrix();

    glutSwapBuffers();
 glutPostRedisplay();
}

int main(int argc, char** argv) {
 glutInit(&argc, argv);
 glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
 glutInitWindowSize(800, 600);
 glutCreateWindow("title");

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0f, 1.0, 0.1f, 100.0f);

    glEnable(GL_DEPTH_TEST);

 glutDisplayFunc(display);
 glutMainLoop();
 return 0;
}
