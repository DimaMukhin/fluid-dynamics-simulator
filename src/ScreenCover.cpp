#include "ScreenCover.h"

glm::vec4* ScreenCover::vertices = new glm::vec4[numOfVertices]{
	glm::vec4(0.0f, (GLfloat)N * scale, 0.0f, 1.0f),
	glm::vec4((GLfloat)N * scale, (GLfloat)N * scale, 0.0f, 1.0f),
	glm::vec4((GLfloat)N * scale, 0.0f, 0.0f, 1.0f),

	glm::vec4((GLfloat)N * scale, 0.0f, 0.0f, 1.0f),
	glm::vec4(0.0f, 0.0f, 0.0f, 1.0f),
	glm::vec4(0.0f, (GLfloat)N * scale, 0.0f, 1.0f),
};

ScreenCover::ScreenCover(GLuint vertexCoordAttribLocation)
{
	this->vertexCoordAttribLocation = vertexCoordAttribLocation;

	init();
}

void ScreenCover::display()
{
	glBindVertexArray(VAO);

	glDrawArrays(GL_TRIANGLES, 0, numOfVertices);

	glBindVertexArray(0);
}

void ScreenCover::init()
{
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices[0]) * numOfVertices, vertices, GL_STATIC_DRAW);

	glEnableVertexAttribArray(vertexCoordAttribLocation);
	glVertexAttribPointer(vertexCoordAttribLocation, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

	// unbind objects
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}
