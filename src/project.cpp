// Display a cube, using glDrawElements

#include "common.h"
#include "Fluid.h"
#include "FluidDisplay.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

const char *WINDOW_TITLE = "Project";
const double FRAME_RATE_MS = 1000.0 / 60.0;

GLuint modelUniformLocation, viewUniformLocation, projectionUniformLocation;
GLuint colorUniformLocation;

glm::vec4 *fakeGrid;
FluidDisplay *fd;
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
	GLuint vColor = glGetAttribLocation(program, "vColor");

	modelUniformLocation = glGetUniformLocation(program, "model");
	viewUniformLocation = glGetUniformLocation(program, "view");
	projectionUniformLocation = glGetUniformLocation(program, "projection");

	// default values
	glUniformMatrix4fv(modelUniformLocation, 1, GL_FALSE, glm::value_ptr(glm::mat4()));
	glUniformMatrix4fv(viewUniformLocation, 1, GL_FALSE, glm::value_ptr(glm::mat4()));
	glUniform4fv(colorUniformLocation, 1, glm::value_ptr(glm::vec4()));

	fd = new FluidDisplay(vPosition, vColor);

	fakeGrid = new glm::vec4[N*N]();
	for (int i = 0; i < N*N; i++) {
		fakeGrid[i] = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
	}

	fd->init();
	fd->update(fakeGrid);

	fluid = new Fluid(0.1f, 0.0001f, 0.0000001f, fd);

	glEnable(GL_DEPTH_TEST);
	glClearColor(1.0, 1.0, 1.0, 1.0);
}

//----------------------------------------------------------------------------

void display(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	fluid->displayFluid();

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

glm::vec2 prevLocation;
int densityState = 0;
void mouse(int button, int state, int x, int y)
{
	if (state == GLUT_DOWN) {
		prevLocation = glm::vec2(x, y);

		switch (button) {
			case GLUT_LEFT_BUTTON:    densityState = 0;  break;
			case GLUT_RIGHT_BUTTON:   densityState = 1;  break;
			case GLUT_MIDDLE_BUTTON:  densityState = 2;  break;
		}
	}
}

//----------------------------------------------------------------------------

void mouseMove(int x, int y)
{
	glm::vec2 moveDir = glm::normalize(glm::vec2(x, y) - prevLocation);

	if (densityState == 0)
		fluid->addDensity(x / scale, y / scale, 20.0f);
	else if (densityState == 1)
		fluid->addVelocity(x / scale, y / scale, moveDir.x, moveDir.y);
	else {
		fluid->addDensity(x / scale, y / scale, 20.0f);
		fluid->addVelocity(x / scale, y / scale, moveDir.x, moveDir.y);
	}


	prevLocation = glm::vec2(x, y);
}

//----------------------------------------------------------------------------

void update(void)
{
	fluid->velocityStep();
	fluid->densityStep();
	fluid->updateDisplay();
}

//----------------------------------------------------------------------------

void reshape(int width, int height)
{
	glViewport(0, 0, width, height);

	glm::mat4  projection = glm::ortho(0.0f, (GLfloat)N * scale, (GLfloat)N * scale, 0.0f);

	glUniformMatrix4fv(projectionUniformLocation, 1, GL_FALSE, glm::value_ptr(projection));
}
