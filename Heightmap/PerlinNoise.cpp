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

#include "../Heightmap/SimplexNoise.h"

#define SPACE 32
#define ENTER 13

SimplexNoise *noise = new SimplexNoise();

double dispSize = 0.005;
double scale = 20;
int tileSize = 32;

int *height;
int maxX = round(2.0 / dispSize);
int maxY = round(1.0 / dispSize);
int maxH = 128;
int waterLevel = maxH / 5 * 3;

double t = 0;

bool fallOff = true;
double perc = 0.2;

unsigned int delta;
double timeStep = 1;

double offX=0, offY=0;

void quad(unsigned char a, unsigned char r, unsigned char g, unsigned char b,
	double x, double y, double width, double height) {
	glBegin(GL_QUADS);
	glColor4f(r / 255.0f, g / 255.0f, b / 255.0f, a / 255.0f);
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
	if (i >= 1.0 - j)
		return 1.0 - (i - (1.0 - j)) / j;
	return 1;
}

double calculateFalloff(double x, double y, double perc) {
	return getFalloff(x, perc) * getFalloff(y, perc);
}

void setHeightMap() {
	for (int i = 0; i < maxX; i++)
		for (int j = 0; j < maxY; j++) {
			height[i + j * maxX] = (noise->noise((i+offX) / maxX * scale,
				(j + offY) / maxY * scale, t) + 1) / 2 * maxH;
			if (fallOff) {
				double fall = calculateFalloff((double)i / maxX,
					(double)j / maxY, perc);
				height[i + j * maxX] *= fall;
			}
		}
}
void update(float deltaTime) {
	t += deltaTime * timeStep;
	setHeightMap();
}
void cube(int i, int j) {
	int h = height[j * maxX + i];
	unsigned int col = 0;
	int stoneLevel = (maxH - waterLevel) / 4 * 3 + waterLevel;
	if (h < waterLevel) {
		double perc = (double)h / waterLevel;
		rect((unsigned int)(perc * 0xFF) << 24 | 0x2E99F3, i * tileSize,
			j * tileSize, tileSize, tileSize);
		rect((unsigned int)((1.0 - perc) * 0xFF) << 24 | 0x1069B3,
			i * tileSize, j * tileSize, tileSize, tileSize);
		return;
	}
	else if (h < stoneLevel) {
		double perc = (double)(h - waterLevel) / (stoneLevel - waterLevel);
		rect((unsigned int)(perc * 0xFF) << 24 | 0x8C8C8C, i * tileSize,
			j * tileSize, tileSize, tileSize);
		rect((unsigned int)((1.0 - perc) * 0xFF) << 24 | 0x008300,
			i * tileSize, j * tileSize, tileSize, tileSize);
		return;
	}
	else {
		double perc = (double)(h - stoneLevel) / (maxH - stoneLevel);
		rect((unsigned int)(perc * 0xFF) << 24 | 0xFFFFFF, i * tileSize,
			j * tileSize, tileSize, tileSize);
		rect((unsigned int)((1.0 - perc) * 0xFF) << 24 | 0x8C8C8C,
			i * tileSize, j * tileSize, tileSize, tileSize);
		return;
	}
	rect(col, i * tileSize, j * tileSize, tileSize, tileSize);
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
		height = new int[maxX*maxY];
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
		waterLevel -= 1;
		if (waterLevel < 0)waterLevel = 0;
	}
	if (c == 'w') {
		waterLevel += 1;
		if (waterLevel >= maxH)waterLevel = maxH-1;
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
	glutCreateWindow("Perlin test by Niels Brunekreef; 3D (x,y,t)");
	glutDisplayFunc(display);
	glutIdleFunc(loop);
	glutKeyboardFunc(keypress);
	glutSpecialFunc(keySpecial);
	glutMainLoop();
	return 0;
}