#ifndef CAPACITATED_EDGE_H
#define CAPACITATED_EDGE_H

#include <cassert>
#include <iostream>

#include "Edge.h"

using namespace std;

class CapacitatedEdge : public Edge {
public:
	double weight;
	int capacity;
	
	CapacitatedEdge();
	CapacitatedEdge(int head, int tail, double weight, int capacity);
};

istream & operator >> (istream & in, CapacitatedEdge & e);

#endif
