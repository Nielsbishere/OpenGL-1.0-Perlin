/*
* above.cpp
*
*Created on: Jul 7, 2016
*Author: Niels Brunekreef
*
*
* SimplexNoise from: http://staffwww.itn.liu.se/~stegu/simplexnoise/SimplexNoise.java
* 		Author: Stefan Gustavson
*		Function: Perlin noise
*		Rewritten: Niels Brunekreef (C++)
*		Added functionality: Niels Brunekreef through Sebastian Lague
*/

using namespace std;

#include <iostream>
#include <ctime>

#include <GL/freeglut.h>

#include "../Wave/SimplexNoise.h"

#define SPACE 32
#define ENTER 13

SimplexNoise *noise = new SimplexNoise();

double scale = 1;

double *height;
int maxH = 128;
int waveHeight = maxH/2;

double t = 0;

unsigned int delta;
double timeStep = 1;

double offX=0;

unsigned int points = 1920;

void setHeightMap() {
	for (int i = 0; i < points; i++)
		height[i] = (noise->noise((i+offX) / points * scale, t) + 1) / 2 * waveHeight;
}
void update(float deltaTime) {
	t += deltaTime * timeStep;
	setHeightMap();
}
void draw() {
	if (height != nullptr) {
		glColor3f(27 / 255.0, 128 / 255.0, 227 / 255.0);
		glBegin(GL_POLYGON);
		glVertex2f(-1, -1);
		glVertex2f(-1, height[0] / maxH * 2 - 1);
		for (unsigned int i = 0; i < points; i++)
			glVertex2f(2.0 / points*i - 1, (double)height[i] / maxH * 2 - 1);
		glVertex2f(1, height[points-1] / maxH * 2 - 1);
		glVertex2f(1, -1);
		glEnd();
	}
}

void display() {
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	draw();
	glFlush();
}

bool init;
void loop() {
	if (!init) {
		delta = glutGet(GLUT_ELAPSED_TIME);
		init = true;
		height = new double[points];
		setHeightMap(); 
		glDisable(GL_DEPTH_TEST);
		return;
	}
	float deltaTime = (glutGet(GLUT_ELAPSED_TIME) - delta) / 1000.0;
	update(deltaTime);
	delta = glutGet(GLUT_ELAPSED_TIME);
	glutPostRedisplay();
	Sleep(1);
}

void keypress(unsigned char c, int x, int y) {
	if (c == 's') {
		waveHeight -= 1;
		if (waveHeight < 0)waveHeight = 0;
	}
	if (c == 'w') {
		waveHeight += 1;
		if (waveHeight >= maxH)waveHeight = maxH-1;
	}
	if (c == 'a') timeStep /= 1.05;
	if (c == 'd')  timeStep *= 1.05;
	if (c == '=') scale /= 1.05;
	if (c == '-')  scale *= 1.05;
}
void keySpecial(int button, int x, int y) {
	if (button == GLUT_KEY_RIGHT) offX += 2.5;
	else if (button == GLUT_KEY_LEFT) offX -= 2.5;
}
void cleanUp() {
	delete[] height;
}

int main(int argc, char** argv) {
	glutInit(&argc, argv);
	glutInitWindowPosition(0, 0);
	glutInitWindowSize(640, 360);
	glutCreateWindow("Perlin test by Niels Brunekreef; 2D (x,t)");
	glutDisplayFunc(display);
	glutIdleFunc(loop);
	glutKeyboardFunc(keypress);
	glutSpecialFunc(keySpecial);
	glutMainLoop();
	return 0;
}