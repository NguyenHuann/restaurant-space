#include <glad/glad.h>
#include <GL/freeglut.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "shader.h"
#include "camera.h"
#include "basic_camera.h"
#include "table_chair.h"
//#include "stool.h"
#include "fan.h"
#include <iostream>

using namespace std;

// FreeGLUT callbacks
void display();
void reshape(int width, int height);
void keyboardFunc(unsigned char key, int x, int y);
void mouseMotionFunc(int x, int y);
void mouseButtonFunc(int button, int state, int x, int y);
void idleFunc();

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// modelling transform
float rotateAngle_X = 0;
float rotateAngle_Y = 0;
float rotateAngle_Z = 0;
float rotateAxis_X = 0.0;
float rotateAxis_Y = 0.0;
float rotateAxis_Z = 1.0;
float translate_X = 0.0;
float translate_Y = 0.0;
float translate_Z = 0.0;
float scale_X = 1.0;
float scale_Y = 1.0;
float scale_Z = 1.0;
bool fan_turn = false;
bool rotate_around = false;

// camera
Camera camera(glm::vec3(0.0f, 2.5f, 3.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;
bool rightMousePressed = false;

float eyeX = 0.0, eyeY = 1.0, eyeZ = 3.0;
float lookAtX = 0.0, lookAtY = 0.0, lookAtZ = 0.0;
glm::vec3 V = glm::vec3(0.0f, 1.0f, 0.0f);
BasicCamera basic_camera(eyeX, eyeY, eyeZ, lookAtX, lookAtY, lookAtZ, V);

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

// Global shader & VAOs (needed by display callback)
Shader* ourShader = nullptr;
int fanAngle = 0;

// VAOs
unsigned int VAO, VAO2, VAO3, VAO4, VAO5;
unsigned int VAOG, VAOW1, VAOW2, VAOB, VAOC, VAOT;
unsigned int VAOF1, VAOF2, VAOF3;
unsigned int VAOdec, VAOdec2, VAOdec3, VAOdec4, VAOdec5, VAOdec6;
Table_Chair table_chair[3];

glm::mat4 transformation(float tx, float ty, float tz, float rx, float ry, float rz, float sx, float sy, float sz) {
	glm::mat4 identityMatrix = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first
	glm::mat4 translateMatrix, rotateXMatrix, rotateYMatrix, rotateZMatrix, scaleMatrix, model;
	translateMatrix = glm::translate(identityMatrix, glm::vec3(tx, ty, tz));
	rotateXMatrix = glm::rotate(identityMatrix, glm::radians(rx), glm::vec3(1.0f, 0.0f, 0.0f));
	rotateYMatrix = glm::rotate(identityMatrix, glm::radians(ry), glm::vec3(0.0f, 1.0f, 0.0f));
	rotateZMatrix = glm::rotate(identityMatrix, glm::radians(rz), glm::vec3(0.0f, 0.0f, 1.0f));
	scaleMatrix = glm::scale(identityMatrix, glm::vec3(sx, sy, sz));
	model = translateMatrix * rotateXMatrix * rotateYMatrix * rotateZMatrix * scaleMatrix;
	return model;
}




void cylinder(Shader& ourShader, unsigned int& VAOdec, float tx, float ty, float tz, float sx, float sy, float sz)
{
	glm::mat4 model;
	model = transformation(tx, ty, tz, rotateAngle_X, rotateAngle_Y, rotateAngle_Z, sx, sy, sz);
	ourShader.setMat4("model", model);
	glBindVertexArray(VAOdec);
	glDrawElements(GL_TRIANGLES, 120, GL_UNSIGNED_INT, 0);
}

void glass(Shader& ourShader, unsigned int& VAO, float tx, float ty, float tz, float sx, float sy, float sz)
{
	glm::mat4 model;
	model = transformation(tx, ty, tz, rotateAngle_X, rotateAngle_Y, rotateAngle_Z, sx, sy, sz);
	ourShader.setMat4("model", model);
	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, 96, GL_UNSIGNED_INT, 0);
}

void bottle(Shader& ourShader, unsigned int& VAO, float tx, float ty, float tz, float sx, float sy, float sz)
{
	glm::mat4 model;
	model = transformation(tx, ty, tz, rotateAngle_X, rotateAngle_Y, rotateAngle_Z, sx, sy, sz);
	ourShader.setMat4("model", model);
	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, 138, GL_UNSIGNED_INT, 0);
}

void bulb(Shader& ourShader, unsigned int& VAO1, unsigned int& VAO2, unsigned int& VAO3, float tx, float ty, float tz)
{
	glm::mat4 model;
	model = transformation(tx + 2, ty + 1, tz + 0, rotateAngle_X, rotateAngle_Y, rotateAngle_Z, .08, .2, .08);
	ourShader.setMat4("model", model);//bulb
	glBindVertexArray(VAO1);
	glDrawElements(GL_TRIANGLES, 138, GL_UNSIGNED_INT, 0);
	model = transformation(tx + 2, ty + 1.2, tz + 0, rotateAngle_X, rotateAngle_Y, rotateAngle_Z, .01, .7, .01);
	ourShader.setMat4("model", model);//rod
	glBindVertexArray(VAO2);
	glDrawElements(GL_TRIANGLES, 120, GL_UNSIGNED_INT, 0);
	model = transformation(tx + 1.925, ty + 1.9, tz - 0.07, rotateAngle_X, rotateAngle_Y, rotateAngle_Z, .3, .1, .3);
	ourShader.setMat4("model", model);//top_holder
	glBindVertexArray(VAO3);
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
}

// FreeGLUT display callback — called every frame
void display()
{
	// timing
	float currentFrame = glutGet(GLUT_ELAPSED_TIME) / 1000.0f;
	deltaTime = currentFrame - lastFrame;
	lastFrame = currentFrame;

	// render
	glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// activate shader
	ourShader->use();
	glm::mat4 model;

	glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom),
		(float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
	ourShader->setMat4("projection", projection);

	glm::mat4 view = camera.GetViewMatrix();
	ourShader->setMat4("view", view);

	float shiftx = 5, shiftz = 0;
	for (int i = 0; i < 3; i++) {
		table_chair[i].tox = shiftx;
		table_chair[i].toz = shiftz;
		*ourShader = table_chair[i].ret_shader(*ourShader, VAO, VAO2, VAO3, VAO4, VAO5);
		shiftz -= 2.97;
	}

	//Wall1
	model = transformation(-2.5, -.75, -9, rotateAngle_X, rotateAngle_Y, rotateAngle_Z, 20, 7, 0.2);
	ourShader->setMat4("model", model);
	glBindVertexArray(VAOW1);
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

	//Wall2
	model = transformation(-2.5, -.75, -9, rotateAngle_X, rotateAngle_Y, rotateAngle_Z, .2, 7, 24);
	ourShader->setMat4("model", model);
	glBindVertexArray(VAOW2);
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

	model = transformation(7.5, -.75, -9, rotateAngle_X, rotateAngle_Y, rotateAngle_Z, .2, 7, 24);
	ourShader->setMat4("model", model);
	glBindVertexArray(VAOW2);
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

	//right whiteBoard
	model = transformation(5, .7, -8.9, rotateAngle_X, rotateAngle_Y, rotateAngle_Z, 4.5, 2.5, 0.2);
	ourShader->setMat4("model", model);
	glBindVertexArray(VAOB);
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
	model = transformation(4.95, .65, -8.95, rotateAngle_X, rotateAngle_Y, rotateAngle_Z, 4.7, 2.7, 0.2);
	ourShader->setMat4("model", model);
	glBindVertexArray(VAO4);
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

	//left whiteBoard
	model = transformation(-.3, .3, -8.9, rotateAngle_X, rotateAngle_Y, rotateAngle_Z, 3.3, 4, 0.2);
	ourShader->setMat4("model", model);
	glBindVertexArray(VAOB);
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
	model = transformation(-.35, 0.25, -8.95, rotateAngle_X, rotateAngle_Y, rotateAngle_Z, 3.5, 4.2, 0.2);
	ourShader->setMat4("model", model);
	glBindVertexArray(VAO4);
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

	//1st layer of biggg table
	model = transformation(-.3, -.75, -5, rotateAngle_X, rotateAngle_Y, rotateAngle_Z, 3, .2, 16);
	ourShader->setMat4("model", model);
	glBindVertexArray(VAOF2);
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

	model = transformation(-.3, -.659, -5, rotateAngle_X, rotateAngle_Y, rotateAngle_Z, 3, 2, 16);
	ourShader->setMat4("model", model);
	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

	model = transformation(-.3, .34, -5, rotateAngle_X, rotateAngle_Y, rotateAngle_Z, 3, .2, 16);
	ourShader->setMat4("model", model);
	glBindVertexArray(VAO5);
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

	//Cabinate
	model = transformation(-2.45, -.75, -8, rotateAngle_X, rotateAngle_Y, rotateAngle_Z, 1, 6, 4);
	ourShader->setMat4("model", model);
	glBindVertexArray(VAOC);
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
	model = transformation(-1.95, .6, -7.95, rotateAngle_X, rotateAngle_Y, rotateAngle_Z, .01, 3.0, 1.8);
	ourShader->setMat4("model", model);
	glBindVertexArray(VAO2);
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
	model = transformation(-1.95, .6, -6.95, rotateAngle_X, rotateAngle_Y, rotateAngle_Z, .01, 3.0, 1.8);
	ourShader->setMat4("model", model);
	glBindVertexArray(VAO2);
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
	model = transformation(-1.95, 0, -7.95, rotateAngle_X, rotateAngle_Y, rotateAngle_Z, .01, 1.0, 3.8);
	ourShader->setMat4("model", model);
	glBindVertexArray(VAO2);
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
	model = transformation(-1.95, -.6, -7.95, rotateAngle_X, rotateAngle_Y, rotateAngle_Z, .01, 1.0, 3.8);
	ourShader->setMat4("model", model);
	glBindVertexArray(VAO2);
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

	//Fan pivot
	model = transformation(2.125, 2.25, -5.875, rotateAngle_X, rotateAngle_Y, rotateAngle_Z, .5, 1, .5);
	ourShader->setMat4("model", model);
	glBindVertexArray(VAOF2);
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

	//chessboard
	const float floorWidth = 10.0f;
	const float floorLength = 12.0f;
	const float tileWidth = 2.0f;
	const float tileLength = 2.0f;
	const float startX = -2.5f;
	const float startY = -0.8f;
	const float startZ = -9.0f;
	const int numTilesX = static_cast<int>(2 * floorWidth / tileWidth);
	const int numTilesZ = static_cast<int>(2 * floorLength / tileLength);
	for (int i = 0; i < numTilesX; ++i) {
		for (int j = 0; j < numTilesZ; ++j) {
			float tx = startX + i * .5f * tileWidth;
			float tz = startZ + j * .5f * tileLength;
			if ((i + j) % 2 == 0) { glBindVertexArray(VAO4); }
			else { glBindVertexArray(VAOB); }
			model = transformation(tx, startY, tz, rotateAngle_X, rotateAngle_Y, rotateAngle_Z,
				tileWidth - 0.01f, 0.1f, tileLength - 0.01f);
			ourShader->setMat4("model", model);
			glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
		}
	}

	//vending_machine
	model = transformation(2.4, -.75, -8.9, rotateAngle_X, rotateAngle_Y, rotateAngle_Z, 3, 2, 1);
	ourShader->setMat4("model", model);
	glBindVertexArray(VAOF2);
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
	model = transformation(3.15, -0.05, -8.9, 90, 90, rotateAngle_Z, .251, .45, .249);
	ourShader->setMat4("model", model);
	glBindVertexArray(VAOdec);
	glDrawElements(GL_TRIANGLES, 120, GL_UNSIGNED_INT, 0);
	model = transformation(2.52, 0, -8.4, rotateAngle_X, rotateAngle_Y, rotateAngle_Z, 2.5, .1, .01);
	ourShader->setMat4("model", model);
	glBindVertexArray(VAO2);
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
	model = transformation(2.52, -.2, -8.4, rotateAngle_X, rotateAngle_Y, rotateAngle_Z, 2.5, .1, .01);
	ourShader->setMat4("model", model);
	glBindVertexArray(VAO2);
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
	model = transformation(2.52, -.4, -8.4, rotateAngle_X, rotateAngle_Y, rotateAngle_Z, 2.5, .1, .01);
	ourShader->setMat4("model", model);
	glBindVertexArray(VAO2);
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

	//bulb
	bulb(*ourShader, VAOdec6, VAOdec, VAO2, -1, 0.8, 2);
	bulb(*ourShader, VAOdec6, VAOdec, VAO2, -1, 0.8, 0);
	bulb(*ourShader, VAOdec6, VAOdec, VAO2, -1, 0.8, -2);

	//bottle
	bottle(*ourShader, VAOdec3, -2, 1.8, -.5, .035, .4, .035);
	bottle(*ourShader, VAOdec3, -2, 1.8, 1.5, .035, .4, .035);
	bottle(*ourShader, VAOdec3, -2, 1.8, -2.5, .035, .4, .035);
	bottle(*ourShader, VAOdec4, -2, 1.8, 1, .035, .4, .035);
	bottle(*ourShader, VAOdec4, -2, 1.8, -3, .035, .4, .035);
	bottle(*ourShader, VAOdec4, -2, 1.8, -2, .035, .4, .035);
	bottle(*ourShader, VAOdec5, -2, 1.8, 2.5, .035, .4, .035);
	bottle(*ourShader, VAOdec5, -2, 1.8, 0, .035, .4, .035);
	bottle(*ourShader, VAOdec3, -2, .8, 0, .035, .4, .035);
	bottle(*ourShader, VAOdec3, -2, .8, 1, .035, .4, .035);
	bottle(*ourShader, VAOdec3, -2, .8, -2, .035, .4, .035);
	bottle(*ourShader, VAOdec4, -2, .8, -3, .035, .4, .035);
	bottle(*ourShader, VAOdec4, -2, .8, 0, .035, .4, .035);
	bottle(*ourShader, VAOdec4, -2, .8, 2.3, .035, .4, .035);
	bottle(*ourShader, VAOdec5, -2, .8, -1, .035, .4, .035);

	//glass
	glass(*ourShader, VAOdec2, .6, .45, 2, .025, .3, .025);
	glass(*ourShader, VAOdec2, .6, .45, -1, .025, .3, .025);
	glass(*ourShader, VAOdec2, 5.7, .105, -2, .025, .3, .025);
	glass(*ourShader, VAOdec2, 5.9, .105, 1, .025, .3, .025);
	glass(*ourShader, VAOdec2, 6, .105, -5, .025, .3, .025);

	//stool
	cylinder(*ourShader, VAOdec, 2, -0.05, 2, .11, .2, .11);
	cylinder(*ourShader, VAOdec, 2, -.75, 2, .01, .7, .01);
	cylinder(*ourShader, VAOdec, 2, -.75, 2, .08, .01, .08);
	cylinder(*ourShader, VAOdec, 2, -0.05, 0.5, .11, .2, .11);
	cylinder(*ourShader, VAOdec, 2, -.75, 0.5, .01, .7, .01);
	cylinder(*ourShader, VAOdec, 2, -.75, 0.5, .08, .01, .08);
	cylinder(*ourShader, VAOdec, 2, -0.05, -1, .11, .2, .11);
	cylinder(*ourShader, VAOdec, 2, -.75, -1, .01, .7, .01);
	cylinder(*ourShader, VAOdec, 2, -.75, -1, .08, .01, .08);
	cylinder(*ourShader, VAOdec, 2, -0.05, -2.5, .11, .2, .11);
	cylinder(*ourShader, VAOdec, 2, -.75, -2.5, .01, .7, .01);
	cylinder(*ourShader, VAOdec, 2, -.75, -2.5, .08, .01, .08);
	cylinder(*ourShader, VAOdec, 2, -0.05, -4, .11, .2, .11);
	cylinder(*ourShader, VAOdec, 2, -.75, -4, .01, .7, .01);
	cylinder(*ourShader, VAOdec, 2, -.75, -4, .08, .01, .08);

	//rack
	model = transformation(-2.4, 0.75, -3.5, rotateAngle_X, rotateAngle_Y, rotateAngle_Z, 1.5, .2, 13);
	ourShader->setMat4("model", model);
	glBindVertexArray(VAO4);
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
	model = transformation(-2.4, 1.730, -3.5, rotateAngle_X, rotateAngle_Y, rotateAngle_Z, 1.5, .2, 13);
	ourShader->setMat4("model", model);
	glBindVertexArray(VAO4);
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
	model = transformation(-2.4, 2.65, -3.5, rotateAngle_X, rotateAngle_Y, rotateAngle_Z, 1.5, .2, 13);
	ourShader->setMat4("model", model);
	glBindVertexArray(VAO4);
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
	model = transformation(-2.4, .8, -3.5, rotateAngle_X, rotateAngle_Y, rotateAngle_Z, .01, 3.97, 13);
	ourShader->setMat4("model", model);
	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

	//Fan blades
	Fan fan;
	*ourShader = fan.local_rotation(*ourShader, VAOF3, fanAngle);
	if (fan_turn)
		fanAngle += 10;

	if (rotate_around)
		camera.ProcessKeyboard(Y_LEFT, deltaTime);

	glutSwapBuffers();
}

// FreeGLUT reshape callback

void reshape(int width, int height)
{
	glViewport(0, 0, width, height);
}

// FreeGLUT idle callback — triggers redisplay every frame

void idleFunc()
{
	glutPostRedisplay();
}


// FreeGLUT keyboard callback (regular ASCII keys)

void keyboardFunc(unsigned char key, int x, int y)
{
	switch (key)
	{
	case 27: // ESC
		glutLeaveMainLoop();
		break;
	case 'w': case 'W': camera.ProcessKeyboard(FORWARD, deltaTime);  break;
	case 's': case 'S': camera.ProcessKeyboard(BACKWARD, deltaTime); break;
	case 'a': case 'A': camera.ProcessKeyboard(LEFT, deltaTime);     break;
	case 'd': case 'D': camera.ProcessKeyboard(RIGHT, deltaTime);    break;
	case 'e': case 'E': camera.ProcessKeyboard(UP, deltaTime);       break;
	case 'r': case 'R': camera.ProcessKeyboard(DOWN, deltaTime);     break;
	case 'x': case 'X': camera.ProcessKeyboard(P_UP, deltaTime);    break;
	case 'c': case 'C': camera.ProcessKeyboard(P_DOWN, deltaTime);  break;
	case 'y': case 'Y': camera.ProcessKeyboard(Y_LEFT, deltaTime);  break;
	case 'v': case 'V': camera.ProcessKeyboard(Y_RIGHT, deltaTime); break;
	case 'z': case 'Z': camera.ProcessKeyboard(R_LEFT, deltaTime);  break;
	case 'q': case 'Q': camera.ProcessKeyboard(R_RIGHT, deltaTime); break;
	case 'g': case 'G': fan_turn = !fan_turn;       break;
	case 'f': case 'F': rotate_around = !rotate_around; break;
	}
	glutPostRedisplay();
}

// FreeGLUT mouse button callback

void mouseButtonFunc(int button, int state, int x, int y)
{
	if (button == GLUT_RIGHT_BUTTON)
	{
		if (state == GLUT_DOWN)
		{
			rightMousePressed = true;
			firstMouse = true; // reset to avoid jumping
		}
		else if (state == GLUT_UP)
		{
			rightMousePressed = false;
		}
	}
}


// FreeGLUT active mouse motion callback (look around)

void mouseMotionFunc(int x, int y)
{
	if (!rightMousePressed) return;

	if (firstMouse)
	{
		lastX = (float)x;
		lastY = (float)y;
		firstMouse = false;
	}
	float xoffset = (float)x - lastX;
	float yoffset = lastY - (float)y; // reversed: y goes bottom-to-top
	lastX = (float)x;
	lastY = (float)y;
	camera.ProcessMouseMovement(xoffset, yoffset);
	glutPostRedisplay();
}


// Helper: upload one VAO with vertex + index data

static void setupVAO(unsigned int& vao, unsigned int& vbo, unsigned int& ebo,
	const void* vertices, size_t vSize,
	const void* indices, size_t iSize)
{
	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &vbo);
	glGenBuffers(1, &ebo);
	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, vSize, vertices, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, iSize, indices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)12);
	glEnableVertexAttribArray(1);
}


// main

int main(int argc, char** argv)
{
	// FreeGLUT init
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(SCR_WIDTH, SCR_HEIGHT);
	glutInitWindowPosition(100, 100);
	glutCreateWindow("Nha hang 3D");

	// GLAD: load OpenGL function pointers (must be after context creation)
	if (!gladLoadGL())
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	glEnable(GL_DEPTH_TEST);

	// Register FreeGLUT callbacks
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutKeyboardFunc(keyboardFunc);
	glutMouseFunc(mouseButtonFunc);
	glutMotionFunc(mouseMotionFunc);
	glutIdleFunc(idleFunc);

	// Build shader
	ourShader = new Shader("vertexShader.vs", "fragmentShader.fs");


	//0.59f, 0.19f, 0.0f,

	float table_top[] = {
		0.0f, 0.0f, 0.0f, 0.78f, 0.66f, 0.44f,
		0.5f, 0.0f, 0.0f, 0.78f, 0.66f, 0.44f,
		0.5f, 0.5f, 0.0f, 0.78f, 0.66f, 0.44f,
		0.0f, 0.5f, 0.0f, 0.78f, 0.66f, 0.44f,

		0.5f, 0.0f, 0.0f, 0.78f, 0.66f, 0.44f,
		0.5f, 0.5f, 0.0f, 0.78f, 0.66f, 0.44f,
		0.5f, 0.0f, 0.5f, 0.78f, 0.66f, 0.44f,
		0.5f, 0.5f, 0.5f, 0.78f, 0.66f, 0.44f,

		0.0f, 0.0f, 0.5f, 0.78f, 0.66f, 0.44f,
		0.5f, 0.0f, 0.5f, 0.78f, 0.66f, 0.44f,
		0.5f, 0.5f, 0.5f, 0.78f, 0.66f, 0.44f,
		0.0f, 0.5f, 0.5f, 0.78f, 0.66f, 0.44f,

		0.0f, 0.0f, 0.5f, 0.78f, 0.66f, 0.44f,
		0.0f, 0.5f, 0.5f, 0.78f, 0.66f, 0.44f,
		0.0f, 0.5f, 0.0f, 0.78f, 0.66f, 0.44f,
		0.0f, 0.0f, 0.0f, 0.78f, 0.66f, 0.44f,

		0.5f, 0.5f, 0.5f, 0.78f, 0.66f, 0.44f,
		0.5f, 0.5f, 0.0f, 0.78f, 0.66f, 0.44f,
		0.0f, 0.5f, 0.0f, 0.78f, 0.66f, 0.44f,
		0.0f, 0.5f, 0.5f, 0.78f, 0.66f, 0.44f,

		0.0f, 0.0f, 0.0f, 0.78f, 0.66f, 0.44f,
		0.5f, 0.0f, 0.0f, 0.78f, 0.66f, 0.44f,
		0.5f, 0.0f, 0.5f, 0.78f, 0.66f, 0.44f,
		0.0f, 0.0f, 0.5f, 0.78f, 0.66f, 0.44f,
	};
	//0.80f, 0.59f, 0.0f,

	float table_leg[] = {
		0.0f, 0.0f, 0.0f,    0.984f, 0.926f, 0.852f,
		0.5f, 0.0f, 0.0f,     0.984f, 0.926f, 0.852f,
		0.5f, 0.5f, 0.0f,     0.984f, 0.926f, 0.852f,
		0.0f, 0.5f, 0.0f,    0.984f, 0.926f, 0.852f,

		0.5f, 0.0f, 0.0f,     0.984f, 0.926f, 0.852f,
		0.5f, 0.5f, 0.0f,     0.984f, 0.926f, 0.852f,
		0.5f, 0.0f, 0.5f,     0.984f, 0.926f, 0.852f,
		0.5f, 0.5f, 0.5f,     0.984f, 0.926f, 0.852f,

		0.0f, 0.0f, 0.5f,     0.984f, 0.926f, 0.852f,
		0.5f, 0.0f, 0.5f,    0.984f, 0.926f, 0.852f,
		0.5f, 0.5f, 0.5f,     0.984f, 0.926f, 0.852f,
		0.0f, 0.5f, 0.5f,     0.984f, 0.926f, 0.852f,

		0.0f, 0.0f, 0.5f,     0.984f, 0.926f, 0.852f,
		0.0f, 0.5f, 0.5f,     0.984f, 0.926f, 0.852f,
		0.0f, 0.5f, 0.0f,     0.984f, 0.926f, 0.852f,
		0.0f, 0.0f, 0.0f,     0.984f, 0.926f, 0.852f,

		0.5f, 0.5f, 0.5f,     0.984f, 0.926f, 0.852f,
		0.5f, 0.5f, 0.0f,    0.984f, 0.926f, 0.852f,
		0.0f, 0.5f, 0.0f,     0.984f, 0.926f, 0.852f,
		0.0f, 0.5f, 0.5f,     0.984f, 0.926f, 0.852f,

		0.0f, 0.0f, 0.0f,    0.984f, 0.926f, 0.852f,
		0.5f, 0.0f, 0.0f,    0.984f, 0.926f, 0.852f,
		0.5f, 0.0f, 0.5f,     0.984f, 0.926f, 0.852f,
		0.0f, 0.0f, 0.5f,     0.984f, 0.926f, 0.852f,
	};


	//chair seat
	float chair_leg[] = {
		0.0f, 0.0f, 0.0f, .543f, .19f, .215f,
		0.5f, 0.0f, 0.0f, .543f, .19f, .215f,
		0.5f, 0.5f, 0.0f, .543f, .19f, .215f,
		0.0f, 0.5f, 0.0f, .543f, .19f, .215f,

		0.5f, 0.0f, 0.0f, .543f, .19f, .215f,
		0.5f, 0.5f, 0.0f, .543f, .19f, .215f,
		0.5f, 0.0f, 0.5f, .543f, .19f, .215f,
		0.5f, 0.5f, 0.5f, .543f, .19f, .215f,

		0.0f, 0.0f, 0.5f, .543f, .19f, .215f,
		0.5f, 0.0f, 0.5f, .543f, .19f, .215f,
		0.5f, 0.5f, 0.5f, .543f, .19f, .215f,
		0.0f, 0.5f, 0.5f, .543f, .19f, .215f,

		0.0f, 0.0f, 0.5f, .543f, .19f, .215f,
		0.0f, 0.5f, 0.5f, .543f, .19f, .215f,
		0.0f, 0.5f, 0.0f, .543f, .19f, .215f,
		0.0f, 0.0f, 0.0f, .543f, .19f, .215f,

		0.5f, 0.5f, 0.5f, .543f, .19f, .215f,
		0.5f, 0.5f, 0.0f, .543f, .19f, .215f,
		0.0f, 0.5f, 0.0f, .543f, .19f, .215f,
		0.0f, 0.5f, 0.5f, .543f, .19f, .215f,

		0.0f, 0.0f, 0.0f, .543f, .19f, .215f,
		0.5f, 0.0f, 0.0f, .543f, .19f, .215f,
		0.5f, 0.0f, 0.5f, .543f, .19f, .215f,
		0.0f, 0.0f, 0.5f, .543f, .19f, .215f,
	};

	//background of board
	float chair_pillar[] = {
		0.0f, 0.0f, 0.0f, .0f, .0f, .0f,
		0.5f, 0.0f, 0.0f, .0f, .0f, .0f,
		0.5f, 0.5f, 0.0f, .0f, .0f, .0f,
		0.0f, 0.5f, 0.0f, .0f, .0f, .0f,

		0.5f, 0.0f, 0.0f, .0f, .0f, .0f,
		0.5f, 0.5f, 0.0f, .0f, .0f, .0f,
		0.5f, 0.0f, 0.5f, .0f, .0f, .0f,
		0.5f, 0.5f, 0.5f, .0f, .0f, .0f,

		0.0f, 0.0f, 0.5f, .0f, .0f, .0f,
		0.5f, 0.0f, 0.5f, .0f, .0f, .0f,
		0.5f, 0.5f, 0.5f, .0f, .0f, .0f,
		0.0f, 0.5f, 0.5f, .0f, .0f, .0f,

		0.0f, 0.0f, 0.5f, .0f, .0f, .0f,
		0.0f, 0.5f, 0.5f, .0f, .0f, .0f,
		0.0f, 0.5f, 0.0f, .0f, .0f, .0f,
		0.0f, 0.0f, 0.0f, .0f, .0f, .0f,

		0.5f, 0.5f, 0.5f, .0f, .0f, .0f,
		0.5f, 0.5f, 0.0f, .0f, .0f, .0f,
		0.0f, 0.5f, 0.0f, .0f, .0f, .0f,
		0.0f, 0.5f, 0.5f, .0f, .0f, .0f,

		0.0f, 0.0f, 0.0f, .0f, .0f, .0f,
		0.5f, 0.0f, 0.0f, .0f, .0f, .0f,
		0.5f, 0.0f, 0.5f, .0f, .0f, .0f,
		0.0f, 0.0f, 0.5f, .0f, .0f, .0f,
	};

	//0.9f, 0.9f, 0.0f,
	float chair_back[] = {
		0.0f, 0.0f, 0.0f, 0.684f, 0.238f, 0.27f,
		0.5f, 0.0f, 0.0f, 0.684f, 0.238f, 0.27f,
		0.5f, 0.5f, 0.0f, 0.684f, 0.238f, 0.27f,
		0.0f, 0.5f, 0.0f, 0.684f, 0.238f, 0.27f,

		0.5f, 0.0f, 0.0f, 0.684f, 0.238f, 0.27f,
		0.5f, 0.5f, 0.0f, 0.684f, 0.238f, 0.27f,
		0.5f, 0.0f, 0.5f, 0.684f, 0.238f, 0.27f,
		0.5f, 0.5f, 0.5f, 0.684f, 0.238f, 0.27f,

		0.0f, 0.0f, 0.5f, 0.684f, 0.238f, 0.27f,
		0.5f, 0.0f, 0.5f, 0.684f, 0.238f, 0.27f,
		0.5f, 0.5f, 0.5f, 0.684f, 0.238f, 0.27f,
		0.0f, 0.5f, 0.5f, 0.684f, 0.238f, 0.27f,

		0.0f, 0.0f, 0.5f, 0.684f, 0.238f, 0.27f,
		0.0f, 0.5f, 0.5f, 0.684f, 0.238f, 0.27f,
		0.0f, 0.5f, 0.0f, 0.684f, 0.238f, 0.27f,
		0.0f, 0.0f, 0.0f, 0.684f, 0.238f, 0.27f,

		0.5f, 0.5f, 0.5f, 0.684f, 0.238f, 0.27f,
		0.5f, 0.5f, 0.0f, 0.684f, 0.238f, 0.27f,
		0.0f, 0.5f, 0.0f, 0.684f, 0.238f, 0.27f,
		0.0f, 0.5f, 0.5f, 0.684f, 0.238f, 0.27f,

		0.0f, 0.0f, 0.0f, 0.684f, 0.238f, 0.27f,
		0.5f, 0.0f, 0.0f, 0.684f, 0.238f, 0.27f,
		0.5f, 0.0f, 0.5f, 0.684f, 0.238f, 0.27f,
		0.0f, 0.0f, 0.5f, 0.684f, 0.238f, 0.27f
	};
	//0.69f, 0.69f, 0.69f,
	float floor[] = {
		0.0f, 0.0f, 0.0f, 0.69f, 0.69f, 0.69f,
		0.5f, 0.0f, 0.0f, 0.69f, 0.69f, 0.69f,
		0.5f, 0.5f, 0.0f, 0.69f, 0.69f, 0.69f,
		0.0f, 0.5f, 0.0f, 0.69f, 0.69f, 0.69f,

		0.5f, 0.0f, 0.0f, 0.69f, 0.69f, 0.69f,
		0.5f, 0.5f, 0.0f, 0.69f, 0.69f, 0.69f,
		0.5f, 0.0f, 0.5f, 0.69f, 0.69f, 0.69f,
		0.5f, 0.5f, 0.5f, 0.69f, 0.69f, 0.69f,

		0.0f, 0.0f, 0.5f, 0.69f, 0.69f, 0.69f,
		0.5f, 0.0f, 0.5f, 0.69f, 0.69f, 0.69f,
		0.5f, 0.5f, 0.5f, 0.69f, 0.69f, 0.69f,
		0.0f, 0.5f, 0.5f, 0.69f, 0.69f, 0.69f,

		0.0f, 0.0f, 0.5f, 0.69f, 0.69f, 0.69f,
		0.0f, 0.5f, 0.5f, 0.69f, 0.69f, 0.69f,
		0.0f, 0.5f, 0.0f, 0.69f, 0.69f, 0.69f,
		0.0f, 0.0f, 0.0f, 0.69f, 0.69f, 0.69f,

		0.5f, 0.5f, 0.5f, 0.69f, 0.69f, 0.69f,
		0.5f, 0.5f, 0.0f, 0.69f, 0.69f, 0.69f,
		0.0f, 0.5f, 0.0f, 0.69f, 0.69f, 0.69f,
		0.0f, 0.5f, 0.5f, 0.69f, 0.69f, 0.69f,

		0.0f, 0.0f, 0.0f, 0.69f, 0.69f, 0.69f,
		0.5f, 0.0f, 0.0f, 0.69f, 0.69f, 0.69f,
		0.5f, 0.0f, 0.5f, 0.69f, 0.69f, 0.69f,
		0.0f, 0.0f, 0.5f, 0.69f, 0.69f, 0.69f
	};

	//front_back wall
	float wall1[] = {
		0.0f, 0.0f, 0.0f, 0.621f, 0.559f, 0.699f,
		0.5f, 0.0f, 0.0f, 0.621f, 0.559f, 0.699f,
		0.5f, 0.5f, 0.0f, 0.621f, 0.559f, 0.699f,
		0.0f, 0.5f, 0.0f, 0.621f, 0.559f, 0.699f,

		0.5f, 0.0f, 0.0f, 0.621f, 0.559f, 0.699f,
		0.5f, 0.5f, 0.0f, 0.621f, 0.559f, 0.699f,
		0.5f, 0.0f, 0.5f, 0.621f, 0.559f, 0.699f,
		0.5f, 0.5f, 0.5f, 0.621f, 0.559f, 0.699f,

		0.0f, 0.0f, 0.5f, 0.621f, 0.559f, 0.699f,
		0.5f, 0.0f, 0.5f, 0.621f, 0.559f, 0.699f,
		0.5f, 0.5f, 0.5f, 0.621f, 0.559f, 0.699f,
		0.0f, 0.5f, 0.5f, 0.621f, 0.559f, 0.699f,

		0.0f, 0.0f, 0.5f, 0.621f, 0.559f, 0.699f,
		0.0f, 0.5f, 0.5f, 0.621f, 0.559f, 0.699f,
		0.0f, 0.5f, 0.0f, 0.621f, 0.559f, 0.699f,
		0.0f, 0.0f, 0.0f, 0.621f, 0.559f, 0.699f,

		0.5f, 0.5f, 0.5f, 0.621f, 0.559f, 0.699f,
		0.5f, 0.5f, 0.0f, 0.621f, 0.559f, 0.699f,
		0.0f, 0.5f, 0.0f, 0.621f, 0.559f, 0.699f,
		0.0f, 0.5f, 0.5f, 0.621f, 0.559f, 0.699f,

		0.0f, 0.0f, 0.0f, 0.621f, 0.559f, 0.699f,
		0.5f, 0.0f, 0.0f, 0.621f, 0.559f, 0.699f,
		0.5f, 0.0f, 0.5f, 0.621f, 0.559f, 0.699f,
		0.0f, 0.0f, 0.5f, 0.621f, 0.559f, 0.699f
	};

	//left_right wall
	float wall2[] = {
		0.0f, 0.0f, 0.0f, 0.656f, 0.613f, 0.7304f,
		0.5f, 0.0f, 0.0f, 0.656f, 0.613f, 0.7304f,
		0.5f, 0.5f, 0.0f, 0.656f, 0.613f, 0.7304f,
		0.0f, 0.5f, 0.0f, 0.656f, 0.613f, 0.7304f,

		0.5f, 0.0f, 0.0f, 0.656f, 0.613f, 0.7304f,
		0.5f, 0.5f, 0.0f, 0.656f, 0.613f, 0.7304f,
		0.5f, 0.0f, 0.5f, 0.656f, 0.613f, 0.7304f,
		0.5f, 0.5f, 0.5f, 0.656f, 0.613f, 0.7304f,

		0.0f, 0.0f, 0.5f, 0.656f, 0.613f, 0.7304f,
		0.5f, 0.0f, 0.5f,0.656f, 0.613f, 0.7304f,
		0.5f, 0.5f, 0.5f, 0.656f, 0.613f, 0.7304f,
		0.0f, 0.5f, 0.5f, 0.656f, 0.613f, 0.7304f,

		0.0f, 0.0f, 0.5f, 0.656f, 0.613f, 0.7304f,
		0.0f, 0.5f, 0.5f, 0.656f, 0.613f, 0.7304f,
		0.0f, 0.5f, 0.0f, 0.656f, 0.613f, 0.7304f,
		0.0f, 0.0f, 0.0f, 0.656f, 0.613f, 0.7304f,

		0.5f, 0.5f, 0.5f, 0.656f, 0.613f, 0.7304f,
		0.5f, 0.5f, 0.0f, 0.656f, 0.613f, 0.7304f,
		0.0f, 0.5f, 0.0f, 0.656f, 0.613f, 0.7304f,
		0.0f, 0.5f, 0.5f, 0.656f, 0.613f, 0.7304f,

		0.0f, 0.0f, 0.0f, 0.656f, 0.613f, 0.7304f,
		0.5f, 0.0f, 0.0f, 0.656f, 0.613f, 0.7304f,
		0.5f, 0.0f, 0.5f, 0.656f, 0.613f, 0.7304f,
		0.0f, 0.0f, 0.5f, 0.656f, 0.613f, 0.7304f
	};



	//0f, 0f, 0f,
	float blackboard[] = {
		0.0f, 0.0f, 0.0f, 0.918f, 0.914f, 0.895f,
		0.5f, 0.0f, 0.0f, 0.918f, 0.914f, 0.895f,
		0.5f, 0.5f, 0.0f, 0.918f, 0.914f, 0.895f,
		0.0f, 0.5f, 0.0f, 0.918f, 0.914f, 0.895f,

		0.5f, 0.0f, 0.0f, 0.918f, 0.914f, 0.895f,
		0.5f, 0.5f, 0.0f, 0.918f, 0.914f, 0.895f,
		0.5f, 0.0f, 0.5f, 0.918f, 0.914f, 0.895f,
		0.5f, 0.5f, 0.5f, 0.918f, 0.914f, 0.895f,

		0.0f, 0.0f, 0.5f, 0.918f, 0.914f, 0.895f,
		0.5f, 0.0f, 0.5f, 0.918f, 0.914f, 0.895f,
		0.5f, 0.5f, 0.5f, 0.918f, 0.914f, 0.895f,
		0.0f, 0.5f, 0.5f, 0.918f, 0.914f, 0.895f,

		0.0f, 0.0f, 0.5f, 0.918f, 0.914f, 0.895f,
		0.0f, 0.5f, 0.5f, 0.918f, 0.914f, 0.895f,
		0.0f, 0.5f, 0.0f, 0.918f, 0.914f, 0.895f,
		0.0f, 0.0f, 0.0f, 0.918f, 0.914f, 0.895f,

		0.5f, 0.5f, 0.5f, 0.918f, 0.914f, 0.895f,
		0.5f, 0.5f, 0.0f, 0.918f, 0.914f, 0.895f,
		0.0f, 0.5f, 0.0f, 0.918f, 0.914f, 0.895f,
		0.0f, 0.5f, 0.5f, 0.918f, 0.914f, 0.895f,

		0.0f, 0.0f, 0.0f, 0.918f, 0.914f, 0.895f,
		0.5f, 0.0f, 0.0f, 0.918f, 0.914f, 0.895f,
		0.5f, 0.0f, 0.5f, 0.918f, 0.914f, 0.895f,
		0.0f, 0.0f, 0.5f, 0.918f, 0.914f, 0.895f
	};

	//0.29f, 0.0f, 0.29f,
	float cabinate[] = {
		0.0f, 0.0f, 0.0f,     0.93f, 0.305f, 0.203f,
		0.5f, 0.0f, 0.0f,     0.93f, 0.305f, 0.203f,
		0.5f, 0.5f, 0.0f,     0.93f, 0.305f, 0.203f,
		0.0f, 0.5f, 0.0f,     0.93f, 0.305f, 0.203f,

		0.5f, 0.0f, 0.0f,     0.93f, 0.305f, 0.203f,
		0.5f, 0.5f, 0.0f,     0.93f, 0.305f, 0.203f,
		0.5f, 0.0f, 0.5f,     0.93f, 0.305f, 0.203f,
		0.5f, 0.5f, 0.5f,    0.93f, 0.305f, 0.203f,

		0.0f, 0.0f, 0.5f,     0.93f, 0.305f, 0.203f,
		0.5f, 0.0f, 0.5f,     0.93f, 0.305f, 0.203f,
		0.5f, 0.5f, 0.5f,    0.93f, 0.305f, 0.203f,
		0.0f, 0.5f, 0.5f,     0.93f, 0.305f, 0.203f,

		0.0f, 0.0f, 0.5f,     0.93f, 0.305f, 0.203f,
		0.0f, 0.5f, 0.5f,    0.93f, 0.305f, 0.203f,
		0.0f, 0.5f, 0.0f,     0.93f, 0.305f, 0.203f,
		0.0f, 0.0f, 0.0f,     0.93f, 0.305f, 0.203f,

		0.5f, 0.5f, 0.5f,     0.93f, 0.305f, 0.203f,
		0.5f, 0.5f, 0.0f,     0.93f, 0.305f, 0.203f,
		0.0f, 0.5f, 0.0f,     0.93f, 0.305f, 0.203f,
		0.0f, 0.5f, 0.5f,     0.93f, 0.305f, 0.203f,

		0.0f, 0.0f, 0.0f,     0.93f, 0.305f, 0.203f,
		0.5f, 0.0f, 0.0f,    0.93f, 0.305f, 0.203f,
		0.5f, 0.0f, 0.5f,     0.93f, 0.305f, 0.203f,
		0.0f, 0.0f, 0.5f,     0.93f, 0.305f, 0.203f,
	};
	//0.95f, 0.95f, 0.95f,
	float ceiling[] = {
		0.0f, 0.0f, 0.0f, 0.95f, 0.95f, 0.95f,
		0.5f, 0.0f, 0.0f, 0.95f, 0.95f, 0.95f,
		0.5f, 0.5f, 0.0f, 0.95f, 0.95f, 0.95f,
		0.0f, 0.5f, 0.0f, 0.95f, 0.95f, 0.95f,

		0.5f, 0.0f, 0.0f, 0.95f, 0.95f, 0.95f,
		0.5f, 0.5f, 0.0f, 0.95f, 0.95f, 0.95f,
		0.5f, 0.0f, 0.5f, 0.95f, 0.95f, 0.95f,
		0.5f, 0.5f, 0.5f, 0.95f, 0.95f, 0.95f,

		0.0f, 0.0f, 0.5f, 0.95f, 0.95f, 0.95f,
		0.5f, 0.0f, 0.5f, 0.95f, 0.95f, 0.95f,
		0.5f, 0.5f, 0.5f, 0.95f, 0.95f, 0.95f,
		0.0f, 0.5f, 0.5f, 0.95f, 0.95f, 0.95f,

		0.0f, 0.0f, 0.5f, 0.95f, 0.95f, 0.95f,
		0.0f, 0.5f, 0.5f, 0.95f, 0.95f, 0.95f,
		0.0f, 0.5f, 0.0f, 0.95f, 0.95f, 0.95f,
		0.0f, 0.0f, 0.0f, 0.95f, 0.95f, 0.95f,

		0.5f, 0.5f, 0.5f, 0.95f, 0.95f, 0.95f,
		0.5f, 0.5f, 0.0f, 0.95f, 0.95f, 0.95f,
		0.0f, 0.5f, 0.0f, 0.95f, 0.95f, 0.95f,
		0.0f, 0.5f, 0.5f, 0.95f, 0.95f, 0.95f,

		0.0f, 0.0f, 0.0f, 0.95f, 0.95f, 0.95f,
		0.5f, 0.0f, 0.0f, 0.95f, 0.95f, 0.95f,
		0.5f, 0.0f, 0.5f, 0.95f, 0.95f, 0.95f,
		0.0f, 0.0f, 0.5f, 0.95f, 0.95f, 0.95f
	};

	//1.0f, 1.0f, 1.0f,
	float fan_holder[] = {
		0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,
		0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,
		0.5f, 0.5f, 0.0f, 1.0f, 1.0f, 1.0f,
		0.0f, 0.5f, 0.0f, 1.0f, 1.0f, 1.0f,

		0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,
		0.5f, 0.5f, 0.0f, 1.0f, 1.0f, 1.0f,
		0.5f, 0.0f, 0.5f, 1.0f, 1.0f, 1.0f,
		0.5f, 0.5f, 0.5f, 1.0f, 1.0f, 1.0f,

		0.0f, 0.0f, 0.5f, 1.0f, 1.0f, 1.0f,
		0.5f, 0.0f, 0.5f, 1.0f, 1.0f, 1.0f,
		0.5f, 0.5f, 0.5f, 1.0f, 1.0f, 1.0f,
		0.0f, 0.5f, 0.5f, 1.0f, 1.0f, 1.0f,

		0.0f, 0.0f, 0.5f, 1.0f, 1.0f, 1.0f,
		0.0f, 0.5f, 0.5f, 1.0f, 1.0f, 1.0f,
		0.0f, 0.5f, 0.0f, 1.0f, 1.0f, 1.0f,
		0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,

		0.5f, 0.5f, 0.5f, 1.0f, 1.0f, 1.0f,
		0.5f, 0.5f, 0.0f, 1.0f, 1.0f, 1.0f,
		0.0f, 0.5f, 0.0f, 1.0f, 1.0f, 1.0f,
		0.0f, 0.5f, 0.5f, 1.0f, 1.0f, 1.0f,

		0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,
		0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,
		0.5f, 0.0f, 0.5f, 1.0f, 1.0f, 1.0f,
		0.0f, 0.0f, 0.5f, 1.0f, 1.0f, 1.0f
	};
	//.44f, .22f, .05f,
	float fan_pivot[] = {
		0.0f, 0.0f, 0.0f,    .1569f, .2f, .2902f,
		0.5f, 0.0f, 0.0f,    .1569f, .2f, .2902f,
		0.5f, 0.5f, 0.0f,     .1569f, .2f, .2902f,
		0.0f, 0.5f, 0.0f,    .1569f, .2f, .2902f,

		0.5f, 0.0f, 0.0f,    .1569f, .2f, .2902f,
		0.5f, 0.5f, 0.0f,    .1569f, .2f, .2902f,
		0.5f, 0.0f, 0.5f,    .1569f, .2f, .2902f,
		0.5f, 0.5f, 0.5f,    .1569f, .2f, .2902f,

		0.0f, 0.0f, 0.5f,     .1569f, .2f, .2902f,
		0.5f, 0.0f, 0.5f,    .1569f, .2f, .2902f,
		0.5f, 0.5f, 0.5f,    .1569f, .2f, .2902f,
		0.0f, 0.5f, 0.5f,    .1569f, .2f, .2902f,

		0.0f, 0.0f, 0.5f,     .1569f, .2f, .2902f,
		0.0f, 0.5f, 0.5f,     .1569f, .2f, .2902f,
		0.0f, 0.5f, 0.0f,    .1569f, .2f, .2902f,
		0.0f, 0.0f, 0.0f,     .1569f, .2f, .2902f,

		0.5f, 0.5f, 0.5f,     .1569f, .2f, .2902f,
		0.5f, 0.5f, 0.0f,     .1569f, .2f, .2902f,
		0.0f, 0.5f, 0.0f,    .1569f, .2f, .2902f,
		0.0f, 0.5f, 0.5f,     .1569f, .2f, .2902f,

		0.0f, 0.0f, 0.0f,     .1569f, .2f, .2902f,
		0.5f, 0.0f, 0.0f,     .1569f, .2f, .2902f,
		0.5f, 0.0f, 0.5f,    .1569f, .2f, .2902f,
		0.0f, 0.0f, 0.5f,    .1569f, .2f, .2902f,
	};
	//.0f, .0f, .42f,
	float fan_blade[] = {
		0.0f, 0.0f, 0.0f,   .49f, .7058f, .424f,
		0.5f, 0.0f, 0.0f,    .49f, .7058f, .424f,
		0.5f, 0.5f, 0.0f,    .49f, .7058f, .424f,
		0.0f, 0.5f, 0.0f,    .49f, .7058f, .424f,

		0.5f, 0.0f, 0.0f,    .49f, .7058f, .424f,
		0.5f, 0.5f, 0.0f,    .49f, .7058f, .424f,
		0.5f, 0.0f, 0.5f,    .49f, .7058f, .424f,
		0.5f, 0.5f, 0.5f,    .49f, .7058f, .424f,

		0.0f, 0.0f, 0.5f,    .49f, .7058f, .424f,
		0.5f, 0.0f, 0.5f,   .49f, .7058f, .424f,
		0.5f, 0.5f, 0.5f,    .49f, .7058f, .424f,
		0.0f, 0.5f, 0.5f,    .49f, .7058f, .424f,

		0.0f, 0.0f, 0.5f,    .49f, .7058f, .424f,
		0.0f, 0.5f, 0.5f,    .49f, .7058f, .424f,
		0.0f, 0.5f, 0.0f,    .49f, .7058f, .424f,
		0.0f, 0.0f, 0.0f,    .49f, .7058f, .424f,

		0.5f, 0.5f, 0.5f,    .49f, .7058f, .424f,
		0.5f, 0.5f, 0.0f,    .49f, .7058f, .424f,
		0.0f, 0.5f, 0.0f,    .49f, .7058f, .424f,
		0.0f, 0.5f, 0.5f,    .49f, .7058f, .424f,

		0.0f, 0.0f, 0.0f,    .49f, .7058f, .424f,
		0.5f, 0.0f, 0.0f,    .49f, .7058f, .424f,
		0.5f, 0.0f, 0.5f,    .49f, .7058f, .424f,
		0.0f, 0.0f, 0.5f,    .49f, .7058f, .424f,
	};

	float border[] = {
		0.0f, 0.0f, 0.0f, .0f, .0f, .0f,
		0.5f, 0.0f, 0.0f, .0f, .0f, .0f,
		0.5f, 0.5f, 0.0f, .0f, .0f, .0f,
		0.0f, 0.5f, 0.0f, .0f, .0f, .0f,

		0.5f, 0.0f, 0.0f, .0f, .0f, .0f,
		0.5f, 0.5f, 0.0f, .0f, .0f, .0f,
		0.5f, 0.0f, 0.5f, .0f, .0f, .0f,
		0.5f, 0.5f, 0.5f, .0f, .0f, .0f,

		0.0f, 0.0f, 0.5f, .0f, .0f, .0f,
		0.5f, 0.0f, 0.5f, .0f, .0f, .0f,
		0.5f, 0.5f, 0.5f, .0f, .0f, .0f,
		0.0f, 0.5f, 0.5f, .0f, .0f, .0f,

		0.0f, 0.0f, 0.5f, .0f, .0f, .0f,
		0.0f, 0.5f, 0.5f, .0f, .0f, .0f,
		0.0f, 0.5f, 0.0f, .0f, .0f, .0f,
		0.0f, 0.0f, 0.0f, .0f, .0f, .0f,

		0.5f, 0.5f, 0.5f, .0f, .0f, .0f,
		0.5f, 0.5f, 0.0f, .0f, .0f, .0f,
		0.0f, 0.5f, 0.0f, .0f, .0f, .0f,
		0.0f, 0.5f, 0.5f, .0f, .0f, .0f,

		0.0f, 0.0f, 0.0f, .0f, .0f, .0f,
		0.5f, 0.0f, 0.0f, .0f, .0f, .0f,
		0.5f, 0.0f, 0.5f, .0f, .0f, .0f,
		0.0f, 0.0f, 0.5f, .0f, .0f, .0f
	};
	unsigned int cube_indices[] = {
		0, 3, 2,
		2, 1, 0,

		4, 5, 7,
		7, 6, 4,

		8, 9, 10,
		10, 11, 8,

		12, 13, 14,
		14, 15, 12,

		16, 17, 18,
		18, 19, 16,

		20, 21, 22,
		22, 23, 20
	};

	//for cylinder
	float decagon[] = {
	0.0f, 0.0f, 0.0f,  0.992f, 0.8588f, 0.1529f, // Center point
	-1.0f, 0.0f, -3.0f,  0.992f, 0.8588f, 0.1529f, // RED
	1.0f, 0.0f, -3.0f,    0.992f, 0.8588f, 0.1529f,// GREEN
	2.25f, 0.0f, -1.75f,     0.992f, 0.8588f, 0.1529f, // BLUE
	3.0f, 0.0f, 0.0f,    0.992f, 0.8588f, 0.1529f, // WHITE
	2.25f, 0.0f, 1.75f,    0.992f, 0.8588f, 0.1529f, // PINK
	1.0f, 0.0f, 3.0f,     0.992f, 0.8588f, 0.1529f, // KHOYERI
	-1.0f, 0.0f, 3.0f,     0.992f, 0.8588f, 0.1529f, // SKY BLUE
	-2.25f, 0.0f, 1.75f,     0.992f, 0.8588f, 0.1529f, // YELLOW
	-3.0f, 0.0f, 0.0f,    0.992f, 0.8588f, 0.1529f, // purple
	-2.25f, 0.0f, -1.75f,     0.992f, 0.8588f, 0.1529f, // orange

	0.0f, 1.0f, 0.0f,    0.0f, 0.694f, 0.824f, // Center point
	-1.0f, 1.0f, -3.0f,    0.0f, 0.694f, 0.824f, // RED
	1.0f, 1.0f, -3.0f,      0.0f, 0.694f, 0.824f, // GREEN
	2.25f, 1.0f, -1.75f,    0.0f, 0.694f, 0.824f,// BLUE
	3.0f, 1.0f, 0.0f,     0.0f, 0.694f, 0.824f, // WHITE
	2.25f, 1.0f, 1.75f,    0.0f, 0.694f, 0.824f, // PINK
	1.0f, 1.0f, 3.0f,     0.0f, 0.694f, 0.824f, // KHOYERI
	-1.0f, 1.0f, 3.0f,     0.0f, 0.694f, 0.824f, // SKY BLUE
	-2.25f, 1.0f, 1.75f,     0.0f, 0.694f, 0.824f,// YELLOW
	-3.0f, 1.0f, 0.0f,     0.0f, 0.694f, 0.824f, // purple
	-2.25f, 1.0f, -1.75f,     0.0f, 0.694f, 0.824f,// orange
	};
	unsigned int deca_indices[] = {
		0,1,2,
		0,2,3,
		0,3,4,
		0,4,5,
		0,5,6,
		0,6,7,
		0,7,8,
		0,8,9,
		0,9,10,
		0,10,1,

		11,12,13,
		11,13,14,
		11,14,15,
		11,15,16,
		11,16,17,
		11,17,18,
		11,18,19,
		11,19,20,
		11,20,21,
		11,21,12,


		2,12,13,
		2,12,1,

		3,13,14,
		3,13,2,

		4,14,15,
		4,14,3,

		5,15,16,
		5,15,4,

		6,16,17,
		6,16,5,

		7,17,18,
		7,17,6,

		8,18,19,
		8,18,7,

		9,19,20,
		9,19,8,

		10,20,21,
		10,20,9,

		1,21,12,
		1,21,10
	};


	//for glass
	float decagon2[] = {
		//0
	0.0f, 0.0f, 0.0f, 1.0f, 0.5f, 0.0f, // Center point
	-1.0f, 0.0f, -3.0f, 1.0f, 0.5f, 0.0f, // RED
	1.0f, 0.0f, -3.0f,  1.0f, 0.5f, 0.0f, // GREEN
	2.25f, 0.0f, -1.75f, 1.0f, 0.5f, 0.0f, // BLUE
	3.0f, 0.0f, 0.0f, 1.0f, 0.5f, 0.0f, // WHITE
	2.25f, 0.0f, 1.75f,1.0f, 0.5f, 0.0f, // PINK
	1.0f, 0.0f, 3.0f, 1.0f, 0.5f, 0.0f, // KHOYERI
	-1.0f, 0.0f, 3.0f, 1.0f, 0.5f, 0.0f, // SKY BLUE
	-2.25f, 0.0f, 1.75f, 1.0f, 0.5f, 0.0f, // YELLOW
	-3.0f, 0.0f, 0.0f, 1.0f, 0.5f, 0.0f, // purple
	-2.25f, 0.0f, -1.75f, 1.0f, 0.5f, 0.0f, // orange
	//11
	0.0f, 1.0f, 0.0f, 1.0f, 1.0f, 1.0f, // Center point
	-1.0f, 1.0f, -3.0f,  1.0f, 1.0f, 1.0f,  // RED
	1.0f, 1.0f, -3.0f,   1.0f, 1.0f, 1.0f,  // GREEN
	2.25f, 1.0f, -1.75f, 1.0f, 1.0f, 1.0f,  // BLUE
	3.0f, 1.0f, 0.0f,  1.0f, 1.0f, 1.0f,  // WHITE
	2.25f, 1.0f, 1.75f,  1.0f, 1.0f, 1.0f,  // PINK
	1.0f, 1.0f, 3.0f,  1.0f, 1.0f, 1.0f,  // KHOYERI
	-1.0f, 1.0f, 3.0f, 1.0f, 1.0f, 1.0f,  // SKY BLUE
	-2.25f, 1.0f, 1.75f, 1.0f, 1.0f, 1.0f,  // YELLOW
	-3.0f, 1.0f, 0.0f,  1.0f, 1.0f, 1.0f,  // purple
	-2.25f, 1.0f, -1.75f,  1.0f, 1.0f, 1.0f,  // orange
	//22
	-4.0f, 0.0f, -4.0f, 1.0f, 1.0f, 1.0f,
	4.0f, 0.0f, -4.0f,  1.0f, 1.0f, 1.0f,
	4.0f, 0.0f, 4.0f, 1.0f, 1.0f, 1.0f,
	-4.0f, 0.0f, 4.0f, 1.0f, 1.0f, 1.0f, // WHITE
	};
	unsigned int deca2_indices[] = {
		0,1,2,
		0,2,3,
		0,3,4,
		0,4,5,
		0,5,6,
		0,6,7,
		0,7,8,
		0,8,9,
		0,9,10,
		0,10,1,

		/*11,12,13,
		11,13,14,
		11,14,15,
		11,15,16,
		11,16,17,
		11,17,18,
		11,18,19,
		11,19,20,
		11,20,21,
		11,21,12,*/


		2,12,13,
		2,12,1,

		3,13,14,
		3,13,2,

		4,14,15,
		4,14,3,

		5,15,16,
		5,15,4,

		6,16,17,
		6,16,5,

		7,17,18,
		7,17,6,

		8,18,19,
		8,18,7,

		9,19,20,
		9,19,8,

		10,20,21,
		10,20,9,

		1,21,12,
		1,21,10,

		///
		24,22,23,
		24,22,25

	};

	//for bottle
	float decagon3[] = {
		//0
	0.0f, 0.0f, 0.0f, 1.0f, 0.5f, 0.0f, // Center point
	-1.0f, 0.0f, -3.0f, 1.0f, 0.5f, 0.0f, // RED
	1.0f, 0.0f, -3.0f,  1.0f, 0.5f, 0.0f, // GREEN
	2.25f, 0.0f, -1.75f, 1.0f, 0.5f, 0.0f, // BLUE
	3.0f, 0.0f, 0.0f, 1.0f, 0.5f, 0.0f, // WHITE
	2.25f, 0.0f, 1.75f,1.0f, 0.5f, 0.0f, // PINK
	1.0f, 0.0f, 3.0f, 1.0f, 0.5f, 0.0f, // KHOYERI
	-1.0f, 0.0f, 3.0f, 1.0f, 0.5f, 0.0f, // SKY BLUE
	-2.25f, 0.0f, 1.75f, 1.0f, 0.5f, 0.0f, // YELLOW
	-3.0f, 0.0f, 0.0f, 1.0f, 0.5f, 0.0f, // purple
	-2.25f, 0.0f, -1.75f, 1.0f, 0.5f, 0.0f, // orange
	//11
	0.0f, 1.0f, 0.0f, 1.0f, 1.0f, 1.0f, // Center point
	-1.0f, 1.0f, -3.0f,  1.0f, 1.0f, 1.0f,  // RED
	1.0f, 1.0f, -3.0f,   1.0f, 1.0f, 1.0f,  // GREEN
	2.25f, 1.0f, -1.75f, 1.0f, 1.0f, 1.0f,  // BLUE
	3.0f, 1.0f, 0.0f,  1.0f, 1.0f, 1.0f,  // WHITE
	2.25f, 1.0f, 1.75f,  1.0f, 1.0f, 1.0f,  // PINK
	1.0f, 1.0f, 3.0f,  1.0f, 1.0f, 1.0f,  // KHOYERI
	-1.0f, 1.0f, 3.0f, 1.0f, 1.0f, 1.0f,  // SKY BLUE
	-2.25f, 1.0f, 1.75f, 1.0f, 1.0f, 1.0f,  // YELLOW
	-3.0f, 1.0f, 0.0f,  1.0f, 1.0f, 1.0f,  // purple
	-2.25f, 1.0f, -1.75f,  1.0f, 1.0f, 1.0f,  // orange
	//22
	-1.0f, 1.9f, -1.0f, .0f, .0f, .0f,
	1.0f, 1.9f, -1.0f,  .0f, .0f, .0f,
	1.0f, 1.9f, 1.0f, .0f, .0f, .0f,
	-1.0f, 1.9f, 1.0f, .0f, .0f, .0f, // WHITE
	};
	unsigned int deca3_indices[] = {
		0,1,2,
		0,2,3,
		0,3,4,
		0,4,5,
		0,5,6,
		0,6,7,
		0,7,8,
		0,8,9,
		0,9,10,
		0,10,1,

		/*11,12,13,
		11,13,14,
		11,14,15,
		11,15,16,
		11,16,17,
		11,17,18,
		11,18,19,
		11,19,20,
		11,20,21,
		11,21,12,*/

		2,12,13,
		2,12,1,
		3,13,14,
		3,13,2,
		4,14,15,
		4,14,3,
		5,15,16,
		5,15,4,
		6,16,17,
		6,16,5,
		7,17,18,
		7,17,6,
		8,18,19,
		8,18,7,
		9,19,20,
		9,19,8,
		10,20,21,
		10,20,9,
		1,21,12,
		1,21,10,

		24,22,23,
		24,22,25,

		23,12,13,
		23,12,22,
		23,13,14,
		22,21,12,
		22,20,21,
		23,14,15,
		22,25,20,
		24,23,15,
		24,15,16,
		25,19,20,
		24,16,17,
		25,18,19,
		25,17,18,
		25,17,24,


	};

	//for bottle2
	float decagon4[] = {
		//0
		0.0f, 0.0f, 0.0f, .0f, 1.0f, 1.0f, // Center point
		-1.0f, 0.0f, -3.0f,.0f, 1.0f, 1.0f, // RED
		1.0f, 0.0f, -3.0f,  .0f, 1.0f, 1.0f, // GREEN
		2.25f, 0.0f, -1.75f, .0f, 1.0f, 1.0f, // BLUE
		3.0f, 0.0f, 0.0f, .0f, 1.0f, 1.0f, // WHITE
		2.25f, 0.0f, 1.75f, .0f, 1.0f, 1.0f, // PINK
		1.0f, 0.0f, 3.0f, .0f, 1.0f, 1.0f, // KHOYERI
		-1.0f, 0.0f, 3.0f, .0f, 1.0f, 1.0f, // SKY BLUE
		-2.25f, 0.0f, 1.75f, .0f, 1.0f, 1.0f, // YELLOW
		-3.0f, 0.0f, 0.0f, .0f, 1.0f, 1.0f, // purple
		-2.25f, 0.0f, -1.75f, .0f, 1.0f, 1.0f, // orange
		//11
		0.0f, 1.0f, 0.0f, 1.0f, 1.0f, 1.0f, // Center point
		-1.0f, 1.0f, -3.0f,  1.0f, 1.0f, 1.0f,  // RED
		1.0f, 1.0f, -3.0f,   1.0f, 1.0f, 1.0f,  // GREEN
		2.25f, 1.0f, -1.75f, 1.0f, 1.0f, 1.0f,  // BLUE
		3.0f, 1.0f, 0.0f,  1.0f, 1.0f, 1.0f,  // WHITE
		2.25f, 1.0f, 1.75f,  1.0f, 1.0f, 1.0f,  // PINK
		1.0f, 1.0f, 3.0f,  1.0f, 1.0f, 1.0f,  // KHOYERI
		-1.0f, 1.0f, 3.0f, 1.0f, 1.0f, 1.0f,  // SKY BLUE
		-2.25f, 1.0f, 1.75f, 1.0f, 1.0f, 1.0f,  // YELLOW
		-3.0f, 1.0f, 0.0f,  1.0f, 1.0f, 1.0f,  // purple
		-2.25f, 1.0f, -1.75f,  1.0f, 1.0f, 1.0f,  // orange
		//22
		-1.0f, 1.9f, -1.0f, 1.0f, .5f, .0f,
		1.0f, 1.9f, -1.0f,  1.0f, .5f, .0f,
		1.0f, 1.9f, 1.0f, 1.0f, .5f, .0f,
		-1.0f, 1.9f, 1.0f, 1.0f, .5f, .0f, // WHITE
	};
	unsigned int deca4_indices[] = {
		0,1,2,
		0,2,3,
		0,3,4,
		0,4,5,
		0,5,6,
		0,6,7,
		0,7,8,
		0,8,9,
		0,9,10,
		0,10,1,

		/*11,12,13,
		11,13,14,
		11,14,15,
		11,15,16,
		11,16,17,
		11,17,18,
		11,18,19,
		11,19,20,
		11,20,21,
		11,21,12,*/

		2,12,13,
		2,12,1,
		3,13,14,
		3,13,2,
		4,14,15,
		4,14,3,
		5,15,16,
		5,15,4,
		6,16,17,
		6,16,5,
		7,17,18,
		7,17,6,
		8,18,19,
		8,18,7,
		9,19,20,
		9,19,8,
		10,20,21,
		10,20,9,
		1,21,12,
		1,21,10,

		24,22,23,
		24,22,25,

		23,12,13,
		23,12,22,
		23,13,14,
		22,21,12,
		22,20,21,
		23,14,15,
		22,25,20,
		24,23,15,
		24,15,16,
		25,19,20,
		24,16,17,
		25,18,19,
		25,17,18,
		25,17,24,


	};

	//for bottle3
	float decagon5[] = {
		//0
		0.0f, 0.0f, 0.0f,    1.0f, .5f, .3125f, // Center point
		-1.0f, 0.0f, -3.0f,  1.0f, .5f, .3125f, // RED
		1.0f, 0.0f, -3.0f,   1.0f, .5f, .3125f,  // GREEN
		2.25f, 0.0f, -1.75f,   1.0f, .5f, .3125f, // BLUE
		3.0f, 0.0f, 0.0f,        1.0f, .5f, .3125f, // WHITE
		2.25f, 0.0f, 1.75f,   1.0f, .5f, .3125f, // PINK
		1.0f, 0.0f, 3.0f,    1.0f, .5f, .3125f,// KHOYERI
		-1.0f, 0.0f, 3.0f,    1.0f, .5f, .3125f,// SKY BLUE
		-2.25f, 0.0f, 1.75f,    1.0f, .5f, .3125f,// YELLOW
		-3.0f, 0.0f, 0.0f,    1.0f, .5f, .3125f,// purple
		-2.25f, 0.0f, -1.75f,    1.0f, .5f, .3125f,// orange
		//11
		0.0f, 1.0f, 0.0f,     .469f, .222f, .215f, // Center point
		-1.0f, 1.0f, -3.0f,      .469f, .222f, .215f,  // RED
		1.0f, 1.0f, -3.0f,       .469f, .222f, .215f,  // GREEN
		2.25f, 1.0f, -1.75f,     .469f, .222f, .215f,  // BLUE
		3.0f, 1.0f, 0.0f,     .469f, .222f, .215f, // WHITE
		2.25f, 1.0f, 1.75f,     .469f, .222f, .215f,  // PINK
		1.0f, 1.0f, 3.0f,     .469f, .222f, .215f,  // KHOYERI
		-1.0f, 1.0f, 3.0f,     .469f, .222f, .215f,  // SKY BLUE
		-2.25f, 1.0f, 1.75f,    .469f, .222f, .215f, // YELLOW
		-3.0f, 1.0f, 0.0f,      .469f, .222f, .215f,// purple
		-2.25f, 1.0f, -1.75f,     .469f, .222f, .215f,  // orange
		//22
		-1.0f, 1.9f, -1.0f,    1.0f, .5f, .0f,
		1.0f, 1.9f, -1.0f,  1.0f, .5f, .0f,
		1.0f, 1.9f, 1.0f, 1.0f, .5f, .0f,
		-1.0f, 1.9f, 1.0f, 1.0f, .5f, .0f, // WHITE
	};
	unsigned int deca5_indices[] = {
		0,1,2,
		0,2,3,
		0,3,4,
		0,4,5,
		0,5,6,
		0,6,7,
		0,7,8,
		0,8,9,
		0,9,10,
		0,10,1,

		/*11,12,13,
		11,13,14,
		11,14,15,
		11,15,16,
		11,16,17,
		11,17,18,
		11,18,19,
		11,19,20,
		11,20,21,
		11,21,12,*/

		2,12,13,
		2,12,1,
		3,13,14,
		3,13,2,
		4,14,15,
		4,14,3,
		5,15,16,
		5,15,4,
		6,16,17,
		6,16,5,
		7,17,18,
		7,17,6,
		8,18,19,
		8,18,7,
		9,19,20,
		9,19,8,
		10,20,21,
		10,20,9,
		1,21,12,
		1,21,10,

		24,22,23,
		24,22,25,

		23,12,13,
		23,12,22,
		23,13,14,
		22,21,12,
		22,20,21,
		23,14,15,
		22,25,20,
		24,23,15,
		24,15,16,
		25,19,20,
		24,16,17,
		25,18,19,
		25,17,18,
		25,17,24,


	};

	//for bulb
	float decagon6[] = {
		//0
		0.0f, 0.0f, 0.0f,    1.0f, 1.0f, 1.0f, // Center point
		-1.0f, 0.0f, -3.0f,  1.0f, 1.0f, 1.0f,  // RED
		1.0f, 0.0f, -3.0f,  1.0f, 1.0f, 1.0f,   // GREEN
		2.25f, 0.0f, -1.75f,  1.0f, 1.0f, 1.0f,  // BLUE
		3.0f, 0.0f, 0.0f,       1.0f, 1.0f, 1.0f,  // WHITE
		2.25f, 0.0f, 1.75f,   1.0f, 1.0f, 1.0f, // PINK
		1.0f, 0.0f, 3.0f,   1.0f, 1.0f, 1.0f, // KHOYERI
		-1.0f, 0.0f, 3.0f,    1.0f, 1.0f, 1.0f, // SKY BLUE
		-2.25f, 0.0f, 1.75f,    1.0f, 1.0f, 1.0f, // YELLOW
		-3.0f, 0.0f, 0.0f,    1.0f, 1.0f, 1.0f, // purple
		-2.25f, 0.0f, -1.75f,    1.0f, 1.0f, 1.0f, // orange
		//11
		0.0f, .1f, 0.0f,     1.0f, 1.0f, 1.0f, // Center point
		-1.0f, .1f, -3.0f,      1.0f, 1.0f, 1.0f,   // RED
		1.0f, .1f, -3.0f,       1.0f, 1.0f, 1.0f,   // GREEN
		2.25f, .1f, -1.75f,     1.0f, 1.0f, 1.0f,  // BLUE
		3.0f, .1f, 0.0f,    1.0f, 1.0f, 1.0f,  // WHITE
		2.25f, .1f, 1.75f,     1.0f, 1.0f, 1.0f,   // PINK
		1.0f, .1f, 3.0f,     1.0f, 1.0f, 1.0f,   // KHOYERI
		-1.0f, .1f, 3.0f,    1.0f, 1.0f, 1.0f,  // SKY BLUE
		-2.25f, .1f, 1.75f,    1.0f, 1.0f, 1.0f, // YELLOW
		-3.0f, .1f, 0.0f,      1.0f, 1.0f, 1.0f, // purple
		-2.25f, .1f, -1.75f,    1.0f, 1.0f, 1.0f,// orange
		//22
		-1.0f, 1.9f, -1.0f,    .109f, .109f, .1054f,
		1.0f, 1.9f, -1.0f, .109f, .109f, .1054f,
		1.0f, 1.9f, 1.0f,  .109f, .109f, .1054f,
		-1.0f, 1.9f, 1.0f, .109f, .109f, .1054f,// WHITE
	};
	unsigned int deca6_indices[] = {
		0,1,2,
		0,2,3,
		0,3,4,
		0,4,5,
		0,5,6,
		0,6,7,
		0,7,8,
		0,8,9,
		0,9,10,
		0,10,1,

		/*11,12,13,
		11,13,14,
		11,14,15,
		11,15,16,
		11,16,17,
		11,17,18,
		11,18,19,
		11,19,20,
		11,20,21,
		11,21,12,*/

		2,12,13,
		2,12,1,
		3,13,14,
		3,13,2,
		4,14,15,
		4,14,3,
		5,15,16,
		5,15,4,
		6,16,17,
		6,16,5,
		7,17,18,
		7,17,6,
		8,18,19,
		8,18,7,
		9,19,20,
		9,19,8,
		10,20,21,
		10,20,9,
		1,21,12,
		1,21,10,

		24,22,23,
		24,22,25,

		23,12,13,
		23,12,22,
		23,13,14,
		22,21,12,
		22,20,21,
		23,14,15,
		22,25,20,
		24,23,15,
		24,15,16,
		25,19,20,
		24,16,17,
		25,18,19,
		25,17,18,
		25,17,24,


	};



	// Setup VAOs using global variables + vertex data defined above
	unsigned int VBO, EBO;
	setupVAO(VAO, VBO, EBO, table_top, sizeof(table_top), cube_indices, sizeof(cube_indices));

	unsigned int VBO2, EBO2;
	setupVAO(VAO2, VBO2, EBO2, table_leg, sizeof(table_leg), cube_indices, sizeof(cube_indices));

	unsigned int VBO3, EBO3;
	setupVAO(VAO3, VBO3, EBO3, chair_leg, sizeof(chair_leg), cube_indices, sizeof(cube_indices));

	unsigned int VBO4, EBO4;
	setupVAO(VAO4, VBO4, EBO4, chair_pillar, sizeof(chair_pillar), cube_indices, sizeof(cube_indices));

	unsigned int VBO5, EBO5;
	setupVAO(VAO5, VBO5, EBO5, chair_back, sizeof(chair_back), cube_indices, sizeof(cube_indices));

	unsigned int VBOG, EBOG;
	setupVAO(VAOG, VBOG, EBOG, floor, sizeof(floor), cube_indices, sizeof(cube_indices));

	unsigned int VBOW1, EBOW1;
	setupVAO(VAOW1, VBOW1, EBOW1, wall1, sizeof(wall1), cube_indices, sizeof(cube_indices));

	unsigned int VBOW2, EBOW2;
	setupVAO(VAOW2, VBOW2, EBOW2, wall2, sizeof(wall2), cube_indices, sizeof(cube_indices));

	unsigned int VBOB, EBOB;
	setupVAO(VAOB, VBOB, EBOB, blackboard, sizeof(blackboard), cube_indices, sizeof(cube_indices));

	unsigned int VBOC, EBOC;
	setupVAO(VAOC, VBOC, EBOC, cabinate, sizeof(cabinate), cube_indices, sizeof(cube_indices));

	unsigned int VBOT, EBOT;
	setupVAO(VAOT, VBOT, EBOT, ceiling, sizeof(ceiling), cube_indices, sizeof(cube_indices));

	// Fan
	unsigned int VBOF1, EBOF1;
	setupVAO(VAOF1, VBOF1, EBOF1, fan_holder, sizeof(fan_holder), cube_indices, sizeof(cube_indices));

	unsigned int VBOF2, EBOF2;
	setupVAO(VAOF2, VBOF2, EBOF2, fan_pivot, sizeof(fan_pivot), cube_indices, sizeof(cube_indices));

	unsigned int VBOF3, EBOF3;
	setupVAO(VAOF3, VBOF3, EBOF3, fan_blade, sizeof(fan_blade), cube_indices, sizeof(cube_indices));

	// Cylinders / decagons
	unsigned int VBOdec, EBOdec;
	setupVAO(VAOdec, VBOdec, EBOdec, decagon, sizeof(decagon), deca_indices, sizeof(deca_indices));

	unsigned int VBOdec2, EBOdec2;
	setupVAO(VAOdec2, VBOdec2, EBOdec2, decagon2, sizeof(decagon2), deca2_indices, sizeof(deca2_indices));

	unsigned int VBOdec3, EBOdec3;
	setupVAO(VAOdec3, VBOdec3, EBOdec3, decagon3, sizeof(decagon3), deca3_indices, sizeof(deca3_indices));

	unsigned int VBOdec4, EBOdec4;
	setupVAO(VAOdec4, VBOdec4, EBOdec4, decagon4, sizeof(decagon4), deca4_indices, sizeof(deca4_indices));

	unsigned int VBOdec5, EBOdec5;
	setupVAO(VAOdec5, VBOdec5, EBOdec5, decagon5, sizeof(decagon5), deca5_indices, sizeof(deca5_indices));

	unsigned int VBOdec6, EBOdec6;
	setupVAO(VAOdec6, VBOdec6, EBOdec6, decagon6, sizeof(decagon6), deca6_indices, sizeof(deca6_indices));

	// Enter FreeGLUT main loop (never returns)
	glutMainLoop();

	delete ourShader;
	return 0;
}
