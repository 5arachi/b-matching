#include "misc.h"

//#define DEBUG

double fractionalPart(double x) {
    double intpart;
    return modf(x, &intpart);
}

bool isIntegral(double x, double EPS) {
    double frac = fractionalPart(x);
    return frac < EPS || frac > 1.0 - EPS;
}

bool isIntegral(const vector<double> &x, double EPS) {
    for (double y : x) {
        if (!isIntegral(y)) {
            return false;
        }
    }
    return true;
}

int randomInteger(int Min, int Max) {

#ifdef DEBUG
    assert(Max >= Min);
#endif

    return Min + rand() % (Max - Min + 1);
}

/* zeit function for recording times */

double CO759_zeit (void) {
    struct rusage ru;

    getrusage (RUSAGE_SELF, &ru);

    return ((double) ru.ru_utime.tv_sec) +
           ((double) ru.ru_utime.tv_usec) / 1000000.0;
}

double CO759_real_zeit (void) {
    return (double) time (0);
}
