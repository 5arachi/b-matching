#ifndef CAPACITATED_GRAPH_H
#define CAPACITATED_GRAPH_H

#include <iostream>

#include "CapacitatedEdge.h"
#include "Graph.h"
#include "misc.h"

using namespace std;

class CapacitatedGraph : public Graph<CapacitatedEdge> {
public:
	vector<int> b;

	CapacitatedGraph(int n = 0);
};

istream & operator >> (istream & in, CapacitatedGraph & G);

CapacitatedGraph randomCompleteCapacitatedGraph(int seed, int n, int minB = 2,
        int maxB = 2, int minWeight = 0, int maxWeight = 1000,
        int minCapacity = 1, int maxCapacity = 1);

CapacitatedGraph randomCapacitatedGraph(int seed, int n, double density, int minB = 2,
                                        int maxB = 2, int minWeight = 0, int maxWeight = 1000,
                                        int minCapacity = 1, int maxCapacity = 1);

#endif
