#ifndef TIMER_H
#define TIMER_H
#include "common.h"
#include "etypes.h"
inline double ms_to_s(double ms)
{
	return ms / 1000.0;
}
inline double s_to_ms(double s)
{
	return s * 1000.0;
}
inline double us_to_s(double us)
{
	return us / 1000000.0;
}
inline double s_to_us(double s)
{
	return s * 1000000.0;
}
/**
#ifdef  __GNUC__
__inline__ uint64 perf_counter(void)
{
	euint32 lo, hi;
	// take time stamp counter, rdtscp does serialize by itself, and is much cheaper than using CPUID
	__asm__ __volatile__ (
		"rdtscp" : "=a"(lo), "=d"(hi)
		);
	return ((uint64)lo) | (((uint64)hi) << 32);
}
**/
#if defined(_MSC_VER) || defined(__MINGW32__)

inline euint64 perf_counter(void)
{
	euint64 ret;
	QueryPerformanceCounter((LARGE_INTEGER*)&ret);
	return ret;
}
inline double pref_frequency(void)
{
	LARGE_INTEGER temp;
	QueryPerformanceFrequency((LARGE_INTEGER*)&temp);
	return ((double)temp.QuadPart) / 1000000.0;
}
inline double cale_elapsed_time(euint64 start, euint64 end, double freq)
{
	return ((double)end - (double)start) / freq;
}
struct TimeCheckpoint
{
	euint64 timeStamp;
	double freq;
	TimeCheckpoint()
		: timeStamp(0)
		, freq(0.0)
	{}
	static inline TimeCheckpoint Tick() {
        TimeCheckpoint ret;
		ret.timeStamp = perf_counter();
		ret.freq = pref_frequency();
		return ret;
	}
	static inline double CaleElapsedTime(TimeCheckpoint& prevCheckpoint, TimeCheckpoint& curtCheckpoint) {
		double avFreq = (curtCheckpoint.freq + prevCheckpoint.freq) / 2.0;
        return cale_elapsed_time(prevCheckpoint.timeStamp, curtCheckpoint.timeStamp, avFreq);
	}
};
#elif defined (__APPLE__)
#include <CoreServices/CoreServices.h>
#include <mach/mach.h>
#include <mach/mach_time.h>
#include <unistd.h>
///#include <kern/kern_types.h>
///#include <kern/clock.h>

struct TimeCheckpoint
{
	uint64 timeStamp;
	
	TimeCheckpoint()
    : timeStamp(0)
	{}
	static inline TimeCheckpoint Tick() {
        TimeCheckpoint ret;
		ret.timeStamp = mach_absolute_time();
		return ret;
	}
	static inline double CaleElapsedTime(TimeCheckpoint& prevCheckpoint, TimeCheckpoint& curtCheckpoint) {
		uint64        elapsed;
        uint64        elapsedNano;
        static mach_timebase_info_data_t    sTimebaseInfo;
        elapsed = curtCheckpoint.timeStamp - prevCheckpoint.timeStamp;
        //Nanoseconds   elapsedNano;
        //elapsedNano = AbsoluteToNanoseconds( *(AbsoluteTime *) &elapsed );
        /**
        uint64        elapsedNano;
        absolutetime_to_nanoseconds(elapsed, &elapsedNano);
         **/
        // Convert to nanoseconds.
        
        // If this is the first time we've run, get the timebase.
        // We can use denom == 0 to indicate that sTimebaseInfo is
        // uninitialised because it makes no sense to have a zero
        // denominator is a fraction.
        
        if ( sTimebaseInfo.denom == 0 ) {
            (void) mach_timebase_info(&sTimebaseInfo);
        }
        
        // Do the maths. We hope that the multiplication doesn't
        // overflow; the price you pay for working in fixed point.
        
        elapsedNano = elapsed * sTimebaseInfo.numer / sTimebaseInfo.denom;
        
        uint64 ret = * (uint64 *) &elapsedNano;
        ret /= 1000;
        return (double)ret;
	}
};
#endif
#endif
