#include <math.h>

// Constants
#define RAND_MAX 077777
#define TAYLOR_SERIES_MAX_ITER 20

/* Calculate Natural log */
double log(double x)
{
    double log_value = 0;
    if (x < 0) {
        log_value = -NAN;
    } else if (x == 0) {
        log_value = -INF;
    } else if (x <= 2) {
        int i;
        for (i = 1; i <= TAYLOR_SERIES_MAX_ITER; i++) {
            log_value += pow(-1, i + 1) * pow(x - 1, i) * (1 / (double) i);
        }
    } else {
        log_value = log(x / 2) + log(2);
    }

    return log_value;
}

/* Calaculate x to the power y */
double pow(double x, int y)
{
    // x ^ y
    int i;
    double pow_value = 1;
    int change = y < 0 ? 1 : -1;
    for (i = y; i != 0; i += change) {
        pow_value *= x;
    }

    return y < 0 ? 1 / pow_value : pow_value;
}

/* Generate random number as per exponential distribution */
double expdev(double lambda)
{
    double dummy;
    do {
        dummy= (double) rand() / RAND_MAX;
    } while (dummy == 0.0);

    return -log(dummy) / lambda;
}