#include "FluidDisplay.h"

FluidDisplay::FluidDisplay(GLuint vertexCoordAttribLocation, GLuint vertexColorAttribLocation)
{
	this->vertexCoordAttribLocation = vertexCoordAttribLocation;
	this->vertexColorAttribLocation = vertexColorAttribLocation;

	particles = new glm::vec4[numOfVertices]();
	int count = 0;

	// TODO: take a note that we go columns rows and not rows columns
	for (int i = 0; i < N; i++) {
		for (int j = 0; j < N; j++) {
			particles[count] = glm::vec4((GLfloat) (j * scale), (GLfloat)(i * scale), 0.0f, 1.0f);
			count++;
		}
	}

	init();

	glPointSize((GLfloat) scale);
}

// TODO: I think that I know why I have a white border in the bottom and left. its because of how glPointSize works... its size around the point or something
void FluidDisplay::display()
{
	glBindVertexArray(VAO);

	glDrawArrays(GL_POINTS, 0, numOfVertices);

	glBindVertexArray(0);
}

void FluidDisplay::init()
{
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec4) * numOfVertices * 2, NULL, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(glm::vec4) * numOfVertices, particles);

	glEnableVertexAttribArray(vertexCoordAttribLocation);
	glVertexAttribPointer(vertexCoordAttribLocation, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

	glEnableVertexAttribArray(vertexColorAttribLocation);
	glVertexAttribPointer(vertexColorAttribLocation, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(sizeof(glm::vec4) * numOfVertices));

	// unbind objects
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void FluidDisplay::update(glm::vec4 * colors)
{
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(glm::vec4) * numOfVertices, sizeof(glm::vec4) * numOfVertices, colors);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	delete colors;
}
