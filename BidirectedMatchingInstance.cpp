#include "BidirectedMatchingInstance.h"

//#define DEBUG

BidirectedMatchingInstance::BidirectedMatchingInstance(BidirectedGraph G, Settings settings) : 
G(G), settings(settings) {
	assert(G.m >= 1);
}

CapacitatedbMatchingInstance BidirectedMatchingInstance::reduceToCapacitatedbMatching() {
	CapacitatedGraph G_(2 * G.n);
	// each vertex i of G becomes two vertices 2 * i ("i-"), 2 * i + 1 ("i+") in G_
	vector<int> maxIncomingFlow(G.n, 0); // sum of capacities of edges incident to "i-"
	for (int i = 0; i < G.m; ++i) {
		int head, tail;
		if (G.E[i].signHead == -1) {
			head = 2 * G.E[i].head;
			maxIncomingFlow[G.E[i].head] += G.E[i].capacity;
		} else {
			head = 2 * G.E[i].head + 1;
		}
		if (G.E[i].signTail == -1) {
			tail = 2 * G.E[i].tail;
			maxIncomingFlow[G.E[i].tail] += G.E[i].capacity;
		} else {
			tail = 2 * G.E[i].tail + 1;
		}
		G_.addEdge(CapacitatedEdge(head, tail, G.E[i].weight, G.E[i].capacity));
	}

	// add edges between "i-" and "i+"
	// set new b values
	for (int i = 0; i < G.n; ++i) {
		G_.addEdge(CapacitatedEdge(2 * i, 2 * i + 1, 0, maxIncomingFlow[i]));
		G_.b[2 * i] = maxIncomingFlow[i];
		G_.b[2 * i + 1] = G_.b[2 * i] + G.b[i];
	}
	return CapacitatedbMatchingInstance(G_, settings);
}

void BidirectedMatchingInstance::solve(bool &feasible, vector<double> &x, double &weight) {
	// reduce to capacitated b-matching
	CapacitatedbMatchingInstance instance = reduceToCapacitatedbMatching();

	// solve capacitated b-matching instance
	instance.solve(feasible, x, weight);
	if (feasible) {
		// discard auxiliary edges
		x.resize(G.m);

#ifdef DEBUG
		assert(isFeasible(x));
#endif

	}
}

bool BidirectedMatchingInstance::isFeasible(const vector<double> &x) {
	if (!isIntegral(x)) {
		return false;
	}
	vector<double> B(G.n, 0);
	for (int i = 0; i < G.m; ++i) {
		if (!(-EPSILON <= x[i] && x[i] <= G.E[i].capacity + EPSILON)) {
			return false;
		}
		B[G.E[i].head] += G.E[i].signHead * x[i];
		B[G.E[i].tail] += G.E[i].signTail * x[i];
	}
	for (int v = 0; v < G.n; ++v) {
		if (fabs(G.b[v] - B[v]) > EPSILON) {
			return false;
		}
	}
	return true;
}

#undef DEBUG
