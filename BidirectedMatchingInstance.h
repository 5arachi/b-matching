#ifndef BIDIRECTEDMATCHINGINSTANCE_H
#define BIDIRECTEDMATCHINGINSTANCE_H

#include <cassert>
#include <iostream>
#include <vector>

#include "CapacitatedbMatchingInstance.h"
#include "BidirectedGraph.h"
#include "CapacitatedGraph.h"
#include "misc.h"

using namespace std;

class BidirectedMatchingInstance {
public:
	BidirectedGraph G;
	Settings settings;

	BidirectedMatchingInstance(BidirectedGraph G, Settings settings = Settings());
	CapacitatedbMatchingInstance reduceToCapacitatedbMatching();
	void solve(bool &feasible, vector<double> &x, double &weight);
	bool isFeasible(const vector<double> &x);
};

#endif
