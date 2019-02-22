#include "Square.h"

glm::vec4* Square::vertices = new glm::vec4[numOfVertices] {
	glm::vec4(0.0f, 0.0f, 0.0f, 1.0f),
};

Square::Square(GLuint modelUniformLocation, GLuint vertexCoordAttribLocation, GLuint colorUniformLocation)
{
	this->modelUniformLocation = modelUniformLocation;
	this->vertexCoordAttribLocation = vertexCoordAttribLocation;
	this->colorUniformLocation = colorUniformLocation;

	init();
}

void Square::display(GLfloat x, GLfloat y, glm::vec4 color)
{
	glBindVertexArray(VAO);

	glm::mat4 model;
	model = glm::translate(model, glm::vec3(x, y, 0.0f));

	glUniformMatrix4fv(modelUniformLocation, 1, GL_FALSE, glm::value_ptr(model));
	glUniform4fv(colorUniformLocation, 1, glm::value_ptr(color));

	glDrawArrays(GL_POINTS, 0, numOfVertices);

	glUniformMatrix4fv(modelUniformLocation, 1, GL_FALSE, glm::value_ptr(glm::mat4()));
	glUniform4fv(colorUniformLocation, 1, glm::value_ptr(glm::vec4()));

	glBindVertexArray(0);
}

Square::~Square()
{
}

void Square::init()
{
	glPointSize((GLfloat) 1); // TODO: change with scale

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
