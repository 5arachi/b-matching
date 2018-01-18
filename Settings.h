#ifndef SETTINGS_H
#define SETTINGS_H

#include <iostream>

using namespace std;

class Settings {
public:
	bool useHeuristic = false;
	double alpha = 0.3;
	bool useAuxiliaryIP = false;
	bool compareWithCPLEX = false;
	bool useVariableFixing = true;
	int maxIterationsAtRootBeforeBranching = 60;
	int maxIterationsBeforeBranching = 3;
	int maxCutPoolSize = 8000;
	int maxAge = 30;
	bool useEdmondsKarp = false;
};

#endif
