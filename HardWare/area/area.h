#ifndef __area__H
#define __area__H
#define MaxSize 4000 // ??4000 ??  

#include <stdio.h> 
#include <math.h>
#include "stdbool.h"
struct area_point
{
	double x;
	double y;
};
struct points_Status
{
	int index;
	int round;
};
struct center_ij
{
	int x;
	int y;
};


void areaCalcInit(double width);
double areaCalcular_rec(double  latitude, double longitude);
double N_Grids(struct area_point P_input, int Dmax, float width);
int Is_contained(struct center_ij c);
double Num_of_Inserting(double length);
struct area_point Insert(struct area_point p1, struct area_point p2, double w );
struct area_point BLtoXY(double latitude, double longitude);					   
double Distance(struct area_point p1, struct area_point p2); 


#endif