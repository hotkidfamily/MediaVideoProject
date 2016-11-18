#pragma once
#include <stdint.h>
#include <Windows.h>
#include <list>

class CSlidingWindowCalc
{
public:
	CSlidingWindowCalc();
	~CSlidingWindowCalc();

	void Reset();
	void Reset(uint32_t durationInMS);

	int AppendSample(uint32_t size);

	uint32_t Bitrate() const;
	double Frequency() const;
	uint64_t AvgSampleSize() const;

	uint32_t MaxSample() const;
	uint32_t MinSample() const;

	uint64_t TotalSampleSize() const;
	uint64_t SampleSize() const;

	uint64_t TotalSamples() const;
	uint64_t Samples() const;

	uint64_t TotalDuration() const;
	uint64_t Duration() const;

protected:
	struct RateSample {
		uint64_t timestamp;
		uint32_t sampleSize;
		uint64_t streamSize;
	};

	uint32_t mInterval;
	std::list<RateSample> mSampleList;
	uint64_t mTotalSampleCount;
	uint64_t mTotalStreamSize;
	uint64_t mStartTickCount;
};

