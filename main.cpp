#include <cassert>
#include <fstream>
#include <iostream>
#include <getopt.h>
#include <string>

#include "BidirectedMatchingInstance.h"
#include "CapacitatedbMatchingInstance.h"
#include "Settings.h"

using namespace std;

CapacitatedbMatchingInstance readGraph(string filename) {

    // std::ifstream fin("test.txt");
    std::ifstream fin(filename);
    int m, n;
    fin >> n >> m;
    CapacitatedGraph G(n);

    for (int u = 0; u < n; ++u) {
        G.b[u] = 3;
    }

    int u, v;
    int weight;
    while (fin >> u >> v >> weight)
    {
        G.addEdge(CapacitatedEdge(u, v, weight, 2));
        // cout << u << ' ' << v << weight << endl;

    }
    fin.close();

    return CapacitatedbMatchingInstance(G);

}

CapacitatedbMatchingInstance readGraph2(string filename) {

    // std::ifstream fin("test.txt");
    std::ifstream fin(filename);
    assert(fin.is_open());
    int m, n;
    fin >> n >> m;
    CapacitatedGraph G(n);



    int u, v;
    int weight;
    for (int i = 0; i < m; ++i) {

        if (i % 1000 == 0) {
            //  cout << "read edge #" << i << endl;
        }
        fin >> u >> v >> weight;
        if (weight < 60) {
            G.addEdge(CapacitatedEdge(u, v, weight, 1));

        }// cout << u << ' ' << v << weight << endl;

    }
    int trash;
    fin >> trash;
    for (int u = 0; u < n; ++u) {
        fin >> G.b[u];
        //cout << "read b = " << G.b[u] << endl;
        if (u == 10) {
            //exit(1);
        }
    }
    fin.close();

    return CapacitatedbMatchingInstance(G);

}

void format2() {
    ifstream fin("../instances/test.txt");
    ofstream fout("../instances/colbert.txt");
    assert(fin.is_open());
    int m, n;
    fin >> n >> m;
    fout << n << " " << m << endl;
    int u, v;
    int weight;
    for (int i = 0; i < m; ++i) {
        if (i % 50000 == 0)cout << "i = " << i << endl;
        fin >> u >> v >> weight;
        if (weight < 60) {
            fout << u << " " << v << " " << weight << " " << 1 << endl;
        }
    }
    int trash;
    fin >> trash;
    for (int u = 0; u < n; ++u) {
        int b;
        fin >> b;
        fout << b << endl;
    }
    fout.close();
    fin.close();
}

/*void format() {
    ifstream fin("../instances/g10000.18780.edg");
    ofstream fout("../instances/g10000.18780.txt");
    int m, n;
    fin >> n >> m;
    fout << n << " " << m << endl;
    int u, v;
    int weight;
    int read = 0;
    while (fin >> u >> v >> weight) {
        fout << u << " " << v << " " << weight << " " << 2 << endl;
        cout << "read " << ++read << endl;
        cout << "weight = " << weight << endl;
    }
    for (int u = 0; u < n; ++u) {
        fout << 3 << endl;
    }
    fout.close();
    fin.close();
}*/

static void usage (char *f) {
    fprintf (stderr, "Usage: %s [-see below-] [prob_file]\n", f);
    fprintf (stderr, "   -b     bidirected\n");
    fprintf (stderr, "   -r     random instance\n");
    fprintf (stderr, "   -s d   random seed\n");
    fprintf (stderr, "   -n d   number of vertices\n");
    fprintf (stderr, "   -d f   density\n");
    fprintf (stderr, "   -h     enable heuristic\n");
    fprintf (stderr, "   -a f   alpha (used by heuristic)\n");
    fprintf (stderr, "   -A     enable auxiliary IP\n");
    fprintf (stderr, "   -v     disable variable fixing\n");
    fprintf (stderr, "   -R d   max. number of iterations at root before branching\n");
    fprintf (stderr, "   -B d   max. number of iterations before branching\n");
    fprintf (stderr, "   -c d   max. age for a cut in the cut pool\n");
    fprintf (stderr, "   -e     use Edmonds-Karp instead of Dinitz algorihtm\n");
}

int main(int argc, char **argv) {
    /*for (int i = 0; i < 5; ++i) {
        int seed;
        seed = rand();
        int n = 1000;
        cout << "SEED = " << seed << endl;
        Settings settings;
        settings.compareWithCPLEX = true;
        settings.useHeuristic = false;
        //BidirectedMatchingInstance I(randomCompleteBidirectedGraph(seed, n), settings);
        CapacitatedbMatchingInstance I(randomCompleteCapacitatedGraph(seed, n), settings);
        bool feasible;
        vector<double> x;
        double weight;
        I.solve(feasible, x, weight);
        cout << "********************" << endl;
    }
    return 0;*/


    Settings settings;
    bool bidirected = false;
    bool random = false;
    int seed = 0;
    int n = 100;
    double density = 1;
    bool complete = true;
    string fileName;
    int c;
    while ((c = getopt (argc, argv, "brs:n:d:ha:AvR:B:c:e")) != EOF) {
        switch (c) {
        case 'b':
            bidirected = true;
            break;
        case 'r':
            random = true;
            break;
        case 's':
            seed = atoi(optarg);
            break;
        case 'n':
            n = atoi(optarg);
            assert(n >= 2);
            break;
        case 'd':
            density = atof(optarg);
            complete = false;
            assert(0 <= density && density <= 1);
            break;
        case 'h':
            settings.useHeuristic = true;
            break;
        case 'a':
            settings.alpha = atof(optarg);
            assert(0 <= settings.alpha);
            break;
        case 'A':
            settings.useAuxiliaryIP = true;
            break;
        case 'v':
            settings.useVariableFixing = false;
            break;
        case 'R':
            settings.maxIterationsAtRootBeforeBranching = atoi(optarg);
            assert(settings.maxIterationsAtRootBeforeBranching >= 1);
            break;
        case 'B':
            settings.maxIterationsBeforeBranching = atoi(optarg);
            assert(settings.maxIterationsBeforeBranching >= 1);
            break;
        case 'c':
            settings.maxAge = atoi(optarg);
            assert(settings.maxAge >= 1);
            break;
        case 'e':
            settings.useEdmondsKarp = true;
            break;
        case '?':
        default:
            usage(argv[0]);
            return 1;
        }
    }

    if (!random) {
        if (optind != argc - 1) {
            usage(argv[0]);
            return 1;
        }
        fileName = argv[optind];
    }

    vector<double> x;
    double weight;
    bool feasible;

    if (bidirected) {
        BidirectedGraph G;
        if (random) {
            if (complete) {
                G = randomCompleteBidirectedGraph(seed, n);
            } else {
                G = randomBidirectedGraph(seed, n, density);
            }
        } else {
            ifstream fin(fileName);
            assert(fin.is_open());
            fin >> G;
            fin.close();
        }
        BidirectedMatchingInstance I(G, settings);
        I.solve(feasible, x, weight);
    } else {
        CapacitatedGraph G;
        if (random) {
            if (complete) {
                G = randomCompleteCapacitatedGraph(seed, n);
            } else {
                G = randomCapacitatedGraph(seed, n, density);
            }
        } else {
            ifstream fin(fileName);
            assert(fin.is_open());
            fin >> G;
            fin.close();
        }
        CapacitatedbMatchingInstance I(G, settings);
        I.solve(feasible, x, weight);
    }
    return 0;


}
