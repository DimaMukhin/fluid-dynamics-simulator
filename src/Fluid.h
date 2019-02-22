#pragma once

#include "common.h"

// TODO: can this be replaced with two dimensional array?
#define IX(i, j) ((i) + (N) * (j))

// TODO: should this be private helper function instead?
#define SWAP(x0,x) {float *tmp=x0;x0=x;x=tmp;}

class Fluid
{
public:
	Fluid(float dt, float diff); // TODO: add viscosity!!!

	void Fluid::addDensity(int x, int y, float amount);

	void Fluid::addVelocity(int x, int y, float amountX, float amountY);

	void set_bnd(int b, float *x);

	void diffuse(int b, float *x, float *x0);

	void advect(int b, float *d, float *d0, float *u, float *v); // this is what actually moves things based on the vector field

	void project();

	void dens_step(); // TODO: can this be private? can any of these be private? should any be static?

	void vel_step();

	~Fluid();

private:
							// TODO: can probably be converted to vectors
	float *u, *v;			// velocity grid in x and y
	float *u_prev, *v_prev; // previous velocity grid in x and y
	
	float *dens; // density grid
	float *dens_prev; // previous density grid

				// TODO: can probably be removed / globalized
	float dt;	// time step

	float diff; // diffussion rate
};