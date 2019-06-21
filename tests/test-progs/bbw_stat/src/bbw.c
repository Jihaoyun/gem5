//
// bbw.c
//
// This program implements a sliding mode type ABS controller for brake-by-wire implementation
// on a 7 DOF vehicle model
// Suvadeep Banerjee
// 09/12/2016

// Original paper
// S. Anwar and B. Zheng, "An Antilock-Braking Algorithm for an Eddy-Current-Based Brake-By-Wire System," in IEEE Transactions on Vehicular Technology, vol. 56, no. 3, pp. 1100-1107, May 2007.

#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <fcntl.h>

#include "util/m5/m5op.h"

double findSign(double inputNum){
	double returnVal;
	if (inputNum > 0)
		returnVal = 1;
	else if (inputNum < 0)
		returnVal = -1;
	else 
		returnVal = 0;
	return returnVal;
}

double findSum(double inputArr[]){
	int i;
	double returnVal = 0;
	for (i=0;i<4;i++)
		returnVal = returnVal + inputArr[i];

	return returnVal;
}

double MARSModelEval(double inputVal){
	double f1;
	if (inputVal <= 39)
		f1 = 0;
	else if (inputVal > 39 && inputVal < 84){
		double p1 = -0.0148;
		double r1 = 0.00038409;
		f1 = p1 * pow((inputVal - 39),2) + r1 * pow((inputVal - 39),3);
	}
	else if (inputVal >= 84)
		f1 = inputVal - 79;

	double f2;
	if (inputVal <= 39)
		f2 = -(inputVal - 79);
	else if (inputVal > 39 && inputVal < 84){
		double p2 = 0.0370;
		double r2 = 0.00038409;
		f2 = p2 * pow((inputVal - 84),2) + r2 * pow((inputVal - 84),3);
	}
	else if (inputVal >= 84)
		f2 = 0;

	double returnVal = 77 + 1.1 * f1 - 0.98 * f2;
	return returnVal;
}



int main(int argc, char **argv) {
	FILE *fp;
	if (argc < 3)
		fp = fopen("simData.dat", "w");
	else
		fp = fopen(argv[1], "w");

	// Parameters for Vehicle Model (Plant)

	double delta = 0;	// Steering angle, assuming straight line braking
	double M = 200;		// Vehicular mass (in kg)
	double Rw = 0.4;	// Wheel radius (in m)
	double Jv = 500; 	// Vehicle inertia (in kgm^2)
	double Jw = 0.6; 	// Wheel inertia (in kgm^2)
	double Fz = 2000;	// Vertical force offered by road
	
	// Parameters of sliding mode controller

	double alpha = 5.5;	// Slip-friction slope
	double kappa_th = 1.1;	// Slip threshold
	double phi = 1.5;	// Boundary layer thickness
	double eta = 0.8;	// Convergence Factor
	double sat;

	// Parameters for including road-slope

	double road_slope_angle = 0 * 2 * M_PI/180;	// Modulate this to include slope in the equations
	double g = 9.81;	// Acceleration due to gravity

	// Simulation parameters

	double ts = 0.01;	// Sampling time
	double duration = 20;	// Simulation duration 
	double timepts = duration/ts;
	timepts = timepts + 1;

	// Initialization of vectors

	double t[(int)timepts];	// Time Vector
	t[0] = 0;
	int i = 1;

	for (i =1; i < timepts; i++){
		t[i] = t[i-1] + ts;
	}

	double vx[(int)timepts];	// linear velocity
	double w[4][(int)timepts];	// 4 wheel angular velocities
	double kappa[4][(int)timepts];	// 4 wheel slip ratios
	double Tb[4][(int)timepts];	// 4 braking torques
	double mu[4];			// 4 friction coefficient-slip ratio relationships
	double wdot[4];			// 4 angular wheel derivatives
	double vxdot;			// Vehicular speed derivative

	double checkVal[(int)timepts];	// Check value vector
	for (i = 0; i<(int)timepts; i++) {
		checkVal[i] = 0;
	}

	// Simulation of the system

	// Initial velocity and angular speed

	int j;
	for (i=0;i<99;i++){
		vx[i] = 96.5;	// Velocity in km/hr
		for (j=0;j<4;j++)
			w[j][i] = 88.45;	// Angular vel in rad/s (needs to be changed accordingly with vx)
	}

	
	// Start of braking from time index 100 (i=99)

	for (i=99;i<(int)timepts;i++){
		double vx_prop_units = vx[i-1] * 1000 / 3600;
		for (j=0;j<4;j++)
			kappa[j][i-1] = 1 - ((Rw * w[j][i-1])/vx_prop_units);

		// Compute friction coefficient-slip ratio relationship for each wheel

		for (j=0;j<4;j++){
			if (kappa[j][i-1] <= kappa_th)
				mu[j] = alpha*kappa[j][i-1];
			else
				mu[j] = alpha*kappa_th;

			// Sliding mode controller output

			if (fabs((kappa_th - kappa[j][i-1])/phi) <= 1)
				sat = (kappa_th - kappa[j][i-1])/phi;
			else
				sat = findSign((kappa_th - kappa[j][i-1])/phi);

			// Braking torque equation

			Tb[j][i] = (Rw * alpha * kappa[j][i-1] * Fz) + ((Jw/vx[i-1]) * (w[j][i-1] / M) * alpha * kappa[j][i-1] * Fz) + (eta * (Jw/Rw) * vx[i-1] * sat);
		}

		// Update the state equations
		//rpns();

		vxdot = -(1/M) * findSum(mu) * Fz;
		vx[i] = vx[i-1] + vxdot * ts;

		for (j=0;j<4;j++){
			rpns();
			wdot[j] = (1/Jw) * (-Tb[j][i] + mu[j] * Fz * Rw) + 0.02*M*g*sin(road_slope_angle); // for road slope
			w[j][i] = w[j][i-1] + wdot[j] * ts;

			if (w[j][i] < 0)
				w[j][i] = 0;
		}

		if (vx[i] <= 0){
			vx[i] = 0;
			//rpns();
			break;
		}

		// Create check error data

		double pred1 = MARSModelEval(w[0][i-1]);
		double pred2 = MARSModelEval(w[1][i-1]);
		double pred3 = MARSModelEval(w[2][i-1]);
		double pred4 = MARSModelEval(w[3][i-1]);

		double err1 = pred1 - w[0][i];
		double err2 = pred2 - w[1][i];
		double err3 = pred3 - w[2][i];
		double err4 = pred4 - w[3][i];

		checkVal[i] = (err1 + err2 + err3 + err4)/4;
		
		//rpns();
			
	}

	// Write Data to output file
	
	for (i=0;i<(int)timepts;i++){
		fprintf(fp, "%f %f %f %f %f %f\n", t[i], w[0][i], w[1][i], w[2][i], w[3][i], vx[i]);
	}

	fclose(fp);

	FILE *cp;
	if (argc < 3)
        cp = fopen("checkData.dat", "w");
    else
        cp = fopen(argv[2], "w");
	for (i=0;i<(int)timepts;i++){
		fprintf(cp, "%f %f\n", t[i], checkVal[i]);
	}

	fclose(cp);
	
	return 0;

}















