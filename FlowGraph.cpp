#include "FlowGraph.h"

//#define DEBUG

FlowGraph::FlowGraph(int n) : Graph<FlowEdge>(n, true) {

#ifdef DEBUG
    assert(n >= 0);
#endif

}

void FlowGraph::addVertex() {
    ++n;
    adjacencyList.push_back(vector<int> ());
}

void FlowGraph::addEdge(int head, int tail, double capacity) {

#ifdef DEBUG
    assert(validVertex(head) && validVertex(tail));
    assert(head != tail);
    assert(capacity >= 0);
#endif

    FlowEdge directEdge(head, tail, capacity, m + 1);
    FlowEdge reverseEdge(tail, head, capacity, m);
    Graph<FlowEdge>::addEdge(directEdge);
    Graph<FlowEdge>::addEdge(reverseEdge);
}

double FlowGraph::augment(int u, int t, double bottleneck, const vector<int> &level,
                          vector<char> &finished) {
    if (u == t || bottleneck < EPSILON) {
        return bottleneck;
    }
    if (finished[u]) {
        return 0;
    }
    finished[u] = true;
    for (int e : adjacencyList[u]) {
        int v = E[e].tail;
        if (level[v] > level[u]) {
            double f = augment(v, t, min(bottleneck, E[e].residualCapacity), level, finished);
            if (f > EPSILON) {
                E[e].residualCapacity -= f;
                E[E[e].mate].residualCapacity += f;
                finished[u] = false;
                return f;
            }
        }
    }
    return 0;
}

double FlowGraph::minCutDinitz(int s, int t, vector<int> &S) {
    double minCutValue = 0;

    for (int i = 0; i < m; ++i) {
        E[i].residualCapacity = E[i].capacity;
    }

    bool augmented = true;
    vector<int> level;
    vector<char> finished;
    while (augmented) {
        level.assign(n, -1);
        level[s] = 0;
        queue<int> Q;
        Q.push(s);
        int d = n, u;
        while (!Q.empty() && level[u = Q.front()] < d) {
            Q.pop();
            if (u == t) {
                d = level[u];
            }
            for (int e : adjacencyList[u]) {
                int v = E[e].tail;
                if (E[e].residualCapacity > EPSILON && level[v] == - 1) {
                    Q.push(v);
                    level[v] = level[u] + 1;
                }
            }
        }
        finished.assign(n, false);
        augmented = false;
        double f;
        while ((f = augment(s, t, DBL_MAX, level, finished)) > EPSILON) {
            minCutValue += f;
            augmented = true;
        }
    }

    queue<int> Q;
    S = vector<int> ();
    Q.push(s);
    vector<bool> visited(n, false);
    visited[s] = true;
    while (!Q.empty()) {
        int u = Q.front();
        Q.pop();
        S.push_back(u);
        for (int e : adjacencyList[u]) {
            int v = E[e].tail;
            if (E[e].residualCapacity > EPSILON && !visited[v]) {
                Q.push(v);
                visited[v] = true;
            }
        }
    }
    return minCutValue;
}

double FlowGraph::minCutEdmondsKarp(int s, int t, vector<int> &S) {
    double minCutValue = 0;

    for (int i = 0; i < m; ++i) {
        E[i].residualCapacity = E[i].capacity;
    }

    // Augment the flow while there is path from source to sink
    vector<int> parentEdge;

    while (BFS(s, t, parentEdge)) {

        double pathFlow = DBL_MAX;

        int e;
        for (int v = t; v != s; v = E[e].head) {
            e = parentEdge[v];
            pathFlow = min(pathFlow, E[e].residualCapacity);
        }

        minCutValue += pathFlow;

        for (int v = t; v != s; v = E[e].head) {
            e = parentEdge[v];
            E[e].residualCapacity -= pathFlow;
            E[E[e].mate].residualCapacity += pathFlow;
        }
    }
    S = vector<int> ();
    for (int u = 0; u < n; ++u) {
        if (parentEdge[u] != -1) {
            S.push_back(u);
        }
    }
    return minCutValue;
}

/* Returns true if there is a path from source 's' to sink 't' in
  residual graph. Also fills parent[] to store the path */

bool FlowGraph::BFS(int s, int t, vector<int> &parentEdge) {
    // Create a queue, enqueue source vertex and mark source vertex
    // as visited
    parentEdge.assign(n, -1);
    queue<int> q;
    q.push(s);
    parentEdge[s] = -2;

    while (!q.empty() && parentEdge[t] == -1) {
        int u = q.front();
        q.pop();
        for (int e : adjacencyList[u]) {
            int v = E[e].tail;
            if (parentEdge[v] == - 1 && E[e].residualCapacity > EPSILON) {
                q.push(v);
                parentEdge[v] = e;
            }
        }
    }

    // If we reached sink in BFS starting from source, then return
    // true, else false
    return (parentEdge[t] != - 1);
}

#undef DEBUG
