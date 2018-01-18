#ifndef CAPACITATEDBMATCHINGINSTANCE_H
#define CAPACITATEDBMATCHINGINSTANCE_H

#include <algorithm>
#include <cfloat>
#include <fstream>
#include <iostream>
#include <list>
#include <map>
#include <numeric>

#include "CO759lp.h"
#include "CapacitatedGraph.h"
#include "Constraint.h"
#include "FlowGraph.h"
#include "misc.h"
#include "Settings.h"
#include "Timer.h"

using namespace std;

class CapacitatedbMatchingInstance {
public:
	CapacitatedGraph G;
	CO759lp lp;
	CO759lp auxIP;
	Settings settings;

	double objectiveLB;
	vector<double> varLB, varUB;
	vector<double> bestbMatching;
	double bestbMatchingValue;
	list<Constraint> cutPool;
	bool integralWeights;

	Timer timeCPLEX;
	Timer timeLP;
	Timer timeAuxiliaryIP;
	Timer timeHeuristicSeparation;
	Timer timeExactSeparation;
	Timer timeFlow;
	Timer timeBranchAndCut;
	Timer timeCutPool;

	bool feasibleCPLEX;
	double optCPLEX;

	CapacitatedbMatchingInstance(CapacitatedGraph G, Settings settings = Settings());
	void resolveLP(bool &infeasible, vector<double> &x);
	void solve(bool &feasible, vector<double> &x_opt, double &weight);
	void solve(int level);
	void initializeLP();
	void improveLB();
	void variableFixing(const vector<double> &x);
	void initializeAuxiliaryIP();
	vector<Constraint> findViolatedInequalities(const vector<double> &x);
	void cleanCutPool();
	void findViolatedInequalityForHandle(const vector<double> &x, vector<int> S, vector<Constraint> &newCuts);
	vector<Constraint> heuristicSeparation(const vector<double> &x);
	vector<Constraint> exactSeparation(const vector<double> &x);
	void findViolatedInequalitiesInsideComponent(const vector<double> &x, FlowGraph & FG,
	        const vector<int> & oldName, vector<Constraint> &newCuts);
	Constraint addViolatedInequality(vector<int> W, const vector<int> &F);
	bool isFeasible(const vector<double> &x);
	bool validSettings();
	bool prune();
};

ostream & operator << (ostream & out, const CapacitatedbMatchingInstance & instance);

#endif
