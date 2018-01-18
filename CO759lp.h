#ifndef CO759LP_H_
#define CO759LP_H_

#include <cassert>
#include <cplex.h>
#include <string>
#include <vector>
#include <iostream>

#include "Constraint.h"

using namespace std;

class CO759lp {
public:
	CPXENVptr cplex_env;
	CPXLPptr cplex_lp;

	CO759lp(string name);
	~CO759lp();
	void newRow(char sense, double rhs);
	void addColumn(double obj, const vector<int> &cmatind,
	               const vector<double> &cmatval, double lb, double ub);
	void opt(bool &infeasible);
	void optIP(bool &infeasible);
	int getStat();
	double objval();
	vector<double> x();
	void write(string fname);
	void changeBounds(int variable, double lb, double ub);
	void changeBounds(const vector<double> &lb, const vector<double> &ub);
	void addConstraint(const Constraint &c);
	void deleteRow(int rowId);
	void deleteRows(int firstRow, int lastRow);
	vector<double> slack(int firstRow, int lastRow);
	int numberOfRows();
	int numberOfColumns();
	void changeRHS(int rowId, double RHS);
	vector<double> reducedCosts();
	void addIntegralityConstraints();
	void removeIntegralityConstraints();
};

#endif
