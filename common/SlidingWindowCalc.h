#pragma once

#include <stdint.h>
#include "AutoLock.h"

class CSlidingWindowCalc
{
public:
	CSlidingWindowCalc();
	~CSlidingWindowCalc();

	void Reset();
	void Reset(uint32_t durationInMS, uint32_t fps);

	int32_t AppendSample(uint32_t size);

	uint32_t Bitrate();
	double Frequency();
	uint64_t AvgSampleSize();

	BOOL MinMaxSample(int32_t &minV, int32_t &maxV) ;

	uint64_t TotalSampleSize() const;
	uint64_t SampleSize() ;

	uint64_t TotalSamples() const;
	uint64_t Samples() ;

	uint64_t TotalDuration() const;
	uint64_t Duration() ;

protected:
	void Destory();

protected:
	struct RateSample {
		uint64_t timestamp;
		uint32_t sampleSize;
		uint64_t streamSize;
		RateSample(){
			ZeroMemory(this, sizeof(struct  RateSample));
		}
	};

	// common
	uint32_t mIntervalInMs;
	volatile uint64_t mTotalSampleCount;
	volatile uint64_t mTotalStreamSize;
	volatile uint64_t mStartTickCount;

	// new method: ring buffer
	RateSample *mSampleRingBuffer;
	long mSampleRingCapability;
	volatile long mRingWirtePos;

	CRITICAL_SECTION cs;
};

