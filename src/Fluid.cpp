#include "Fluid.h"

/*
Initializing new Fluid container
	dt: time step
	diff: diffussion rate
	visc: viscosity rate
	fd: FluidDisplay for rendering density
*/
Fluid::Fluid(float dt, float diff, float visc, FluidDisplay *fd)
{
	this->dt = dt;
	this->diff = diff;
	this->visc = visc;
	this->fd = fd;

	// initialize all values
	velocityX = new float[N * N]();
	velocityY = new float[N * N]();
	previousVelocityX = new float[N * N]();
	previousVelocityY = new float[N * N]();
	dens = new float[N * N]();
	previousDens = new float[N * N]();
}

/*
add density to location
cannot add density outside of grid
	x: x location of cell
	y: y location of cell
	amount: amount of density to add
*/
void Fluid::addDensity(int x, int y, float amount)
{
	if (x >= 0 && x < N && y >= 0 && y < N)
		dens[IX(x, y)] += amount;
}

/*
add velocity to location
cannot add velocity outside of grid
	x: x location of cell
	y: y location of cell
	amountX: amount of velocity in x direction to add
	amountY: amount of velocity in y direction to add
*/
void Fluid::addVelocity(int x, int y, float amountX, float amountY)
{
	if (x >= 0 && x < N && y >= 0 && y < N) {
		velocityX[IX(x, y)] += amountX;
		velocityY[IX(x, y)] += amountY;
	}
}

/*
Set boundary values so no density escapes the grid
	boundaryType:	set to 0 to copy values from adjacent cells into edge cells
					set to 1 to set horizontal edges
					set to 2 to set vertical edges
	grid: the grid to set boundary values to
*/
void Fluid::setBoundaryValues(int boundaryType, float * grid)
{
	// edges
	for (int i = 1; i <= (N - 2); i++) {
		grid[IX(0, i)] = boundaryType == 1 ? -grid[IX(1, i)] : grid[IX(1, i)];
		grid[IX(N - 1, i)] = boundaryType == 1 ? -grid[IX(N - 2, i)] : grid[IX(N - 2, i)];
		grid[IX(i, 0)] = boundaryType == 2 ? -grid[IX(i, 1)] : grid[IX(i, 1)];
		grid[IX(i, N - 1)] = boundaryType == 2 ? -grid[IX(i, N - 2)] : grid[IX(i, N - 2)];
	}

	// corners
	grid[IX(0, 0)] = 0.5 * (grid[IX(1, 0)] + grid[IX(0, 1)]);
	grid[IX(0, N - 1)] = 0.5 * (grid[IX(1, N - 1)] + grid[IX(0, N - 2)]);
	grid[IX(N - 1, 0)] = 0.5 * (grid[IX(N - 2, 0)] + grid[IX(N - 1, 1)]);
	grid[IX(N - 1, N - 1)] = 0.5 * (grid[IX(N - 2, N - 1)] + grid[IX(N - 1, N - 2)]);
}

/*
Diffuse step of a grid
	boundaryCondition: boundary condition when fixing boundary cells (see setBoundaryValues for more information)
	grid: the grid to diffuse its values
	previousGrid: the previous values of the grid
	rate: diffussion rate / viscosity 
*/
void Fluid::diffuse(int boundaryCondition, float * grid, float * previousGrid, float rate)
{
	// diffussion/viscosity rate 'a' depends on constant diff, and the size of the grid (based on to Navier stokes equation)
	float a = dt * diff * (N - 2) * (N - 2); 

	// solving system of linear equations
	// number of iterations for Gauss-Seidel algorithm
	for (int k = 0; k < 20; k++) {
		for (int i = 1; i <= (N - 2); i++) {
			for (int j = 1; j <= (N - 2); j++) {
				grid[IX(i, j)] = (previousGrid[IX(i, j)] + a * (grid[IX(i - 1, j)] + grid[IX(i + 1, j)] + grid[IX(i, j - 1)] + grid[IX(i, j + 1)])) / (1 + 4 * a);
				
				// protection against a rare bug where sometimes we get NaN as a value in c++ (probably because of how memory is stored)
				if (grid[IX(i, j)] != grid[IX(i, j)]) {
					grid[IX(i, j)] = 0;
				}
			}
		}

		setBoundaryValues(boundaryCondition, grid);
	}
}

/*
Move a grid of values based on a velocity grid
This one can be a bit hard to understand, so for more information, see the paper that came with this project
	boundaryCondition: boundary condition when fixing boundary cells (see setBoundaryValues for more information)
	grid: the grid to move its values
	previousGrid: the previous values of the grid
	velocityX: the x-direction velocities to use when moving the grid
	velocityY: the y-direction velocities to use when moving the grid
*/
void Fluid::move(int boundaryCondition, float *grid, float *previousGrid, float *velocityX, float *velocityY)
{
	float x, y;				// x,y of current cell
	int i0, j0, i1, j1;		// x,y values of 4 closest cells to x,y
	float s0, t0, s1, t1;	// distances from x,y to the 4 closest cells
	float dt0;				// time step back in time amount

	dt0 = dt * (N - 2); // step back (based on Navier Stokes)

	// for every cell
	for (int i = 1; i <= (N - 2); i++) {
		for (int j = 1; j <= (N - 2); j++) {
			// find previous location of particle (x,y)
			x = i - dt0 * velocityX[IX(i, j)];
			y = j - dt0 * velocityY[IX(i, j)];

			// fixing x points falling outside the boundaries of the container
			if (x < 0.5) x = 0.5; 
			if (x > (N - 2) + 0.5) x = (N - 2) + 0.5; 
			
			// x coordinates of 4 closest cells to (x,y)
			i0 = (int) x; 
			i1 = i0 + 1;
			
			// fixing y points falling outside the boundaries of the container
			if (y < 0.5) y = 0.5; 
			if (y > (N - 2) + 0.5) y = (N - 2) + 0.5; 
			
			// y coordinates of 4 closest cells to (x,y)
			j0 = (int) y; 
			j1 = j0 + 1;

			// calculating distances between x,y and centers of 4 closest cells
			s1 = x - i0;
			s0 = 1 - s1;
			t1 = y - j0;
			t0 = 1 - t1;

			// linear interpolation between 4 closest cells to (x,y) to determine new value at cell
			grid[IX(i, j)] = s0 * (t0 * 
				previousGrid[IX(i0, j0)] + t1 * 
				previousGrid[IX(i0, j1)]) + s1 * 
				(t0 * previousGrid[IX(i1, j0)] + t1 * previousGrid[IX(i1, j1)]);
		}
	}

	setBoundaryValues(boundaryCondition, grid);
}

/*
fix velocity grid such that it becomes mass conserving
this is probably the most complex algorithm in this solver
for more information see the paper that comes with this project
*/
void Fluid::fixMassConservation()
{
	float h = 1.0 / (N - 2);

	// compute divergence
	for (int i = 1; i <= (N - 2); i++) {
		for (int j = 1; j <= (N - 2); j++) {
			previousVelocityY[IX(i, j)] = -0.5 * h * (velocityX[IX(i + 1, j)] - velocityX[IX(i - 1, j)] + velocityY[IX(i, j + 1)] - velocityY[IX(i, j - 1)]);
			previousVelocityX[IX(i, j)] = 0;
		}
	}

	setBoundaryValues(0, previousVelocityY);
	setBoundaryValues(0, previousVelocityX);

	// solve Posson equation with Gauss-Seidel algorithm
	for (int k = 0; k < 20; k++) {
		for (int i = 1; i <= (N - 2); i++) {
			for (int j = 1; j <= (N - 2); j++) {
				previousVelocityX[IX(i, j)] = (previousVelocityY[IX(i, j)] + previousVelocityX[IX(i - 1, j)] + previousVelocityX[IX(i + 1, j)] +
					previousVelocityX[IX(i, j - 1)] + previousVelocityX[IX(i, j + 1)]) / 4;
			}
		}

		setBoundaryValues(0, previousVelocityX);
	}

	// subtract gradient field
	for (int i = 1; i <= (N - 2); i++) {
		for (int j = 1; j <= (N - 2); j++) {
			velocityX[IX(i, j)] -= 0.5*(previousVelocityX[IX(i + 1, j)] - previousVelocityX[IX(i - 1, j)]) / h;
			velocityY[IX(i, j)] -= 0.5*(previousVelocityX[IX(i, j + 1)] - previousVelocityX[IX(i, j - 1)]) / h;
		}
	}

	setBoundaryValues(1, velocityX);
	setBoundaryValues(2, velocityY);
}

/*
Density step
called every update()
*/
void Fluid::densityStep()
{
	// !IMPORTANT! need to make sure that we add new density before calling densityStep()
	SWAP(previousDens, dens);

	diffuse(0, dens, previousDens, diff);

	SWAP(previousDens, dens);

	move(0, dens, previousDens, velocityX, velocityY);
}

/*
velocity step
called every update()
*/
void Fluid::velocityStep()
{
	// !IMPORTANT! need to make sure that we add new velocity before calling velocityStep()
	SWAP(previousVelocityX, velocityX);

	diffuse(1, velocityX, previousVelocityX, visc);

	SWAP(previousVelocityY, velocityY);

	diffuse(2, velocityY, previousVelocityY, visc);
	fixMassConservation();

	SWAP(previousVelocityX, velocityX);
	SWAP(previousVelocityY, velocityY);

	move(1, velocityX, previousVelocityX, previousVelocityX, previousVelocityY);
	move(2, velocityY, previousVelocityY, previousVelocityX, previousVelocityY);

	fixMassConservation();
}

/*
update display colors
*/
void Fluid::updateDisplay()
{
	glm::vec4 *colors = new glm::vec4[N*N];
	for (int i = 0; i < N*N; i++) {
		float currColor = 1 - dens[i];
		colors[i] = glm::vec4(currColor, currColor, currColor, 1.0f);
	}

	fd->update(colors);
}

/*
display density grid on screen
*/
void Fluid::displayFluid()
{
	fd->display();
}
