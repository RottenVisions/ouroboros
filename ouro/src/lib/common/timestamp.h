// 2017-2018 Rotten Visions, LLC. https://www.rottenvisions.com

#ifndef OURO_TIMESTAMP_H
#define OURO_TIMESTAMP_H

#include "common/platform.h"

namespace Ouroboros{

// Indicates if the timestamp can be calculated
// by calling RDTSC (timestamp counter). The advantage of using this is that it can quickly and accurately return the actual clock tick
//. The downside is that this does not use SpeedStep technology to change their clock speed CPU.
#ifdef unix
//#define OURO_USE_RDTSC
#else // unix
//#define OURO_USE_RDTSC
#endif // unix

enum OUROTimingMethod
{
	RDTSC_TIMING_METHOD, // The number of clock cycles passed since the CPU was powered on, achieving nanosecond timing accuracy
	GET_TIME_OF_DAY_TIMING_METHOD,
	GET_TIME_TIMING_METHOD,
	NO_TIMING_METHOD,
};

extern OUROTimingMethod g_timingMethod;

const char* getTimingMethodName();

#ifdef unix

inline uint64 timestamp_rdtsc()
{
	uint32 rethi, retlo;
	__asm__ __volatile__ (
		"rdtsc":
		"=d"    (rethi),
		"=a"    (retlo)
						  );
	return uint64(rethi) << 32 | retlo;
}

// Use gettimeofday. The test is roughly -600 times faster than RDTSC.
// In addition, there is a problem 
// In the 2.4 kernel, it is possible to call gettimeofday two times in a row
// Returning a result is backwards.
#include <sys/time.h>

inline uint64 timestamp_gettimeofday()
{
	timeval tv;
	gettimeofday( &tv, NULL );
	return 1000000ULL * uint64( tv.tv_sec ) + uint64( tv.tv_usec );
}

#include <time.h>
#include <asm/unistd.h>

inline uint64 timestamp_gettime()
{
	timespec tv;
	assert(syscall( __NR_clock_gettime, CLOCK_MONOTONIC, &tv ) == 0);
	return 1000000000ULL * tv.tv_sec + tv.tv_nsec;
}

inline uint64 timestamp()
{
#ifdef OURO_USE_RDTSC
	return timestamp_rdtsc();
#else // OURO_USE_RDTSC
	if (g_timingMethod == RDTSC_TIMING_METHOD)
		return timestamp_rdtsc();
	else if (g_timingMethod == GET_TIME_OF_DAY_TIMING_METHOD)
		return timestamp_gettimeofday();
	else // GET_TIME_TIMING_METHOD
		return timestamp_gettime();

#endif // OURO_USE_RDTSC
}

#elif defined(_WIN32)

#ifdef OURO_USE_RDTSC
#pragma warning (push)
#pragma warning (disable: 4035)
inline uint64 timestamp()
{
	__asm rdtsc
}
#pragma warning (pop)
#else // OURO_USE_RDTSC

#include <windows.h>

inline uint64 timestamp()
{
	LARGE_INTEGER counter;
	QueryPerformanceCounter( &counter );
	return counter.QuadPart;
}

#endif // OURO_USE_RDTSC

#else
	#error Unsupported platform!
#endif

uint64 stampsPerSecond();
double stampsPerSecondD();

uint64 stampsPerSecond_rdtsc();
double stampsPerSecondD_rdtsc();

uint64 stampsPerSecond_gettimeofday();
double stampsPerSecondD_gettimeofday();

inline double stampsToSeconds(uint64 stamps)
{
	return double(stamps) / stampsPerSecondD();
}

// -----------------------------------------------------------------------------
class TimeStamp
{
public:
	TimeStamp(uint64 stamps = 0) :
		stamp_( stamps )
	{
	}

	operator uint64& ()
	{
		return stamp_;
	}

	operator uint64() const
	{
		return stamp_;
	}

	inline uint64 stamp() { return stamp_; }

	inline double inSeconds() const;
	inline void setInSeconds(double seconds);

	inline static double toSeconds(uint64 stamps);
	inline static TimeStamp fromSeconds( double seconds );

	uint64	stamp_;
};


inline double TimeStamp::toSeconds(uint64 stamps)
{
	return double(stamps) / stampsPerSecondD();
}

inline TimeStamp TimeStamp::fromSeconds(double seconds)
{
	return uint64(seconds * stampsPerSecondD());
}

inline double TimeStamp::inSeconds() const
{
	return toSeconds( stamp_ );
}

inline void TimeStamp::setInSeconds(double seconds)
{
	stamp_ = fromSeconds(seconds);
}



}

#endif // OURO_TIMESTAMP_H
