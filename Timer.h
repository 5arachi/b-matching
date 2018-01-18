#ifndef TIMER_H
#define TIMER_H

#include <cassert>
#include <iostream>
#include "misc.h"

using namespace std;

class Timer {
public:
	bool running;
	double totalTime;
	double t;

	Timer();
	void start();
	void pause();
};

#endif
