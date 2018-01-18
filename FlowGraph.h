#ifndef FLOW_GRAPH_H
#define FLOW_GRAPH_H

#include <cfloat>
#include <iostream>
#include <queue>

#include "FlowEdge.h"
#include "Graph.h"
#include "misc.h"

using namespace std;

class FlowGraph: public Graph<FlowEdge> {
public:
	FlowGraph(int n = 0);

	void addVertex();
	void addEdge(int head, int tail, double capacity);
	double augment(int u, int t, double bottleneck, const vector<int> &level,
	               vector<char> &finished);
	double minCutDinitz(int s, int t, vector<int> &S);
	double minCutEdmondsKarp(int s, int t, vector<int> &S);
	bool BFS(int s, int t, vector<int> &parentEdge);
};

#endif
