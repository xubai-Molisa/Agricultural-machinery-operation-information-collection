#include "area.h"

//static int MaxSize = 4000;
static int Dmax = 35;                    //×î´ó¾àÀëÃÅÏŞ 
static double TotalAcreage = 0;		 
static double width = 0;                 //¸û¾ß¿í, ³õÊ¼º¯ÊıÀïÉèÖÃ 
static double n_ins2 = 0;      
static double side_length = 0.5;          // Õ¤¸ñ¿í¶È 
struct area_point P_last ;
struct area_point origin;
static int first_p = 0;  // ÊÇ·ñÎªµÚÒ»¸öµã 
static int origin_state = 0; // Ô­µãÊÇ·ñ¶¨ÁË 
struct center_ij Past_Center_Points[MaxSize];  //ÀúÊ·¶àÉÙµã²éÖØ®ä4000µã 
static struct points_Status Past_Center_Points_Status; //ç¼“å­˜é˜Ÿåˆ—æŒ‡é’ˆ 


void areaCalcInit(double toolWidth)
{
	width = toolWidth;
			
	n_ins2 = Num_of_Inserting(width/2);    // ¸ù¾İ ¸û¾ß¿í¶È È·¶¨´¹Ö±ÓÚ¹ì¼£·½ÏòµÈ·Ö¶àÉÙ´Î 
	
	TotalAcreage = 0;
	Past_Center_Points_Status.index = 0;
	Past_Center_Points_Status.round = 0;
	origin_state = 0; 
	first_p = 0;
	P_last.x = 0; P_last.y = 0;
}


double areaCalcular_rec(double latitude, double longitude)
{
	double res;
	struct area_point Input;

	if (latitude == 0 || longitude == 0)
		res = 0;
	else
	{
		Input = BLtoXY(latitude, longitude);
		res = N_Grids(Input, Dmax, width);
		
	}
	TotalAcreage += res * side_length * side_length / 666.7;
	return TotalAcreage;
}

double N_Grids(struct area_point P_input, int Dmax, float width) 
{
	int i,j;    // Ñ­»·ÓÃ 
	double n_ins1;  //Á½µãÖ®¼ä·ÖÎª¶àÉÙ¶Î 
	double d; // ÊäÈëµãÓëÉÏÒ»¸öµãÖ®¼äµÄ¾àÀë
	struct area_point Pm_temp, P_temp1;
	struct area_point Pm[140][8];  // Á½µã²åµãºóµÄËùÓĞµã 
	int index1 = 0; 
	int index2 = 1;
	struct area_point delta, delta_temp; //ºáÏò²åµã×ø±ê±ä»¯Á¿ 
	struct center_ij center_temp;
	int contained; 
	double num = 0; 
	
	 
	 
	
	if (P_input.x == 0 || P_input.y == 0)    // µãÓĞÎÊÌâ 
		return 0;  
	if (first_p == 0)  // µÚÒ»¸öµã£¬Ã»ÓĞÉÏÒ»¸öµã  
		{
			P_last = P_input;
			origin = P_input;
			first_p = 1;
			return 0; 
		}		
	else 
	{
		d = Distance(P_last, P_input);
		if (d > Dmax)     // Á½µãÖ®¼ä¾àÀëÌ«´ó 
		{
			P_last = P_input;
			return 0; 
		}
		if(origin_state == 0)
		{
			if (d <= Dmax)
			{
				origin_state = 1;  //Ô­µãºÏ¸ñÁË    
			}
			origin = P_input;   // Ô­µãÎªºÏ¸ñµÄµÚÒ»¸öÏß¶ÎµÄµÚ¶ş¸öµã 
			
		}
		
		//È·¶¨ÕâÁ½µãÖ®¼ä·ÖÎª¶àÉÙ¶Î  
		n_ins1 =  Num_of_Inserting(d);
		
		// ²åµãµÄ¼ä¸ô 
		P_temp1.x = (P_input.x - P_last.x)/n_ins1;
		P_temp1.y = (P_input.y - P_last.y)/n_ins1;
		
		// ¹ì¼£²åµã½áÊø 
		for (j=1;j<=n_ins1;j++)
		{
			Pm_temp.x = P_last.x + (j-1) * P_temp1.x;
			Pm_temp.y = P_last.y + (j-1) * P_temp1.y;
			Pm[index1][0] = Pm_temp;  index1 ++;	
		}
		
		// ¹ì¼£Á½±ß¿ªÊ¼²åµã 
		delta = Insert(P_last, P_input, width);
		delta_temp.x = delta.x / n_ins2;
		delta_temp.y = delta.y / n_ins2;
		// ²åµã 
		for (i=1;i<=n_ins2;i++)
		{
			for (j=0;j<index1;j++)
			{
				Pm[j][2*i].x = Pm[j][0].x - delta_temp.x * i;
				Pm[j][2*i].y = Pm[j][0].y + delta_temp.y * i;
				Pm[j][2*i-1].x = Pm[j][0].x + delta_temp.x * i;
				Pm[j][2*i-1].y = Pm[j][0].y - delta_temp.y * i;
				
			}
			index2 += 2;
		}
		
		// ÕÒPmÖĞËùÓĞµãµÄÖĞĞÄ
		for (i=0;i<index2;i++) //col
		{
			for (j=0;j<index1;j++) //row
			{
				center_temp.x = (int)(floor((Pm[j][i].x - origin.x) / side_length));
				center_temp.y = (int)(floor((Pm[j][i].y - origin.y) / side_length));
				//printf("%d\n",center_temp.x);                                       // ²âÊÔ²é¿´ 
				contained = Is_contained(center_temp);
				num += contained; 
			}
		} 
		
		
	}
	P_last = P_input;
 	return num;
}

 
 /*
	ÀúÊ·4000¸öÕ¤¸ñÖĞĞÄ²éÖØ
 	·µ»Ø1 ÓĞĞ§  ·µ»Ø0 ÖØ¸´ÁËÃ»Ğ§ 
 */
int Is_contained(struct center_ij c)
{
	int i; // for loop

	// full?
	if (Past_Center_Points_Status.index == MaxSize) 
		{
			Past_Center_Points_Status.round ++;
			Past_Center_Points_Status.index = Past_Center_Points_Status.index % MaxSize;
		}
	 
	if (Past_Center_Points_Status.round == 0)  // Ã»Âú 
	{
		if (Past_Center_Points_Status.index == 0)  // ¿Õ
		{
			Past_Center_Points[Past_Center_Points_Status.index] = c; // rear = 0
			Past_Center_Points_Status.index ++;  // rear = 1	
			return 1;		
		}
		else
		{
			for(i=1;i<Past_Center_Points_Status.index;i++) //²éÖØ 
			{
				if (c.x == Past_Center_Points[i].x && c.y == Past_Center_Points[i].y)
				{
					return 0;
				}							
			}
			Past_Center_Points[Past_Center_Points_Status.index] = c; 
			Past_Center_Points_Status.index ++; 
			return 1;
		 } 				
	}
	else     // full  .round > 1
	{	
		for(i=0;i<MaxSize;i++)
		{
			if(c.x == Past_Center_Points[i].x && c.y == Past_Center_Points[i].y)
				return 0;
		}
		Past_Center_Points[Past_Center_Points_Status.index] = c; 
		Past_Center_Points_Status.index ++; 
		return 1;
	 } 		 
	
 } 
 
 /*È·¶¨²åµã´ÎÊı*/
double Num_of_Inserting(double length)
{
	double i,interval,n;
	for(i=0;i<100;i++)
	{
		n = pow(2, i);
		interval = length / n;
		if (interval <= side_length)
			return n;
	}
	return 0;
 } 

/*ºáÏò²åµã¼ä¸ô*/
struct area_point Insert(struct area_point p1, struct area_point p2, double w )
{
	double X,Y,theta;
	struct area_point delta;
	X = p2.x - p1.x;
	Y = p2.y - p1.y;
	theta = atan2(Y,X);
	delta.x = sin(theta) * w / 2;
	delta.y = cos(theta) * w / 2;
	return delta;	
 } 



struct area_point BLtoXY(double latitude, double longitude)
{
	double f;
	double e1;
	double a;
	double a1, a2, a3, a4;
	double b1, b2, b3, b4;
	double c0, c1, c2, c3;
	struct area_point x;
	double IPI;
	double B;
	double L;
	double L_0;
	double sinB;
	double cosB;
	double tanB;
	double l;
	double m;
	double N;
	double q2;
	double s;
	int Datum;

	Datum = 80;
	if (Datum == 84)
	{
		a = 6378137;
		f = 1 / 298.257223563;
	}
	else if (Datum == 54)
	{
		a = 6378245;
		f = 1 / 298.3;
	}
	else if (Datum == 80)
	{
		a = 6378140;
		f = 1 / 298.257;
	}
	e1 = 2 * f - f * f; //(a*a-b*b)/(a*a) 

	a1 = 1 + 3.0 / 4.0 * e1 + 45.0 / 64.0 * pow(e1, 2) + 175.0 / 256.0 * pow(e1, 3) + 11025.0 / 16384.0 * pow(e1, 4) + 43659.0 / 65536.0 * pow(e1, 5);
	a2 = 3.0 / 4.0 * e1 + 15.0 / 16.0 * pow(e1, 2) + 525.0 / 512.0 * pow(e1, 3) + 2205.0 / 2048.0 * pow(e1, 4) + 72765.0 / 65536.0 * pow(e1, 5);
	a3 = 15.0 / 64.0 * pow(e1, 2) + 105.0 / 256.0 * pow(e1, 3) + 2205.0 / 4096.0 * pow(e1, 4) + 10359.0 / 16384.0 * pow(e1, 5);
	a4 = 35.0 / 512.0 * pow(e1, 3) + 315.0 / 2048.0 * pow(e1, 4) + 31185.0 / 13072.0 * pow(e1, 5);
	b1 = a1 * a * (1 - e1);
	b2 = -1.0 / 2.0 * a2 * a * (1 - e1);
	b3 = 1.0 / 4.0 * a3 * a * (1 - e1);
	b4 = -1.0 / 6.0 * a4 * a * (1 - e1);
	c0 = b1;
	c1 = 2 * b2 + 4 * b3 + 6 * b4;
	c2 = -(8 * b3 + 32 * b4);
	c3 = 32 * b4;
	//printf("%lf ,%lf ,%lf ,%lf \n",c0,c1,c2,c3);

	IPI = 3.1415926535898 / 180.0;
	B = latitude * IPI;
	L = longitude * IPI;
	L_0 = 117 * IPI; //ä¸­å¤®ç»åº¦çš„å€¼ï¼Œæ ¹æ®è¾“å…¥åæ ‡å˜åŒ–
					 //å¸¦å·è®¡ç®—ï¼Œ6åº¦å¸¦å®½ä¸‹ï¼Œå¸¦å·=å–æ•´ï¼ˆï¼ˆè¾“å…¥ç‚¹ç»åº¦+6ï¼‰/6ï¼‰
					 //ä¸­å¤®ç»åº¦è®¡ç®—ï¼Œ6åº¦å¸¦å®½ä¸‹ï¼Œå½“åœ°ä¸­å¤®ç»åº¦=6åº¦*å¸¦å·-3åº¦ã€‚
					 //L_0å°±æ˜¯ä¸­å¤®ç»åº¦

	sinB = sin(B);
	cosB = cos(B);
	tanB = tan(B);
	l = L - L_0;
	m = l * cosB;
	N = a / sqrt(1 - e1 * pow(sinB, 2));
	q2 = e1 / (1 - e1) * pow(cosB, 2);

	s = c0 * B + cosB * (c1 * sinB + c2 * pow(sinB, 3) + c3 * pow(sinB, 5));

	x.x = s + 0.5 * N * tanB * pow(m, 2) + 1 / 24 * (5 - pow(tanB, 2) + 9 * q2 + 4 * pow(q2, 2)) * N * tanB * pow(m, 4) + 1 / 720 * (61 - 58 * pow(tanB, 6)) * N * tanB * pow(m, 6);
	x.y = N * m + 1 / 6 * (1 - pow(tanB, 2) + q2) * N * pow(m, 3) + 1 / 120 * (5 - 18 * tanB * tanB + pow(tanB, 4) - 14 * q2 - 58 * q2 * pow(tanB, 2)) * N * pow(m, 5) + 1000000 * 20 + 500000;
	return x;
}

double Distance(struct area_point p1, struct area_point p2)
{
	double x;
	x = sqrt(pow((p1.x - p2.x), 2) + pow((p1.y - p2.y), 2));
	return x;
}


