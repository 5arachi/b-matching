#include "Edge.h"

//#define DEBUG

Edge::Edge() {

}

Edge::Edge(int head, int tail) : head(head), tail(tail) {

#ifdef DEBUG
	assert(head >= 0 && tail >= 0 && head != tail);
#endif

}

int Edge::otherEndpoint(int v) {

#ifdef DEBUG
	assert(v == head || v == tail);
#endif

	return v == head ? tail : head;
}

#undef DEBUG
