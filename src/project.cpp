// Display a cube, using glDrawElements

#include "common.h"
#include "Fluid.h"
#include "Square.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

const char *WINDOW_TITLE = "Project";
const double FRAME_RATE_MS = 1000.0 / 60.0;

GLuint modelUniformLocation, viewUniformLocation, projectionUniformLocation;
GLuint colorUniformLocation;

Square *square;
Fluid *fluid;

//----------------------------------------------------------------------------

// OpenGL initialization
void init()
{
	// Load shaders and use the resulting shader program
	GLuint program = InitShader("vshader.glsl", "fshader.glsl");
	glUseProgram(program);

	// set up vertex arrays
	GLuint vPosition = glGetAttribLocation(program, "vPosition");

	modelUniformLocation = glGetUniformLocation(program, "model");
	viewUniformLocation = glGetUniformLocation(program, "view");
	projectionUniformLocation = glGetUniformLocation(program, "projection");

	// default values
	glUniformMatrix4fv(modelUniformLocation, 1, GL_FALSE, glm::value_ptr(glm::mat4()));
	glUniformMatrix4fv(viewUniformLocation, 1, GL_FALSE, glm::value_ptr(glm::mat4()));
	glUniform4fv(colorUniformLocation, 1, glm::value_ptr(glm::vec4()));

	square = new Square(modelUniformLocation, vPosition, colorUniformLocation);
	// TODO: NOTE! dummy version has a bug. diffusion doesnt work in dummy version. in this version diffussion works without velocity
	fluid = new Fluid(0.1f, 0.0000001f, square);

	glEnable(GL_DEPTH_TEST);
	glClearColor(1.0, 1.0, 1.0, 1.0);
}

//----------------------------------------------------------------------------

void display(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	fluid->addDensity(100, 100, 10.0f);
	fluid->addVelocity(100, 100, 1.0f, 1.0f);
	fluid->vel_step();
	fluid->dens_step();
	fluid->displayD();

	glutSwapBuffers();
	glFinish();
}

//----------------------------------------------------------------------------

void keyboard(unsigned char key, int x, int y)
{
	switch (key) {
	case 033: // Escape Key
	case 'q': case 'Q':
		exit(EXIT_SUCCESS);
		break;
	}
}

//----------------------------------------------------------------------------

void mouse(int button, int state, int x, int y)
{
	if (state == GLUT_DOWN) {
	}
}

//----------------------------------------------------------------------------

void update(void)
{
}

//----------------------------------------------------------------------------

void reshape(int width, int height)
{
	glViewport(0, 0, width, height);

	glm::mat4  projection = glm::ortho(0.0f, (GLfloat)N * scale, (GLfloat)N * scale, 0.0f);

	glUniformMatrix4fv(projectionUniformLocation, 1, GL_FALSE, glm::value_ptr(projection));
}
