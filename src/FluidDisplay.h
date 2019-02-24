#pragma once

#include "common.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class FluidDisplay
{
public:
	static const GLuint numOfVertices = N * N;

	FluidDisplay(GLuint vertexCoordAttribLocation, GLuint vertexColorAttribLocation);

	void init();

	void update(glm::vec4 *colors);

	void display();

private:
	glm::vec4 *particles;
	GLuint VAO, VBO;
	GLuint vertexCoordAttribLocation;
	GLuint vertexColorAttribLocation;
};