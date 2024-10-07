/* math.h - log, pow, expdev */

#ifndef _MATH_H_
#define _MATH_H_

// Maths constants
#define INF 1.0 / 0.0
#define NAN 0.0 / 0.0

// Function prototypes
double log(double x);         // Calculate Natural log
double pow(double x, int y);  // Calaculate x to the power y
double expdev(double lambda); // Generate random number as per exponential distribution

#endif // _MATH_H_