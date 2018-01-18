#ifndef FLOW_EDGE_H
#define FLOW_EDGE_H

#include <cassert>
#include <iostream>

#include "Edge.h"

using namespace std;

class FlowEdge : public Edge {
public:
	double capacity;
	double residualCapacity;
	int mate;

	FlowEdge();
	FlowEdge(int head, int tail, double capacity, int mate);
};

#endif
