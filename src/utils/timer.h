#ifndef TIMER_H
#define TIMER_H

#include <chrono>

using namespace std::chrono;

namespace ruya
{
	
	
	
	class Timer
	{
	public:
		Timer(bool start = false);
		void start();
		void stop();
		double elapsed_time_s() const;
		double elapsed_time_ms() const;
		double elapsed_time_us() const;
		long long elapsed_time_ns() const;
		
		double time_since_creation_s() const;
		long long time_since_creation_ns() const;

	private:
		steady_clock::time_point mTCreation; // start time
		steady_clock::time_point mTStart; // start time
		steady_clock::time_point mTStop; // stop time


	};



}



#endif // !TIMER_H



