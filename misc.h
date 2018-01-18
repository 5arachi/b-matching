#ifndef MISC_H
#define MISC_H

#include <cassert>
#include <cmath>
#include <vector>
#include <iostream>

#define EPSILON 0.001

using namespace std;

double fractionalPart(double x);

bool isIntegral(double x, double EPS = EPSILON);

bool isIntegral(const vector<double> &x, double EPS = EPSILON);

int randomInteger(int Min, int Max);

double CO759_zeit(void);

double CO759_real_zeit(void);

#endif
