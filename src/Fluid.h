#pragma once

#include "common.h"
#include "FluidDisplay.h"

#define IX(i, j) ((i) + (N) * (j))

#define SWAP(x0,x) {float *tmp=x0;x0=x;x=tmp;}

class Fluid
{
public:
	Fluid(float dt, float diff, float visc, FluidDisplay *fd);

	void Fluid::addDensity(int x, int y, float amount);

	void Fluid::addVelocity(int x, int y, float amountX, float amountY);

	void setBoundaryValues(int boundaryType, float * grid);

	void diffuse(int boundaryCondition, float * grid, float * previousGrid, float diff);

	void move(int boundaryCondition, float *grid, float *previousGrid, float *velocityX, float *velocityY);

	void fixMassConservation();

	void densityStep();

	void velocityStep();

	void updateDisplay();

	void displayFluid();

private:
	float *velocityX, *velocityY;					// velocity grid in x and y
	float *previousVelocityX, *previousVelocityY;	// previous velocity grid in x and y

	float *dens;			// density grid
	float *previousDens;	// previous density grid

	float dt;	// time step

	float diff; // diffussion rate
	float visc; // viscosity of the fluid

	FluidDisplay *fd;
};