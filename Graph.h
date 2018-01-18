#ifndef GRAPH_H
#define GRAPH_H

//#define DEBUG

#include <cassert>
#include <iostream>
#include <vector>

#include "Edge.h"

using namespace std;

template <class EdgeType>
class Graph {
public:
	int n, m;
	bool directed;

	vector<vector<int> > adjacencyList;
	vector<EdgeType> E;

	Graph(int n = 0, bool directed = false) : n(n), m(0), adjacencyList(n), directed(directed) {

#ifdef DEBUG
		assert(n >= 0);
#endif

	}

	bool validVertex(int u) {
		return 0 <= u && u < n;
	}

	void addEdge(const EdgeType &e) {

#ifdef DEBUG
		assert(e.head != e.tail);
		assert(validVertex(e.head) && validVertex(e.tail));
#endif

		E.push_back(e);
		adjacencyList[e.head].push_back(m);
		if (!directed) {
			adjacencyList[e.tail].push_back(m);
		}
		++m;
	}
};

#undef DEBUG

#endif
