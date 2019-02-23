#pragma once

#include "common.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class ScreenCover
{
public:
	static const GLuint numOfVertices = 6;
	static glm::vec4 *vertices;

	ScreenCover(GLuint vertexCoordAttribLocation);

	void display();

private:
	GLuint VAO, VBO;
	GLuint vertexCoordAttribLocation;

	void init();
};