#include <stdio.h>
#include <GLUT/glut.h>
#include <stdlib.h>  // для exit()

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

// Глобальные переменные для управления
float xangle = 0.0;
float yangle = 0.0;
float xpos = 0.0;
float ypos = 0.0;
float zpos = -2.0;

// Массив для отслеживания нажатых клавиш
bool keys[256] = {false};

void keyboardDown(unsigned char key, int x, int y) {
    // Выход по ESC (ASCII код 27)
    if (key == 27) {  // 27 = ESC
        exit(0);
    }
    keys[key] = true;
}

void keyboardUp(unsigned char key, int x, int y) {
    keys[key] = false;
}

void specialDown(int key, int x, int y) {
    keys[key] = true;
}

void specialUp(int key, int x, int y) {
    keys[key] = false;
}

void updateCamera() {
    // Движение W A S D
    if (keys['w'] || keys['W']) {
        zpos += 0.05;
    }
    if (keys['s'] || keys['S']) {
        zpos -= 0.05;
    }
    if (keys['a'] || keys['A']) {
        xpos -= 0.05;
    }
    if (keys['d'] || keys['D']) {
        xpos += 0.05;
    }
    
    // Движение вверх/вниз (пробел и E)
    if (keys[' ']  keys['Q']) {
        ypos -= 0.05;
    }
    if (keys['e'] || keys['E']) {
        ypos += 0.05;
    }
    
    // Вращение стрелками
    if (keys[GLUT_KEY_RIGHT]) {
        yangle += 2.0;
    }
    if (keys[GLUT_KEY_LEFT]) {
        yangle -= 2.0;
    }
    if (keys[GLUT_KEY_UP]) {
        xangle += 2.0;
    }
    if (keys[GLUT_KEY_DOWN]) {
        xangle -= 2.0;
    }
}

void display() {
    updateCamera();
    
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    
    gluLookAt(0, 0, 5, 0, 0, 0, 0, 1, 0);
    
    glTranslatef(xpos, ypos, zpos);
    glRotatef(yangle, 0, 1, 0);
    glRotatef(xangle, 1, 0, 0);
    
    drawCube(0.5);
    
    glutSwapBuffers();
    glutPostRedisplay();
}

void reshape(int w, int h) {
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();  // ← Здесь была ошибка: "чйча" вместо "glLoadIdentity()"
    gluPerspective(45.0, (double)w / (double)h, 0.1, 100.0);
    glMatrixMode(GL_MODELVIEW);
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(800, 600);
    glutCreateWindow("3D Cube Controller - Press ESC to exit");
    
    glutKeyboardFunc(keyboardDown);
    glutKeyboardUpFunc(keyboardUp);
    glutSpecialFunc(specialDown);
    glutSpecialUpFunc(specialUp);
    
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    
    glEnable(GL_DEPTH_TEST);
    
    glutMainLoop();
    return 0;
}
