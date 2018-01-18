#include "Timer.h"

Timer::Timer() : totalTime(0), running(false) {

}

void Timer::start() {
	assert(!running);
	running = true;
	t = CO759_zeit();
}

void Timer::pause() {
	assert(running);
	running = false;
	totalTime += CO759_zeit() - t;
}
