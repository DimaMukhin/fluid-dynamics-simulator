#include "Fluid.h"

Fluid::Fluid(float dt, float diff)
{
	this->dt = dt;
	this->diff = diff;

	u = new float[N * N]();
	v = new float[N * N]();
	u_prev = new float[N * N]();
	v_prev = new float[N * N]();
	dens = new float[N * N]();
	dens_prev = new float[N * N]();
}

// add density to location
// cannot add density outside of grid
void Fluid::addDensity(int x, int y, float amount)
{
	if (x >= 0 && x < N && y >= 0 && y < N)
		dens[IX(x, y)] += amount;
}

// add velocity to location
// cannot add velocity outside of grid
void Fluid::addVelocity(int x, int y, float amountX, float amountY)
{
	if (x >= 0 && x < N && y >= 0 && y < N) {
		u[IX(x, y)] += amountX;
		v[IX(x, y)] += amountY;
	}
}

// b is boundry type TODO: can be optimized!
// b can mean u, v, or dens
void Fluid::set_bnd(int b, float * x)
{
	// edges
	for (int i = 1; i <= (N - 2); i++) {
		x[IX(0, i)] = b == 1 ? -x[IX(1, i)] : x[IX(1, i)];
		x[IX(N - 1, i)] = b == 1 ? -x[IX(N - 2, i)] : x[IX(N - 2, i)];
		x[IX(i, 0)] = b == 2 ? -x[IX(i, 1)] : x[IX(i, 1)];
		x[IX(i, N - 1)] = b == 2 ? -x[IX(i, N - 2)] : x[IX(i, N - 2)];
	}

	// corners
	x[IX(0, 0)] = 0.5 * (x[IX(1, 0)] + x[IX(0, 1)]);
	x[IX(0, N - 1)] = 0.5 * (x[IX(1, N - 1)] + x[IX(0, N - 2)]);
	x[IX(N - 1, 0)] = 0.5*(x[IX(N - 2, 0)] + x[IX(N - 1, 1)]);
	x[IX(N - 1, N - 1)] = 0.5*(x[IX(N - 2, N - 1)] + x[IX(N - 1, N - 2)]);
}

// b will come into play later
// x is current grid of values (we will use velocity and density) [reusable]
// TODO: split this and make a private helper function so it is more OOP
// x0 is previous values of the above
void Fluid::diffuse(int b, float * x, float * x0)
{
	float a = dt * diff * (N - 2) * (N - 2); // diffussion rate 'a' depends on constant diff, and the size of the grid (probably according to Navier stokes

	// TODO: 20 is number of iterations for Gauss-Seidel algorithm. can probably lower it for more efficiency or make it constant variable
	for (int k = 0; k < 20; k++) {
		for (int i = 1; i < (N - 2); i++) {
			for (int j = 1; j < (N - 2); j++) {
				x[IX(i, j)] = (x0[IX(i, j)] + a * (x[IX(i - 1, j)] + x[IX(i + 1, j)] + x[IX(i, j - 1)] + x[IX(i, j + 1)])) / (1 + 4 * a);
			}
		}

		set_bnd(b, x);
	}
}

// d is current velocity or current dencity (so we make it reusable)
// d0 is the previous for d
// TODO: split this and make a private helper function so it is more OOP
// we need u and v passed because of reusability for vec step and vel step
void Fluid::advect(int b, float *d, float *d0, float *u, float *v)
{
	int i, j, i0, j0, i1, j1;
	float x, y, s0, t0, s1, t1, dt0;

	dt0 = dt * N;

	for (i = 1; i <= (N - 2); i++) {
		for (j = 1; j <= (N - 2); j++) {
			x = i - dt0 * u[IX(i, j)];
			y = j - dt0 * v[IX(i, j)];

			if (x < 0.5) x = 0.5; if (x > (N - 2) + 0.5) x = (N - 2) + 0.5; i0 = (int)x; i1 = i0 + 1;
			if (y < 0.5) y = 0.5; if (y > (N - 2) + 0.5) y = (N - 2) + 0.5; j0 = (int)y; j1 = j0 + 1;

			s1 = x - i0;
			s0 = 1 - s1;
			t1 = y - j0;
			t0 = 1 - t1;

			d[IX(i, j)] = s0 * (t0 * d0[IX(i0, j0)] + t1 * d0[IX(i0, j1)]) + s1 * (t0 * d0[IX(i1, j0)] + t1 * d0[IX(i1, j1)]);
		}
	}

	set_bnd(b, d);
}

void Fluid::project()
{
	int i, j, k;
	float h;

	h = 1.0 / (N - 2);

	for (i = 1; i <= (N - 2); i++) {
		for (j = 1; j <= (N - 2); j++) {
			v_prev[IX(i, j)] = -0.5 * h * (u[IX(i + 1, j)] - u[IX(i - 1, j)] + v[IX(i, j + 1)] - v[IX(i, j - 1)]);
			u_prev[IX(i, j)] = 0;
		}
	}

	set_bnd(0, v_prev);
	set_bnd(0, u_prev);

	for (k = 0; k < 20; k++) {
		for (i = 1; i <= (N - 2); i++) {
			for (j = 1; j <= (N - 2); j++) {
				u_prev[IX(i, j)] = (v_prev[IX(i, j)] + u_prev[IX(i - 1, j)] + u_prev[IX(i + 1, j)] +
					u_prev[IX(i, j - 1)] + u_prev[IX(i, j + 1)]) / 4;
			}
		}

		set_bnd(0, u_prev);
	}

	for (i = 1; i <= (N - 2); i++) {
		for (j = 1; j <= (N - 2); j++) {
			u[IX(i, j)] -= 0.5*(u_prev[IX(i + 1, j)] - u_prev[IX(i - 1, j)]) / h;
			v[IX(i, j)] -= 0.5*(u_prev[IX(i, j + 1)] - u_prev[IX(i, j - 1)]) / h;
		}
	}

	set_bnd(1, u);
	set_bnd(2, v);
}

void Fluid::dens_step()
{
	//add_source(); // this step was improved TODO: !IMPORTANT! need to make sure that we add new density before calling dens_step()
	SWAP(dens_prev, dens);
	diffuse(0, dens, dens_prev);
	SWAP(dens_prev, dens);
	advect(0, dens, dens_prev, u, v);
}

void Fluid::vel_step()
{
	//add_source(N, u, u0, dt); add_source(N, v, v0, dt); // this step was improved TODO: !IMPORTANT! need to make sure that we add new velocity before calling vel_step()
	SWAP(u_prev, u);
	diffuse(1, u, u_prev);
	SWAP(v_prev, v);
	diffuse(2, v, v_prev);
	project(); // TODO: dont know yet
	SWAP(u_prev, u); SWAP(v_prev, v);

	advect(1, u, u_prev, u_prev, v_prev);
	advect(2, v, v_prev, u_prev, v_prev);
	project(); // TODO: dont know yet
}
