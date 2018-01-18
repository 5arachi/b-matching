#ifndef CONSTRAINT_H
#define CONSTRAINT_H

#include <cassert>
#include <cmath>
#include <iostream>
#include <vector>

using namespace std;

class Constraint {
public:
	vector<int> varIndices;
	vector<double> varCoefficients;
	char sense;
	double RHS;
	int age;

	Constraint(const vector<int> &varIndices, const vector<double> &varCoefficients, char sense, double RHS);
};

#endif
