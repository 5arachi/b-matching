#ifndef BIDIRECTED_GRAPH_H
#define BIDIRECTED_GRAPH_H

#include <cassert>
#include <iostream>

#include "BidirectedEdge.h"
#include "Graph.h"
#include "misc.h"

using namespace std;

class BidirectedGraph : public Graph<BidirectedEdge> {
public:
	vector<int> b;

	BidirectedGraph(int n = 0);
};

istream & operator >> (istream & in, BidirectedGraph & G);

BidirectedGraph randomCompleteBidirectedGraph(int seed, int n, int minB = 2, int maxB = 2,
                                      int minWeight = 0, int maxWeight = 1000, int minCapacity = 1,
                                      int maxCapacity = 1);

BidirectedGraph randomBidirectedGraph(int seed, int n, double density, int minB = 2, int maxB = 2,
                                      int minWeight = 0, int maxWeight = 1000, int minCapacity = 1,
                                      int maxCapacity = 1);

#endif
