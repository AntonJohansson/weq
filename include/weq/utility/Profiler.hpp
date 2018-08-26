#pragma once

#include <chrono>
#include <string>
#include <unordered_map>

#define USE_PROFILING 1

#if USE_PROFILING
#define CONCAT_(a, b) a ## b
#define CONCAT(a, b) CONCAT_(a, b)
#define PROFILE(what) utility::Profiler CONCAT(__profiler__, __LINE__) (what);
#else
#define PROFILE(what) 
#endif

namespace weq::utility{

struct Profiler{
	using Clock = std::chrono::high_resolution_clock;
	struct time_entry{long long time{0}; long long min; long long max{0};};

	Profiler(const std::string& what) : what(what) {
		start = Clock::now();
	}

	~Profiler(){
		long long time = std::chrono::duration_cast<std::chrono::nanoseconds>(Clock::now() - start).count();
		if(auto e = entries.find(what); e != entries.end()){
			e->second.time += time;
			e->second.time /= 2;
			if(time < e->second.min)e->second.min = time;
			if(time > e->second.max)e->second.max = time;
		}else{
			entries[what] = {time, time, time};
		};
	}

	const std::string what{""};
	Clock::time_point start{};
	static std::unordered_map<std::string, time_entry> entries;
};

}
