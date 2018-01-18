#ifndef BIDIRECTED_EDGE_H
#define BIDIRECTED_EDGE_H

#include <cmath>
#include <iostream>
#include <fstream>

#include "CapacitatedEdge.h"

using namespace std;

class BidirectedEdge : public CapacitatedEdge {
public:
	int signHead, signTail;

	BidirectedEdge();
	BidirectedEdge(int head, int signHead, int tail, int signTail, double weight, int capacity);
};

istream & operator >> (istream & in, BidirectedEdge & e);

#endif
