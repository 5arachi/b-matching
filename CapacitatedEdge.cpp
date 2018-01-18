#include "CapacitatedEdge.h"

//#define DEBUG

CapacitatedEdge::CapacitatedEdge() {

}

CapacitatedEdge::CapacitatedEdge(int head, int tail, double weight, int capacity) :
	Edge(head, tail), weight(weight), capacity(capacity) {

#ifdef DEBUG
	assert(capacity >= 0);
#endif

}

istream & operator >> (istream & in, CapacitatedEdge & e) {
	in >> e.head >> e.tail >> e.weight >> e.capacity;

#ifdef DEBUG
	assert(e.head >= 0 && e.tail >= 0 && e.head != e.tail);
	assert(e.capacity >= 0);
#endif

	return in;
}

#undef DEBUG
