#include "stdafx.h"
#include "SlidingWindowCalc.h"

CSlidingWindowCalc::CSlidingWindowCalc()
	: m_duration(5000)
	, m_totalSampleCount(0)
	, m_totalStreamSize(0)
	, m_startTickCount(GetTickCount())
{

}

CSlidingWindowCalc::~CSlidingWindowCalc()
{
}

void CSlidingWindowCalc::reset() {
	m_totalStreamSize = 0;
	m_totalSampleCount = 0;
	m_startTickCount = GetTickCount();
	m_sampleList.clear();
}

void CSlidingWindowCalc::reset(uint32_t durationInMS) {
	m_duration = durationInMS;
	m_totalStreamSize = 0;
	m_totalSampleCount = 0;
	m_startTickCount = GetTickCount();
	m_sampleList.clear();
}

int CSlidingWindowCalc::appendDataSize(uint32_t size)
{
	m_totalSampleCount++;
	m_totalStreamSize += size;

	RateSample sample;
	sample.sampleSize = size;
	sample.timestamp = GetTickCount();
	sample.streamSize = m_totalStreamSize;
	m_sampleList.push_back(sample);

	while (totalInCalcDuration() > m_duration)
		m_sampleList.pop_front();

	return 0;
}

float CSlidingWindowCalc::frequencyPerSecond() const
{
	return (totalInCalcSamples()*1000.0 / totalInCalcDuration());
}

uint32_t CSlidingWindowCalc::sizePerSecond() const
{
	const RateSample &firstSample = *m_sampleList.begin();
	const RateSample &lastSample = *m_sampleList.rbegin();

	uint64_t duration = totalInCalcDuration();
	if (duration > 0) {
		// count in micro second
		uint64_t bitrate = (totalInCalcStreamSize()) * 1000 / duration;
		return (uint32_t)bitrate;
	}

	return 0;
}

uint32_t CSlidingWindowCalc::getMaxSize() const 
{
	uint32_t maxSize = 0;
	for (std::list<RateSample>::const_iterator it = m_sampleList.begin();
		it != m_sampleList.end(); ++it) {
		if (maxSize < it->sampleSize) {
			maxSize = it->sampleSize;
		}
	}

	return maxSize;
}

uint32_t CSlidingWindowCalc::getMinSize() const
{
	uint32_t minSize = 0;
	for (std::list<RateSample>::const_iterator it = m_sampleList.begin();
		it != m_sampleList.end(); ++it) {
		if (minSize > it->sampleSize) {
			minSize = it->sampleSize;
		}
	}

	return minSize;
}

uint64_t CSlidingWindowCalc::totalStreamSize() const
{
	return m_totalStreamSize;
}

uint64_t CSlidingWindowCalc::totalInCalcStreamSize() const
{
	const RateSample &firstSample = *m_sampleList.begin();
	const RateSample &lastSample = *m_sampleList.rbegin();

	uint64_t streamSize = lastSample.streamSize - firstSample.streamSize;

	return streamSize;
}

uint64_t CSlidingWindowCalc::totalSamples() const 
{
	return m_totalSampleCount;
}

uint64_t CSlidingWindowCalc::totalInCalcSamples() const
{
	return m_sampleList.size();
}

uint64_t CSlidingWindowCalc::totalDuration() const
{
	return (GetTickCount() - m_startTickCount);
}

uint64_t CSlidingWindowCalc::totalInCalcDuration() const
{
	const RateSample &firstSample = *m_sampleList.begin();
	const RateSample &lastSample = *m_sampleList.rbegin();

	uint64_t durationTime = lastSample.timestamp - firstSample.timestamp;

	return durationTime;
}
