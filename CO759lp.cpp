#include "CO759lp.h"

//#define DEBUG

CO759lp::CO759lp(string name) {
    int rval = 0;
    cplex_env = CPXopenCPLEX(&rval);
    assert(!rval);
    cplex_lp = CPXcreateprob(cplex_env, &rval, name.c_str());
    assert(cplex_lp && !rval);
}

CO759lp::~CO759lp() {
    CPXfreeprob(cplex_env, &cplex_lp);
}

void CO759lp::newRow(char sense, double rhs) {
    assert(!CPXnewrows(cplex_env, cplex_lp, 1, &rhs, &sense, NULL, NULL));
}

void CO759lp::addColumn(double obj, const vector<int> &cmatind,
                        const vector<double> &cmatval, double lb, double ub) {
    int cmatbeg[] = {0};
    assert(!CPXaddcols(cplex_env, cplex_lp, 1, cmatind.size(), &obj,
                       cmatbeg, &cmatind[0], &cmatval[0], &lb, &ub, NULL));
}

void CO759lp::opt(bool &infeasible) {
    assert(!CPXdualopt(cplex_env, cplex_lp));
    int solstat = CPXgetstat(cplex_env, cplex_lp);

    if (solstat == CPX_STAT_INFEASIBLE) {
        infeasible = true;
    } else {
        infeasible = false;
        assert(solstat == CPX_STAT_OPTIMAL || solstat == CPX_STAT_OPTIMAL_INFEAS);
    }
}

void CO759lp::optIP(bool &infeasible) {
    assert(!CPXmipopt(cplex_env, cplex_lp));
    int solstat = CPXgetstat(cplex_env, cplex_lp);
    if (solstat == CPXMIP_INFEASIBLE) {
        infeasible = true;
    } else {
        infeasible = false;
        assert(solstat == CPXMIP_OPTIMAL  || solstat == CPXMIP_OPTIMAL_INFEAS || CPXMIP_OPTIMAL_TOL);
    }
}

int CO759lp::getStat() {
    return CPXgetstat(cplex_env, cplex_lp);
}

double CO759lp::objval() {
    double obj;
    assert(!CPXgetobjval(cplex_env, cplex_lp, &obj));
    return obj;
}

vector<double> CO759lp::x() {
    int ncols = numberOfColumns();

#ifdef DEBUG
    assert(ncols > 0);
#endif

    vector<double> x(ncols);
    assert(!CPXgetx(cplex_env, cplex_lp, &x[0], 0, ncols - 1));
    return x;
}

void CO759lp::write(string fname) {
    assert(!CPXwriteprob(cplex_env, cplex_lp, fname.c_str(), "RLP"));
}

void CO759lp::changeBounds(int variable, double lb, double ub) {
    int indices[] = {variable, variable};
    char lu[] = {'L', 'U'};
    double bd[] = {lb, ub};
    assert(!CPXchgbds(cplex_env, cplex_lp, 2, indices, lu, bd));
}

void CO759lp::changeBounds(const vector<double> &lb, const vector<double> &ub) {
    int nVar = numberOfColumns();
    vector<int> indices(2 * nVar);
    vector<char> lu(2 * nVar);
    vector<double> bd(2 * nVar);

#ifdef DEBUG
    assert(lb.size() == nVar);
    assert(ub.size() == nVar);
#endif

    for (int i = 0; i < nVar; ++i) {
        indices[2 * i] = indices[2 * i + 1] = i;
        lu[2 * i] = 'L';
        lu[2 * i + 1] = 'U';
        bd[2 * i] = lb[i];
        bd[2 * i + 1] = ub[i];
    }
    assert(!CPXchgbds(cplex_env, cplex_lp, 2 * nVar, &indices[0], &lu[0], &bd[0]));
}

void CO759lp::addConstraint(const Constraint &c) {
    int rmatbeg[] = {0};
    assert(!CPXaddrows(cplex_env, cplex_lp, 0, 1, c.varIndices.size(), &c.RHS,
                       &c.sense, rmatbeg, &c.varIndices[0], &c.varCoefficients[0], NULL, NULL));
}

void CO759lp::deleteRow(int rowId) {

#ifdef DEBUG
    assert(0 <= rowId && rowId < numberOfRows());
#endif

    assert(!CPXdelrows(cplex_env, cplex_lp, rowId, rowId));
}

void CO759lp::deleteRows(int firstRow, int lastRow) {

#ifdef DEBUG
    assert(0 <= firstRow && firstRow <= lastRow);
    assert(lastRow < numberOfRows());
#endif

    assert(!CPXdelrows(cplex_env, cplex_lp, firstRow, lastRow));
}

vector<double> CO759lp::slack(int firstRow, int lastRow) {

#ifdef DEBUG
    assert(0 <= firstRow && firstRow <= lastRow);
    assert(lastRow < numberOfRows());
#endif

    vector<double> slacks(lastRow - firstRow + 1);
    assert(!CPXgetslack(cplex_env, cplex_lp, &slacks[0], firstRow, lastRow));
    return slacks;
}

int CO759lp::numberOfRows() {
    return CPXgetnumrows(cplex_env, cplex_lp);
}

int CO759lp::numberOfColumns() {
    return CPXgetnumcols(cplex_env, cplex_lp);
}

void CO759lp::changeRHS(int rowId, double RHS) {

#ifdef DEBUG
    assert(0 <= rowId && rowId < numberOfRows());
#endif

    assert(!CPXchgrhs(cplex_env, cplex_lp, 1, &rowId, &RHS));
}

vector<double> CO759lp::reducedCosts() {
    vector<double> reducedCosts(numberOfColumns());
    if (numberOfColumns() > 0) {
        assert(!CPXgetdj(cplex_env, cplex_lp, &reducedCosts[0], 0, numberOfColumns() - 1));
    }
    return reducedCosts;
}

void CO759lp::addIntegralityConstraints() {
    vector<int> varIndices;
    int nVar = numberOfColumns();
    for (int i = 0; i < nVar; ++i) {
        varIndices.push_back(i);
    }
    vector<char> xctype(nVar, CPX_INTEGER);
    assert(!CPXchgctype(cplex_env, cplex_lp, nVar, &varIndices[0], &xctype[0]));
}

void CO759lp::removeIntegralityConstraints() {
    vector<int> varIndices;
    int nVar = numberOfColumns();
    for (int i = 0; i < nVar; ++i) {
        varIndices.push_back(i);
    }
    vector<char> xctype(nVar, CPX_CONTINUOUS);
    assert(!CPXchgctype(cplex_env, cplex_lp, nVar, &varIndices[0], &xctype[0]));
    CPXchgprobtype(cplex_env, cplex_lp, CPXPROB_LP);
}

#undef DEBUG
