#include "BidirectedEdge.h"

//#define DEBUG

BidirectedEdge::BidirectedEdge() {

}

BidirectedEdge::BidirectedEdge(int head, int signHead, int tail, int signTail, double weight,
                               int capacity) :
	CapacitatedEdge(head, tail, weight, capacity), signHead(signHead), signTail(signTail) {

#ifdef DEBUG
	assert(abs(signHead) == 1 && abs(signTail) == 1);
#endif

}

istream & operator >> (istream & in, BidirectedEdge & e) {
	in >> e.head >> e.signHead >> e.tail >> e.signTail >> e.weight >> e.capacity;

#ifdef DEBUG
	assert(e.head >= 0 && e.tail >= 0 && e.head != e.tail);
	assert(abs(e.signHead) == 1 && abs(e.signTail) == 1);
	assert(e.capacity >= 0);
#endif

	return in;
}

#undef DEBUG
