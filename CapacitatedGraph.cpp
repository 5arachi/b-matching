#include "CapacitatedGraph.h"

//#define DEBUG

CapacitatedGraph::CapacitatedGraph(int n) : Graph<CapacitatedEdge> (n), b(n) {

#ifdef DEBUG
	assert(n >= 0);
#endif

}

istream & operator >> (istream & in, CapacitatedGraph & G) {
	int n, m;
	in >> n >> m;
	G = CapacitatedGraph(n);

#ifdef DEBUG
	assert(m >= 0);
#endif

	for (int i = 0; i < m; ++i) {
		CapacitatedEdge e;
		in >> e;
		G.addEdge(e);
	}
	for (int i = 0; i < G.n; ++i) {
		in >> G.b[i];
	}
	return in;
}

CapacitatedGraph randomCompleteCapacitatedGraph(int seed, int n, int minB, int maxB, int minWeight,
        int maxWeight, int minCapacity, int maxCapacity) {

#ifdef DEBUG
	assert(n >= 2);
	assert(0 <= minB && minB <= maxB);
	assert(maxWeight >= minWeight);
	assert(0 <= minCapacity && minCapacity <= maxCapacity);
#endif

	srand(seed);
	CapacitatedGraph G(n);
	for (int u = 0; u < n; ++u) {
		G.b[u] = randomInteger(minB, maxB);
		for (int v = u + 1; v < n; ++v) {
			G.addEdge(CapacitatedEdge(u, v, randomInteger(minWeight, maxWeight),
			                          randomInteger(minCapacity, maxCapacity)));
		}
	}
	return G;
}

CapacitatedGraph randomCapacitatedGraph(int seed, int n, double density, int minB, int maxB, int minWeight,
                                        int maxWeight, int minCapacity, int maxCapacity) {

#ifdef DEBUG
	assert(n >= 2);
	assert(0 <= minB && minB <= maxB);
	assert(maxWeight >= minWeight);
	assert(0 <= minCapacity && minCapacity <= maxCapacity);
	assert(0 <= density && density <= 1);
#endif

	srand(seed);
	CapacitatedGraph G(n);
	for (int u = 0; u < n; ++u) {
		G.b[u] = randomInteger(minB, maxB);
	}
	int m = (ceil) (density * ((n * (n - 1)) / 2));
	for (int i = 0; i < m; ++i) {
		int u = randomInteger(0, n - 1);
		int v = randomInteger(0, n - 2);
		if (v >= u) {
			++v;
		}
		G.addEdge(CapacitatedEdge(u, v, randomInteger(minWeight, maxWeight),
		                          randomInteger(minCapacity, maxCapacity)));
	}
	return G;
}

#undef DEBUG
