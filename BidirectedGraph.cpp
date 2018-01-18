#include "BidirectedGraph.h"

//#define DEBUG

BidirectedGraph::BidirectedGraph(int n) : Graph<BidirectedEdge>(n), b(n) {

#ifdef DEBUG
	assert(n >= 0);
#endif

}

istream & operator >> (istream & in, BidirectedGraph & G) {
	int n, m;
	in >> n >> m;
	G = BidirectedGraph(n);

#ifdef DEBUG
	assert(m >= 0);
#endif

	for (int i = 0; i < m; ++i) {
		BidirectedEdge e;
		in >> e;
		G.addEdge(e);
	}
	for (int i = 0; i < G.n; ++i) {
		in >> G.b[i];
	}
	return in;
}

BidirectedGraph randomCompleteBidirectedGraph(int seed, int n, int minB, int maxB, int minWeight,
        int maxWeight, int minCapacity, int maxCapacity) {

#ifdef DEBUG
	assert(n >= 2);
	assert(maxB >= minB);
	assert(maxWeight >= minWeight);
	assert(0 <= minCapacity && minCapacity <= maxCapacity);
#endif

	srand(seed);
	BidirectedGraph G(n);
	for (int u = 0; u < n; ++u) {
		G.b[u] = randomInteger(minB, maxB);
		for (int v = u + 1; v < n; ++v) {
			int signU = rand() % 2 ? -1 : 1;
			int signV = rand() % 2 ? -1 : 1;
			G.addEdge(BidirectedEdge(u, signU, v, signV, randomInteger(minWeight, maxWeight),
			                         randomInteger(minCapacity, maxCapacity)));
		}
	}
	return G;
}

BidirectedGraph randomBidirectedGraph(int seed, int n, double density, int minB, int maxB, int minWeight,
                                      int maxWeight, int minCapacity, int maxCapacity) {

#ifdef DEBUG
	assert(n >= 2);
	assert(maxB >= minB);
	assert(maxWeight >= minWeight);
	assert(0 <= minCapacity && minCapacity <= maxCapacity);
	assert(0 <= density && density <= 1);
#endif

	srand(seed);
	BidirectedGraph G(n);
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
		int signU = rand() % 2 ? -1 : 1;
		int signV = rand() % 2 ? -1 : 1;
		G.addEdge(BidirectedEdge(u, signU, v, signV, randomInteger(minWeight, maxWeight),
		                         randomInteger(minCapacity, maxCapacity)));
	}
	return G;
}

#undef DEBUG
