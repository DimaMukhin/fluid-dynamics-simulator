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

	u = new float[N * N]();
	v = new float[N * N]();
	u_prev = new float[N * N]();
	v_prev = new float[N * N]();
	dens = new float[N * N]();
	dens_prev = new float[N * N]();
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
		u[IX(x, y)] += amountX;
		v[IX(x, y)] += amountY;
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

// b will come into play later
// x is current grid of values (we will use velocity and density) [reusable]
// x0 is previous values of the above
// rate is the rate of diffussion for density, and the rate of viscosity for velocity
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
			}
		}

		setBoundaryValues(boundaryCondition, grid);
	}
}

// d is current velocity or current dencity (so we make it reusable)
// d0 is the previous for d
// we need u and v passed because of reusability for vec step and vel step
void Fluid::advect(int b, float *d, float *d0, float *u, float *v)
{
	int i, j, i0, j0, i1, j1;
	float x, y, s0, t0, s1, t1, dt0;

	dt0 = dt * (N - 2); // step back (based on Navier Stokes)

	// for every cell
	for (i = 1; i <= (N - 2); i++) {
		for (j = 1; j <= (N - 2); j++) {
			// find previous location of particle (x,y)
			x = i - dt0 * u[IX(i, j)];
			y = j - dt0 * v[IX(i, j)];

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
			j0 = (int)y; j1 = j0 + 1;

			// calculating distances between x,y and centers of 4 closest cells
			s1 = x - i0;
			s0 = 1 - s1;
			t1 = y - j0;
			t0 = 1 - t1;

			// linear interpolation between 4 closest cells to (x,y) to determine new value at cell
			d[IX(i, j)] = s0 * (t0 * d0[IX(i0, j0)] + t1 * d0[IX(i0, j1)]) + s1 * (t0 * d0[IX(i1, j0)] + t1 * d0[IX(i1, j1)]);
		}
	}

	setBoundaryValues(b, d);
}

void Fluid::project()
{
	int i, j, k;
	float h;

	h = 1.0 / (N - 2);

	// compute divergence
	for (i = 1; i <= (N - 2); i++) {
		for (j = 1; j <= (N - 2); j++) {
			v_prev[IX(i, j)] = -0.5 * h * (u[IX(i + 1, j)] - u[IX(i - 1, j)] + v[IX(i, j + 1)] - v[IX(i, j - 1)]);
			u_prev[IX(i, j)] = 0;
		}
	}

	setBoundaryValues(0, v_prev);
	setBoundaryValues(0, u_prev);

	// solve Posson equation with Gauss-Seidel algorithm
	for (k = 0; k < 20; k++) {
		for (i = 1; i <= (N - 2); i++) {
			for (j = 1; j <= (N - 2); j++) {
				u_prev[IX(i, j)] = (v_prev[IX(i, j)] + u_prev[IX(i - 1, j)] + u_prev[IX(i + 1, j)] +
					u_prev[IX(i, j - 1)] + u_prev[IX(i, j + 1)]) / 4;
			}
		}

		setBoundaryValues(0, u_prev);
	}

	// subtract gradient field
	for (i = 1; i <= (N - 2); i++) {
		for (j = 1; j <= (N - 2); j++) {
			u[IX(i, j)] -= 0.5*(u_prev[IX(i + 1, j)] - u_prev[IX(i - 1, j)]) / h;
			v[IX(i, j)] -= 0.5*(u_prev[IX(i, j + 1)] - u_prev[IX(i, j - 1)]) / h;
		}
	}

	setBoundaryValues(1, u);
	setBoundaryValues(2, v);
}

void Fluid::dens_step()
{
	//add_source(); // this step was improved TODO: !IMPORTANT! need to make sure that we add new density before calling dens_step()
	SWAP(dens_prev, dens);
	diffuse(0, dens, dens_prev, diff);
	SWAP(dens_prev, dens);
	advect(0, dens, dens_prev, u, v);
}

void Fluid::vel_step()
{
	//add_source(N, u, u0, dt); add_source(N, v, v0, dt); // this step was improved TODO: !IMPORTANT! need to make sure that we add new velocity before calling vel_step()
	SWAP(u_prev, u);
	diffuse(1, u, u_prev, visc);
	SWAP(v_prev, v);
	diffuse(2, v, v_prev, visc);
	project();
	SWAP(u_prev, u); 
	SWAP(v_prev, v);

	advect(1, u, u_prev, u_prev, v_prev);
	advect(2, v, v_prev, u_prev, v_prev);

	project();
}

void Fluid::updateDisplay()
{
	glm::vec4 *colors = new glm::vec4[N*N];
	for (int i = 0; i < N*N; i++) {
		float currColor = 1 - dens[i];
		colors[i] = glm::vec4(currColor, currColor, currColor, 1.0f);
	}

	fd->update(colors);
}

void Fluid::displayFluid()
{
	fd->display();
}
