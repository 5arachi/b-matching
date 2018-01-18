#include "Constraint.h"

//#define DEBUG

Constraint::Constraint(const vector<int> &varIndices, const vector<double> &varCoefficients,
                       char sense, double RHS) : varIndices(varIndices), varCoefficients(varCoefficients),
	sense(sense), RHS(RHS), age(0) {

#ifdef DEBUG
	assert(varIndices.size() == varCoefficients.size());
	assert(sense == 'L' || sense == 'E' || sense == 'G');
#endif

}

#undef DEBUG
