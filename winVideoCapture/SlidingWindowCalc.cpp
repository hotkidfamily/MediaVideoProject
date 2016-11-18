#include "stdafx.h"
#include "SlidingWindowCalc.h"

CSlidingWindowCalc::CSlidingWindowCalc()
	: mInterval(5000)
	, mTotalSampleCount(0)
	, mTotalStreamSize(0)
	, mStartTickCount(GetTickCount())
{

}

CSlidingWindowCalc::~CSlidingWindowCalc()
{
}

void CSlidingWindowCalc::Reset() {
	mTotalStreamSize = 0;
	mTotalSampleCount = 0;
	mStartTickCount = GetTickCount();
	mSampleList.clear();
}

void CSlidingWindowCalc::Reset(uint32_t durationInMS) {
	mInterval = durationInMS;
	mTotalStreamSize = 0;
	mTotalSampleCount = 0;
	mStartTickCount = GetTickCount();
	mSampleList.clear();
}

int CSlidingWindowCalc::AppendSample(uint32_t size)
{
	mTotalSampleCount++;
	mTotalStreamSize += size;

	RateSample sample;
	sample.sampleSize = size;
	sample.timestamp = GetTickCount();
	sample.streamSize = mTotalStreamSize;
	mSampleList.push_back(sample);

	while (Duration() > mInterval)
		mSampleList.pop_front();

	return 0;
}

double CSlidingWindowCalc::Frequency() const
{
	return (Samples()*1000.0 / Duration());
}

uint32_t CSlidingWindowCalc::Bitrate() const
{
	uint64_t bitrate = SampleSize() / Duration();

	return (uint32_t)bitrate;
}

uint64_t CSlidingWindowCalc::AvgSampleSize() const
{
	uint64_t avg = 1;
	if (mSampleList.size())
		avg = SampleSize() / Samples();
	return avg;
}

uint32_t CSlidingWindowCalc::MaxSample() const 
{
	uint32_t maxSize = 0;
	for (std::list<RateSample>::const_iterator it = mSampleList.begin();
		it != mSampleList.end(); ++it) {
		maxSize = max(maxSize, it->sampleSize);
	}

	return maxSize;
}

uint32_t CSlidingWindowCalc::MinSample() const
{
	uint32_t minSize = 0xffffffff;
	for (std::list<RateSample>::const_iterator it = mSampleList.begin();
		it != mSampleList.end(); ++it) {
		minSize = min(minSize, it->sampleSize);
	}

	return minSize;
}

uint64_t CSlidingWindowCalc::TotalSampleSize() const
{
	return mTotalStreamSize;
}

uint64_t CSlidingWindowCalc::SampleSize() const
{
	uint64_t sampleSize = 0;
	if (mSampleList.size() > 2){
		sampleSize = mSampleList.back().streamSize - mSampleList.front().streamSize;
	}

	return sampleSize;
}

uint64_t CSlidingWindowCalc::TotalSamples() const 
{
	return mTotalSampleCount;
}

uint64_t CSlidingWindowCalc::Samples() const
{
	return mSampleList.size();
}

uint64_t CSlidingWindowCalc::TotalDuration() const
{
	return (GetTickCount() - mStartTickCount);
}

uint64_t CSlidingWindowCalc::Duration() const
{
	uint64_t durationTime = 1;
	if (mSampleList.size() > 2){
		durationTime = mSampleList.back().timestamp - mSampleList.front().timestamp;
	}
	return durationTime;
}
