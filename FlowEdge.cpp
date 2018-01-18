#include "FlowEdge.h"

//#define DEBUG

FlowEdge::FlowEdge() {

}

FlowEdge::FlowEdge(int head, int tail, double capacity, int mate) : Edge(head, tail),
	capacity(capacity), mate(mate) {

#ifdef DEBUG
	assert(capacity >= 0);
	assert(mate >= 0);
#endif

}

#undef DEBUG
