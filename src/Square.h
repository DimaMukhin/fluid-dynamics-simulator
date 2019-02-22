#pragma once

#include "common.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>

class Square
{
public:
	static const GLuint numOfVertices = 6;
	static glm::vec4 *vertices;

	Square(GLuint modelUniformLocation, GLuint vertexCoordAttribLocation, GLuint colorUniformLocation);

	void display(GLfloat x, GLfloat y, glm::vec4 color);

	~Square();

private:
	GLuint VAO, VBO;
	GLuint modelUniformLocation;
	GLuint colorUniformLocation;
	GLuint vertexCoordAttribLocation;

	void init();
};