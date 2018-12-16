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

#include "../Leaves/SimplexNoise.h"

#define SPACE 32
#define ENTER 13

SimplexNoise *noise = new SimplexNoise();

float dispSize = 0.00390625f;
float scale = 20;
int tileSize = 32;

float *height;
int maxX = round(2.f / dispSize);
int maxY = round(1.f / dispSize);

float leafDecay = 0.5;

float t = 0, tleaf = 0;

bool fallOff = true;
float perc = 0.2f;

unsigned int delta;
float timeStep = 1;

float fall = 30;

float offX=0, offY=0;

void quad(unsigned char a, unsigned char r, unsigned char g, unsigned char b,
	double x, double y, double width, double height) {
	glBegin(GL_QUADS);
	glColor4f(r / 255.f, g / 255.f, b / 255.f, a / 255.f);
	double _x = x / tileSize * dispSize - 1;
	double _y = y / tileSize * dispSize * 2 - 1;
	double dispSizeX = dispSize * width / tileSize;
	double dispSizeY = dispSize * height / tileSize * 2;
	glVertex2f(_x, _y);
	glVertex2f(_x + dispSizeX, _y);
	glVertex2f(_x + dispSizeX, _y + dispSizeY);
	glVertex2f(_x, _y + dispSizeY);
	glEnd();
}
void rect(unsigned int col, double x, double y, double width, double height) {
	quad((col & 0xFF000000) >> 24, (col & 0xFF0000) >> 16, (col & 0xFF00) >> 8,
		col & 0xFF, x, y, width, height);
}

double getFalloff(double i, double j) {
	if (i <= j)
		return i / j;
	if (i >= 1.f - j)
		return 1.f - (i - (1.f - j)) / j;
	return 1;
}

double calculateFalloff(double x, double y, double perc) {
	return getFalloff(x, perc) * getFalloff(y, perc);
}

void setHeightMap() {
	for (int i = 0; i < maxX; i++)
		for (int j = 0; j < maxY; j++) {
			height[i + j * maxX] = (noise->noise((i+offX) / maxX * scale,
				(j + offY) / maxY * scale, t) + 1) / 2;
			if (fallOff) {
				float fall = calculateFalloff((float)i / maxX,
					(float)j / maxY, perc);
				height[i + j * maxX] *= fall;
			}
		}
	//Spherical fall off + tnoise
}
void update(float deltaTime) {
	t += deltaTime * timeStep;
	tleaf += deltaTime * timeStep;
	while (tleaf >= fall)tleaf -= fall;
	setHeightMap();
}
void cube(int i, int j) {
	float h = height[j * maxX + i];
	if (h < leafDecay) {
		float perc = h / leafDecay;
		float perc2 = tleaf / fall;
		rect((unsigned int)(perc * perc * (1 - perc2) * 0xFF) << 24 | 0x0B640B, i * tileSize,
			j * tileSize, tileSize, tileSize);
		rect((unsigned int)(perc * perc * perc2 * 0xFF) << 24 | 0x802B03, i * tileSize,
			j * tileSize, tileSize, tileSize);
	}
	else {
		float perc = (h - leafDecay) / (1 - leafDecay);
		float perc2 = tleaf / fall;
		rect((unsigned int)(perc * (1 - perc2) * 0xFF) << 24 | 0x00FF33, i * tileSize,
			j * tileSize, tileSize, tileSize);
		rect((unsigned int)((1.0 - perc) * (1 - perc2) * 0xFF) << 24 | 0x0B640B,
			i * tileSize, j * tileSize, tileSize, tileSize);
		rect((unsigned int)(perc * perc2 * 0xFF) << 24 | 0xB4360B, i * tileSize,
			j * tileSize, tileSize, tileSize);
		rect((unsigned int)((1.0 - perc) * perc2 * 0xFF) << 24 | 0x802B03,
			i * tileSize, j * tileSize, tileSize, tileSize);
	}
}
void draw() {
	if (height != nullptr)
		for (int i = 0; i < maxX; i++)
			for (int j = 0; j < maxY; j++)
				cube(i, j);
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
		height = new float[maxX*maxY];
		setHeightMap();
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
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
		leafDecay -= 0.01;
		if (leafDecay < 0)leafDecay = 0;
	}
	if (c == 'w') {
		leafDecay += 0.01;
		if (leafDecay >= 1)leafDecay = 1;
	}
	if (c == 'a') timeStep /= 1.05;
	if (c == 'd')  timeStep *= 1.05;
	if (c == '=') scale /= 1.05;
	if (c == '-')  scale *= 1.05;
}
void keySpecial(int button, int x, int y) {
	if (button == GLUT_KEY_RIGHT) offX += 2.5;
	else if (button == GLUT_KEY_LEFT) offX -= 2.5;
	if (button == GLUT_KEY_UP) offY += 2.5;
	else if (button == GLUT_KEY_DOWN) offY -= 2.5;
}
void cleanUp() {
	delete[] height;
}

int main(int argc, char** argv) {
	glutInit(&argc, argv);
	glutInitWindowPosition(0, 0);
	glutInitWindowSize(640, 360);
	glutCreateWindow("Perlin noise leaves");
	glutDisplayFunc(display);
	glutIdleFunc(loop);
	glutKeyboardFunc(keypress);
	glutSpecialFunc(keySpecial);
	glutMainLoop();
	return 0;
}