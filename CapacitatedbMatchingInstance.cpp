#include "CapacitatedbMatchingInstance.h"

//#define DEBUG

CapacitatedbMatchingInstance::CapacitatedbMatchingInstance(CapacitatedGraph G, Settings settings) : G(G),
    lp("bMatchingLP"), auxIP("auxiliaryIP"), settings(settings) {
        assert(G.m >= 1);
}

bool CapacitatedbMatchingInstance::validSettings() {
    if (settings.useHeuristic && settings.alpha < 0) {
        return false;
    }
    if (settings.maxIterationsAtRootBeforeBranching <= 0 || settings.maxIterationsBeforeBranching <= 0) {
        return false;
    }
    if (settings.maxCutPoolSize <= 0) {
        return false;
    }
    if (settings.maxAge <= 0) {
        return false;
    }
    return true;
}

void CapacitatedbMatchingInstance::initializeLP() {
    // Initialize LP with degree constraints and edge capacities.
    for (int v = 0; v < G.n; ++v) {
        lp.newRow('E', G.b[v]);
    }

    // Create a column for each edge.
    for (int e = 0; e < G.m; ++e) {
        vector<int> cmatind = {G.E[e].head, G.E[e].tail};
        vector<double> cmatval = {1, 1};
        lp.addColumn(G.E[e].weight, cmatind, cmatval, 0, G.E[e].capacity);
    }

    vector<int> varIndices;
    vector<double> varCoefficients;
    for (int i = 0; i < G.m; ++i) {
        varIndices.push_back(i);
        varCoefficients.push_back(G.E[i].weight);
    }
    lp.addConstraint(Constraint(varIndices, varCoefficients, 'G', -DBL_MAX));
}

void CapacitatedbMatchingInstance::initializeAuxiliaryIP() {
    auxIP.newRow('G', -DBL_MAX);
    auxIP.newRow('E', accumulate(G.b.begin(), G.b.end(), 0));
    for (int e = 0; e < G.m; e++) {
        vector<int> cmatind = {0, 1};
        vector<double> cmatval = {G.E[e].weight, 2};
        auxIP.addColumn(G.E[e].weight, cmatind, cmatval, 0, G.E[e].capacity);
    }
    auxIP.addIntegralityConstraints();
}

void CapacitatedbMatchingInstance::improveLB() {
    if (integralWeights && fractionalPart(objectiveLB) > EPSILON) {
        objectiveLB = ceil(objectiveLB);
    }
    if (settings.useAuxiliaryIP) {
        timeAuxiliaryIP.start();
        auxIP.changeBounds(varLB, varUB);
        auxIP.changeRHS(0, objectiveLB);
        bool auxInfeasible;
        auxIP.optIP(auxInfeasible);
        if (auxInfeasible) {
            objectiveLB = DBL_MAX;
        } else if (auxIP.getStat() == CPXMIP_OPTIMAL) {
            objectiveLB = auxIP.objval();
        }
        timeAuxiliaryIP.pause();
    }
}

void CapacitatedbMatchingInstance::solve(bool &feasible, vector<double> &x_opt, double &weight) {
    assert(validSettings());
    cout.precision(10);
    std::cout << std::fixed;
    cout << "solving Capacitated b-Matching Instance" << endl;

    integralWeights = true;
    for (int e = 0; e < G.m && integralWeights; ++e) {
        integralWeights = isIntegral(G.E[e].weight);
    }

    initializeLP();

    if (settings.useAuxiliaryIP) {
        initializeAuxiliaryIP();
    }

    if (settings.compareWithCPLEX) {
        lp.addIntegralityConstraints();
        bool infeasible;
        cout << "CPLEX starting" << endl;
        timeCPLEX.start();
        lp.optIP(infeasible);
        timeCPLEX.pause();
        cout << "CPLEX ended" << endl;
        feasibleCPLEX = !infeasible;
        if (feasibleCPLEX) {
            optCPLEX = lp.objval();
        }
        lp.removeIntegralityConstraints();
    }

    objectiveLB = -DBL_MAX;
    bestbMatchingValue = DBL_MAX;
    varLB = vector<double> (G.m, 0);
    varUB = vector<double> (G.m);
    for (int i = 0; i < G.m; ++i) {
        varUB[i] = G.E[i].capacity;
    }
    cout << "starting branch-and-cut" << endl;
    timeBranchAndCut.start();
    solve(0);
    timeBranchAndCut.pause();
    cout << "===============================================" << endl;
    if (bestbMatchingValue < DBL_MAX) {
        feasible = true;
        x_opt = bestbMatching;
        weight = bestbMatchingValue;
        cout << "=========== FINISHED: " << weight << endl;
#ifdef DEBUG
        assert(isFeasible(x_opt));
#endif
    } else {
        feasible = false;
        cout << "=========== FINISHED: infeasible" << endl;
    }
    cout << "Total time: " << timeBranchAndCut.totalTime << endl;
    cout << "timeLP: " << timeLP.totalTime << endl;
    cout << "timeAuxiliaryIP: " << timeAuxiliaryIP.totalTime << endl;
    cout << "timeHeuristicSeparation: " << timeHeuristicSeparation.totalTime << endl;
    cout << "timeExactSeparation: " << timeExactSeparation.totalTime << endl;
    cout << "timeFlow: " << timeFlow.totalTime << endl;
    cout << "timeCutPool: " << timeCutPool.totalTime << endl;
    cout << "===============================================" << endl;

    if (settings.compareWithCPLEX) {
        assert(feasible == feasibleCPLEX);
        if (feasible) {
            cout << "optCPLEX = " << optCPLEX << endl;
            assert(fabs(weight - optCPLEX) <= 0.1);
        }
        cout << "RUNNING TIME OF CPLEX: " << timeCPLEX.totalTime << endl;
    }
}

void CapacitatedbMatchingInstance::variableFixing(const vector<double> &x) {
    if (bestbMatchingValue < DBL_MAX) {
        double UB;
        if (integralWeights) {
            UB = round(bestbMatchingValue) - 1 + EPSILON;
        } else {
            UB = bestbMatchingValue - EPSILON;
        }
        double objectiveValue = lp.objval();
        vector<double> reducedCosts = lp.reducedCosts();
        for (int i = 0; i < G.m; ++i) {
            // UB >= value >= objectiveValue + reducedCost[i] * delta(x[i])
            // x[i] is not fixed, has non-zero reduced cost
            if (varUB[i] >= varLB[i] + 0.5 && fabs(reducedCosts[i]) >= EPSILON) {
                if (fabs(x[i] - varLB[i]) <= EPSILON) {
                    // variable is at LB

#ifdef DEBUG
                    assert(reducedCosts[i] >= 0);
#endif

                    double tmp = varLB[i] + floor((UB - objectiveValue) / reducedCosts[i]);
                    varUB[i] = min(varUB[i], tmp);
                } else if (fabs(x[i] - varUB[i]) <= EPSILON) {
                    // variable is at UB

#ifdef DEBUG
                    assert(reducedCosts[i] <= 0);
#endif

                    double tmp = varUB[i] +
                                 ceil((objectiveValue - UB) / (- reducedCosts[i]));
                    varLB[i] = max(varLB[i], tmp);
                }
#ifdef DEBUG
                else {
                    assert(false);
                }
#endif
            }
        }
    }
}

void CapacitatedbMatchingInstance::resolveLP(bool &infeasible, vector<double> &x) {
    cout << "solving LP" << endl;
    improveLB();
    lp.changeBounds(varLB, varUB);
    lp.changeRHS(G.n, objectiveLB);
    timeLP.start();
    lp.opt(infeasible);
    timeLP.pause();
    if (!infeasible) {
        objectiveLB = max(objectiveLB, lp.objval());
        x = lp.x();
        if (isIntegral(x)) {
            if (objectiveLB < bestbMatchingValue) {
                bestbMatchingValue = objectiveLB;
                bestbMatching = x;
            }
        } else if (settings.useVariableFixing) {
            variableFixing(x);
        }
    }
}

bool CapacitatedbMatchingInstance::prune() {
    if (!integralWeights) {
        return objectiveLB >= bestbMatchingValue - EPSILON;
    } else {
        return ceil(objectiveLB - EPSILON) >= round(bestbMatchingValue);
    }
}

void CapacitatedbMatchingInstance::solve(int level) {
    int iteration = 0;
    int iterationsWithoutImprovement = 0;
    double oldObjectiveLB = objectiveLB;

    bool infeasible;
    vector<double> x;
    resolveLP(infeasible, x);
    bool branch = false;
    while (!infeasible && !prune() && !branch) {
        ++iteration;
        cout << "*********************" << endl;
        cout << "ITERATION " << iteration << " (level " << level << ")" << endl;
        cout << "objectiveLB = " << objectiveLB << endl;
        cout << "bestbMatchingValue = ";
        if (bestbMatchingValue < DBL_MAX) {
            cout << bestbMatchingValue << endl;
        } else {
            cout << "INF" << endl;
        }

        if (settings.compareWithCPLEX) {
            cout << "(optCPLEX = ";
            if (!feasibleCPLEX) {
                cout << "INFEASIBLE)" << endl;
            } else {
                cout << optCPLEX << ")" << endl;
            }
        }

        if (objectiveLB > oldObjectiveLB + EPSILON) {
            iterationsWithoutImprovement = 0;
        } else {
            ++iterationsWithoutImprovement;
            cout << "iterationsWithoutImprovement: " << iterationsWithoutImprovement << endl;
            if ((level == 0 && iterationsWithoutImprovement ==
                    settings.maxIterationsAtRootBeforeBranching) ||
                    (level > 0 && iterationsWithoutImprovement ==
                     settings.maxIterationsBeforeBranching)) {
                branch = true;
            }
        }
        cout << "*********************" << endl;
        if (!branch) {
            // add cutting planes
            vector<Constraint> newCuts = findViolatedInequalities(x);
            if (newCuts.empty()) {
                cout << "did not find violated inequality" << endl;
                branch = true;
            } else {
                cout << "found " << newCuts.size() << " new cuts " << endl;
                cout << "BEFORE CLEANING CUTPOOL: " << cutPool.size() << " constraints" << endl;
                timeCutPool.start();
                cleanCutPool();
                timeCutPool.pause();
                cout << "AFTER CLEANING CUTPOOL: " << cutPool.size() << " constraints" << endl;
                for (int i = 0; i < newCuts.size(); ++i) {
                    lp.addConstraint(newCuts[i]);
                    cutPool.push_back(newCuts[i]);
                }
                oldObjectiveLB = objectiveLB;
                resolveLP(infeasible, x);
            }
        }
    }
    if (branch) {
        cout << "**************************************************************** BRANCHING " << endl;

        // branch
        int bestEdgeToBranch = - 1;
        double bestDistanceToHalf = 0;
        for (int i = 0; i < x.size(); ++i) {
            double distanceToHalf = fabs(0.5 - fractionalPart(x[i]));
            if (bestEdgeToBranch == -1 || distanceToHalf < bestDistanceToHalf) {
                bestEdgeToBranch = i;
                bestDistanceToHalf = distanceToHalf;
            }
        }

        vector<double> rememberVarLB, rememberVarUB;
        double rememberObjectiveLB;

        rememberObjectiveLB = objectiveLB;
        rememberVarLB = varLB;
        rememberVarUB = varUB;
        varLB[bestEdgeToBranch] = ceil(x[bestEdgeToBranch]);

        solve(level + 1);
        objectiveLB = rememberObjectiveLB;
        if (!prune()) {
            varLB = rememberVarLB;
            varUB = rememberVarUB;
            varUB[bestEdgeToBranch] = floor(x[bestEdgeToBranch]);

            solve(level + 1);
        }
    }
}

vector<Constraint> CapacitatedbMatchingInstance::findViolatedInequalities(const vector<double> &x) {
    if (settings.useHeuristic) {
        timeHeuristicSeparation.start();
        vector<Constraint> newCuts = heuristicSeparation(x);
        timeHeuristicSeparation.pause();
        if (!newCuts.empty()) {
            return newCuts;
        }
    }
    timeExactSeparation.start();
    vector<Constraint> newCuts = exactSeparation(x);
    timeExactSeparation.pause();
    return newCuts;
}

vector<Constraint> CapacitatedbMatchingInstance::exactSeparation(const vector<double> &x) {
    cout << "--------calling exact separation algorithm--------" << endl;
    vector<int> newName(G.n, -1);
    vector<char> finished(G.n, false);
    vector<Constraint> newCuts;
    for (int u = 0; u < G.n; ++u) {
        if (newName[u] == -1) {
            vector<int> oldName;
            newName[u] = 0;
            oldName.push_back(u);
            queue<int> Q;
            Q.push(u);
            FlowGraph FG(1);
            while (!Q.empty()) {
                int v = Q.front();
                Q.pop();
                for (int e : G.adjacencyList[v]) {
                    double capacity = min(x[e], G.E[e].capacity - x[e]);
                    if (capacity > EPSILON) {
                        int w = G.E[e].otherEndpoint(v);
                        if (newName[w] == -1) {
                            newName[w] = FG.n;
                            FG.addVertex();
                            oldName.push_back(w);
                            Q.push(w);
                        }
                        if (!finished[w]) {
                            FG.addEdge(newName[v], newName[w], capacity);
                        }
                    }
                }
                finished[v] = true;
            }
            findViolatedInequalitiesInsideComponent(x, FG, oldName, newCuts);
        }
    }
    return newCuts;
}

void CapacitatedbMatchingInstance::findViolatedInequalitiesInsideComponent(const vector<double> &x,
        FlowGraph & FG, const vector<int> & oldName, vector<Constraint> &newCuts) {

#ifdef DEBUG
    assert(oldName.size() == FG.n);
#endif

    // start by checking the entire component
    findViolatedInequalityForHandle(x, oldName, newCuts);

    // Gomory-Hu
    vector<int> p(FG.n, 0);

    for (int s = 1; s < FG.n; ++s) {
        int t = p[s];
        vector<int> S_;

        timeFlow.start();
        double minCutValue;
        if (settings.useEdmondsKarp) {
            minCutValue = FG.minCutEdmondsKarp(s, t, S_);
        } else {
            minCutValue = FG.minCutDinitz(s, t, S_);
        }
        timeFlow.pause();

        sort(S_.begin(), S_.end());

        for (int i = 0; i < FG.n; ++i) {
            if (i != s && binary_search(S_.begin(), S_.end(), i) && p[i] == t) {
                p[i] = s;
            }
        }

        if (binary_search(S_.begin(), S_.end(), p[t])) {
            p[s] = p[t];
            p[t] = s;
        }

        if (minCutValue < 1 - EPSILON) {
            vector<int> S;
            for (int v : S_) {
                S.push_back(oldName[v]);
            }
            findViolatedInequalityForHandle(x, S, newCuts);
        }
    }
}

void CapacitatedbMatchingInstance::findViolatedInequalityForHandle(const vector<double> &x, vector<int> S,
        vector<Constraint> &newCuts) {
    sort(S.begin(), S.end());
    int T = 0;
    int bestSwapEdgePosition = - 1;
    double bestSwapValue;
    double cutValue = 0;
    vector<int> varIndices;
    vector<double> varCoefficients;
    double RHS = 1;
    for (int u : S) {
        T = (T + G.b[u]) % 2;
        for (int e : G.adjacencyList[u]) {
            if (!binary_search(S.begin(), S.end(), G.E[e].otherEndpoint(u))) {
                varIndices.push_back(e);
                if (G.E[e].capacity - x[e] < x[e]) {
                    T = (T + G.E[e].capacity) % 2;
                    varCoefficients.push_back(-1);
                    cutValue += G.E[e].capacity - x[e];
                    RHS -= G.E[e].capacity;
                } else {
                    varCoefficients.push_back(1);
                    cutValue += x[e];
                }

                if (G.E[e].capacity % 2 == 1) {
                    double swapValue = fabs(2 * x[e] - G.E[e].capacity);
                    if (bestSwapEdgePosition == -1 || swapValue < bestSwapValue) {
                        bestSwapEdgePosition = varIndices.size() - 1;
                        bestSwapValue = swapValue;
                    }
                }
            }
        }
    }

    if ((T % 2 == 1 && cutValue < 1 - EPSILON) || (T % 2 == 0 && bestSwapEdgePosition != - 1
            && cutValue + bestSwapValue < 1 - EPSILON)) {

        if (T % 2 == 0) {
            // fix parity
            cutValue += bestSwapValue;
            int bestSwapEdge = varIndices[bestSwapEdgePosition];
            if (G.E[bestSwapEdge].capacity - x[bestSwapEdge] < x[bestSwapEdge]) {
                // remove that edge from F
                varCoefficients[bestSwapEdgePosition] = 1;
                RHS += G.E[bestSwapEdge].capacity;
            } else {
                // add that edge to F
                varCoefficients[bestSwapEdgePosition] = - 1;
                RHS -= G.E[bestSwapEdge].capacity;
            }
        }
        newCuts.push_back(Constraint(varIndices, varCoefficients, 'G', RHS));

#ifdef DEBUG
        double LHS = 0;
        double RHS_ = 1;
        for (int i = 0; i < varIndices.size(); ++i) {
            int e = varIndices[i];
            if (varCoefficients[i] < 0) {
                LHS += G.E[e].capacity - x[e];
                RHS_ -= G.E[e].capacity;
            } else {
                LHS += x[e];
            }
        }
        assert(fabs(LHS - cutValue) <= EPSILON);
        assert(fabs(RHS - RHS_) <= EPSILON);
#endif
    }
}

vector<Constraint> CapacitatedbMatchingInstance::heuristicSeparation(const vector<double> &x) {
    cout << "------------calling heuristic algorithm------------" << endl;
    vector<Constraint> newCuts;
    vector<char> visited (G.n, false);
    for (int s = 0; s < G.n; s++) {
        if (!visited[s]) {
            vector<int> vertices;
            queue<int> q;
            q.push(s);
            visited[s] = true;
            while (!q.empty()) {
                int u = q.front();
                q.pop();
                vertices.push_back(u);
                for (int e : G.adjacencyList[u]) {
                    int v = G.E[e].otherEndpoint(u);
                    if (!visited[v] && min(x[e], G.E[e].capacity - x[e]) >= settings.alpha) {
                        q.push(v);
                        visited[v] = true;
                    }
                }
            }
            findViolatedInequalityForHandle(x, vertices, newCuts);
        }
    }
    return newCuts;
}

void CapacitatedbMatchingInstance::cleanCutPool() {
    int oldCutPoolSize = lp.numberOfRows() - G.n - 1;

#ifdef DEBUG
    assert(lp.numberOfRows() == G.n + 1 + cutPool.size());
#endif

    if (cutPool.empty()) {
        return;
    }
    vector<int> cnt(settings.maxAge + 1, 0); // cnt[i] = # of constraints with age i
    vector<double> slacks = lp.slack(G.n + 1, G.n + 1 + oldCutPoolSize - 1);
    int i = 0;
    int j = 0;
    for (list<Constraint>::iterator it = cutPool.begin(); it != cutPool.end(); ++j, ++it) {
#ifdef DEBUG
        assert(slacks[j] <= EPSILON);
#endif

        if (slacks[j] < - EPSILON) {
            ++it->age;
        } else {
            it->age = 0;
        }
        ++cnt[it->age];

#ifdef DEBUG
        assert(it->age <= settings.maxAge);
#endif

    }
    int threshold = settings.maxAge;
    int toDelete = cnt[settings.maxAge];
    int needToDelete = max(oldCutPoolSize - settings.maxCutPoolSize, 0);
    while (toDelete < needToDelete) {
        toDelete += cnt[--threshold];

#ifdef DEBUG
        assert(threshold >= 0);
#endif

    }

    // delete all constraints with age >= threshold
    i = 0;
    for (list<Constraint>::iterator it = cutPool.begin(); it != cutPool.end(); ) {
        if (it->age >= threshold) {
            it = cutPool.erase(it);
            lp.deleteRow(G.n + 1 + i);
        } else {
            ++it, ++i;
        }
    }

#ifdef DEBUG
    int newSize = cutPool.size();
    assert(newSize == oldCutPoolSize - toDelete);
    assert(lp.numberOfRows() == G.n + 1 + newSize);
#endif

}

bool CapacitatedbMatchingInstance::isFeasible(const vector<double> &x) {
    if (!isIntegral(x)) {
        return false;
    }
    vector<double> B(G.n, 0);
    for (int i = 0; i < G.m; ++i) {
        if (!(-EPSILON <= x[i] && x[i] <= G.E[i].capacity + EPSILON)) {
            return false;
        }
        B[G.E[i].head] += x[i];
        B[G.E[i].tail] += x[i];
    }
    for (int v = 0; v < G.n; ++v) {
        if (fabs(G.b[v] - B[v]) > EPSILON) {
            return false;
        }
    }
    return true;
}

#undef DEBUG
