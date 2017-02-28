#include "stdafx.h"
#include "SlidingWindowCalc.h"

#define SAMPLE_COUNT_FPS (30)

CSlidingWindowCalc::CSlidingWindowCalc()
	: mIntervalInMs(5000)
	, mTotalSampleCount(0)
	, mTotalStreamSize(0)
	, mStartTickCount(GetTickCount())
	, mRingWirtePos(0)
	, mSampleRingCapability(6 * SAMPLE_COUNT_FPS)
{
	mSampleRingBuffer = new RateSample[mSampleRingCapability];
	InitializeCriticalSection(&cs);
}

CSlidingWindowCalc::~CSlidingWindowCalc()
{
	Destory();
	DeleteCriticalSection(&cs);
}

void CSlidingWindowCalc::Destory()
{
	if (mSampleRingBuffer){
		delete[] mSampleRingBuffer;
		mSampleRingCapability = 0;
		mRingWirtePos = 0;
	}
}

void CSlidingWindowCalc::Reset() {
	mTotalStreamSize = 0;
	mTotalSampleCount = 0;
	mStartTickCount = GetTickCount();

	mRingWirtePos = 0;
}

void CSlidingWindowCalc::Reset(uint32_t durationInMS, uint32_t fps) {
	mIntervalInMs = durationInMS;
	mTotalStreamSize = 0;
	mTotalSampleCount = 0;
	mStartTickCount = GetTickCount();

	Destory();

	mSampleRingBuffer = new RateSample[mSampleRingCapability];
	mRingWirtePos = 0;
}

int32_t CSlidingWindowCalc::AppendSample(uint32_t size)
{
	CAutoLock lock(cs);

	volatile long pos = 0;
	mTotalSampleCount++;
	mTotalStreamSize += size;

	RateSample *sample = &mSampleRingBuffer[mRingWirtePos];
	sample->sampleSize = size;
	sample->timestamp = GetTickCount();
	sample->streamSize = mTotalStreamSize;

	mRingWirtePos = (mRingWirtePos + 1) % mSampleRingCapability;

	return 0;
}

double CSlidingWindowCalc::Frequency()
{
	CAutoLock lock(cs);

	return (Samples()*1000.0 / Duration());
}

uint32_t CSlidingWindowCalc::Bitrate()
{
	CAutoLock lock(cs);

	uint64_t bitrate = SampleSize() / Duration();

	return (uint32_t)bitrate;
}

uint64_t CSlidingWindowCalc::AvgSampleSize() 
{
	CAutoLock lock(cs);

	uint64_t avg = 1;

	avg = SampleSize() / Samples();

	return avg;
}

BOOL CSlidingWindowCalc::MinMaxSample(int32_t &minV, int32_t &maxV) 
{
	CAutoLock lock(cs);

	uint32_t minSize = 0xffffffff;
	uint32_t maxSize = 0;

	if (mTotalSampleCount > mSampleRingCapability){
		for (uint64_t i = 0; i < mSampleRingCapability; i++){
			uint32_t sampleSize = mSampleRingBuffer[i].sampleSize;
			if (minSize > sampleSize)
				minSize = sampleSize;
			else if (maxSize < sampleSize)
				maxSize = sampleSize;
		}
	} else{
		for (uint64_t i = 0; i < mRingWirtePos; i++){
			uint32_t sampleSize = mSampleRingBuffer[i].sampleSize;
			if (minSize > sampleSize)
				minSize = sampleSize;
			else if (maxSize < sampleSize)
				maxSize = sampleSize;
		}
	}

	minV = minSize == 0xffffffff ? 0 : minSize; maxV = maxSize;

	return TRUE;
}

uint64_t CSlidingWindowCalc::TotalSampleSize() const
{
	return mTotalStreamSize;
}

uint64_t CSlidingWindowCalc::SampleSize()
{
	uint64_t sampleSize = 0;
	long recentPos = 0;
	long oldestPos = 0;

	recentPos = (mRingWirtePos - 1);
	if (recentPos < 0){
		recentPos = mSampleRingCapability - 1;
	}
	
	if (mTotalSampleCount > mSampleRingCapability){
		oldestPos = mRingWirtePos;
	}
	sampleSize = mSampleRingBuffer[recentPos].streamSize - mSampleRingBuffer[oldestPos].streamSize;

	return sampleSize;
}

uint64_t CSlidingWindowCalc::TotalSamples() const
{
	return mTotalSampleCount;
}

uint64_t CSlidingWindowCalc::Samples()
{
	long size = 1;

	if (mTotalSampleCount < mSampleRingCapability){
		size = mRingWirtePos - 1;
		if (size <= 0){
			size = 1;
		}
	}else{
		size = mSampleRingCapability;
	}

	return size;
}

uint64_t CSlidingWindowCalc::TotalDuration() const
{
	return (GetTickCount() - mStartTickCount);
}

uint64_t CSlidingWindowCalc::Duration()
{
	uint64_t durationTime = 1;
	long oldestPos = 0;
	long recentPos = mRingWirtePos - 1;
	if (recentPos < 0){
		recentPos = mSampleRingCapability - 1;
	}

	if (mTotalSampleCount > mSampleRingCapability){
		oldestPos = mRingWirtePos;
	}

	if (recentPos != oldestPos){
		durationTime = mSampleRingBuffer[recentPos].timestamp - mSampleRingBuffer[oldestPos].timestamp;
	}

	return durationTime;
}
