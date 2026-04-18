#include "timer.h"

#include <iostream>

ruya::Timer::Timer(bool start)
	: mTStart(steady_clock::now()), mTStop(steady_clock::now()), mTCreation(steady_clock::now())
{
	if (start)
		this->start();
}

void ruya::Timer::start()
{
	mTStart = steady_clock::now();
}

void ruya::Timer::stop()
{
	mTStop = steady_clock::now();
}



/*
* Returns the elsapsed time between start() and stop() calls.
*	-	If the stop call has not yet been called after start() then
*		it returns the elapsed time since start() until now.
* //TODO: in what unit?
*/
double ruya::Timer::elapsed_time_s() const
{
	return (double)(elapsed_time_ns()) / 1000'000'000;
}

double ruya::Timer::elapsed_time_ms() const
{
	return (double)(elapsed_time_ns()) / 1000'000;
}

double ruya::Timer::elapsed_time_us() const
{
	return (double)(elapsed_time_ns())/1000;
}

long long ruya::Timer::elapsed_time_ns() const
{
	if (mTStart > mTStop)
		return (steady_clock::now() - mTStart).count();
	else
		return (mTStop - mTStart).count();
}

double ruya::Timer::time_since_creation_s() const
{
	return (double)(time_since_creation_ns()) / 1000'000'000;
}

long long ruya::Timer::time_since_creation_ns() const
{
	return (steady_clock::now() - mTCreation).count();
}
