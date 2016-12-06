#include "stdafx.h"
#include "logger.h"
#include "SlidingWindowCalc.h"

#define SAMPLE_COUNT_FPS (30)

CSlidingWindowCalc::CSlidingWindowCalc()
	: mIntervalInMs(5000)
	, mTotalSampleCount(0)
	, mTotalStreamSize(0)
	, mStartTickCount(GetTickCount())
	, mRingReadPos(0)
	, mRingWirtePos(0)
{
	mSampleRingCapability = mIntervalInMs / 1000 * SAMPLE_COUNT_FPS + SAMPLE_COUNT_FPS;
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
		mRingReadPos = 0;
	}
}

void CSlidingWindowCalc::Reset() {
	mTotalStreamSize = 0;
	mTotalSampleCount = 0;
	mStartTickCount = GetTickCount();

	mRingWirtePos = 0;
	mRingReadPos = 0;
}

void CSlidingWindowCalc::Reset(uint32_t durationInMS, uint32_t fps) {
	mIntervalInMs = durationInMS;
	mTotalStreamSize = 0;
	mTotalSampleCount = 0;
	mStartTickCount = GetTickCount();

	Destory();

	mSampleRingCapability = mIntervalInMs / 1000 * fps + SAMPLE_COUNT_FPS;

	mSampleRingBuffer = new RateSample[mSampleRingCapability];
	mRingWirtePos = 0;
	mRingReadPos = 0;
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
	uint64_t size = Samples();
	long readPos = mRingReadPos;

	for (uint64_t i = readPos; i < readPos + size; i++){
		uint32_t sampleSize = mSampleRingBuffer[i%mSampleRingCapability].sampleSize;
		if (minSize > sampleSize)
			minSize = sampleSize;
		else if (maxSize < sampleSize)
			maxSize = sampleSize;
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
	long readPos = 0;
	readPos = (mRingWirtePos - 1);
	if (readPos < 0){
		readPos = 0;
	}

	if (mTotalSampleCount > 2){
		sampleSize = mSampleRingBuffer[mRingWirtePos].streamSize - mSampleRingBuffer[readPos].streamSize;
	}

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
	}else{
		size = mSampleRingCapability - 1;
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
	long readPos = 0;
	long writePos = mRingWirtePos - 1;
	if (writePos < 0){
		writePos = 0;
	}

	if (mTotalSampleCount >= mSampleRingCapability){
		readPos = mRingWirtePos;
	}

	if (mTotalSampleCount > 2){
		durationTime = mSampleRingBuffer[writePos].timestamp - mSampleRingBuffer[readPos].timestamp;
	}

	return durationTime;
}
