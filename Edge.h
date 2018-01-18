#ifndef EDGE_H
#define EDGE_H

#include <cassert>
#include <iostream>

using namespace std;

class Edge {
public:
	int head, tail;

	Edge();
	Edge(int head, int tail);
	int otherEndpoint(int v);
};

#endif
