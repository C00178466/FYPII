#include <iostream>
#include <stdlib.h>

#define GLUT_DISABLE_ATEXIT_HACK

#include "glut.h"
#include <Windows.h>
#include "Terrain.h"

using namespace std;

float angle = 60.0f;
Terrain* terrain;

void CleanUp()
{
	delete terrain;
}

void HandleInput(unsigned char key, int x, int y)
{
	switch (key)
	{
		case 27: //Escape key
			CleanUp();
			exit(0);
	}
}

void InitRenderingFunctions()
{
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_COLOR_MATERIAL);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_NORMALIZE);
	glShadeModel(GL_SMOOTH);
}

void HandleResize(int w, int h)
{
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45.0, (double)w / (double)h, 1.0, 200.0);
}

void Render()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(0.0f, 0.0f, -10.0f);
	glRotatef(30.0f, 1.0f, 0.0f, 0.0f);
	glRotatef(-angle, 0.0f, 1.0f, 0.0f);
	
	GLfloat ambientColor[] = {0.4f, 0.4f, 0.4f, 1.0f};
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambientColor);
	
	GLfloat lightColor0[] = {0.6f, 0.6f, 0.6f, 1.0f};
	GLfloat lightPos0[] = {-0.5f, 0.8f, 0.1f, 0.0f};
	glLightfv(GL_LIGHT0, GL_DIFFUSE, lightColor0);
	glLightfv(GL_LIGHT0, GL_POSITION, lightPos0);
	
	float scale = 5.0f / max(terrain->width() - 1, terrain->length() - 1);
	glScalef(scale, scale, scale);
	glTranslatef(-(float)(terrain->width() - 1) / 2, 0.0f, -(float)(terrain->length() - 1) / 2);
	
	glColor3f(0.3f, 0.9f, 0.0f);
	for(int z = 0; z < terrain->length() - 1; z++)
	{
		//Makes OpenGL draw a triangle at every three consecutive vertices
		glBegin(GL_TRIANGLE_STRIP);

		for(int x = 0; x < terrain->width(); x++)
		{
			Vec3f normal = terrain->getNormal(x, z);
			glNormal3f(normal[0], normal[1], normal[2]);
			glVertex3f(x, terrain->getHeight(x, z), z);
			normal = terrain->getNormal(x, z + 1);
			glNormal3f(normal[0], normal[1], normal[2]);
			glVertex3f(x, terrain->getHeight(x, z + 1), z + 1);
		}

		glEnd();
	}
	
	glutSwapBuffers();
}

void Update(int value)
{

	//increment angle to rotate terrain (For Demo Purposes)
	angle += 1.0f;

	if (angle > 360)
	{
		angle -= 360;
	}
	
	glutPostRedisplay(); // Update Window
	glutTimerFunc(25, Update, 0); // Call Update every 25 milliseconds
}

int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(600, 600);
	
	glutCreateWindow("Terrain");
	InitRenderingFunctions();
	
	terrain = Terrain::loadTerrain("heightmap.bmp", 20);
	
	glutDisplayFunc(Render);
	glutKeyboardFunc(HandleInput);
	glutReshapeFunc(HandleResize);
	glutTimerFunc(25, Update, 0);
	
	glutMainLoop();

	return 0;
}









