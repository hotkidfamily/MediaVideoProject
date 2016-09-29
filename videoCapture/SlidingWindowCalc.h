#pragma once
#include <stdint.h>
#include <Windows.h>
#include <list>

class CSlidingWindowCalc
{
public:
	CSlidingWindowCalc();
	~CSlidingWindowCalc();

	void reset();
	void reset(uint32_t durationInMS);
	uint32_t sizePerSecond() const;
	float frequencyPerSecond() const;
	int appendDataSize(uint32_t size);

	uint32_t getMaxSize() const;
	uint32_t getMinSize() const;

	uint64_t totalStreamSize() const;
	uint64_t totalInCalcStreamSize() const;

	uint64_t totalSamples() const;
	uint64_t totalInCalcSamples() const;

	uint64_t totalDuration() const;
	uint64_t totalInCalcDuration() const;

protected:
	struct RateSample {
		uint64_t timestamp;
		uint32_t sampleSize;
		uint64_t streamSize;
	};

	uint32_t m_duration;
	std::list<RateSample> m_sampleList;
	uint64_t m_totalSampleCount;
	uint64_t m_totalStreamSize;
	uint64_t m_startTickCount;
};

