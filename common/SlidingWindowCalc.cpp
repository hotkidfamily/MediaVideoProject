#include "stdafx.h"
#include "SlidingWindowCalc.h"

#define SAMPLE_COUNT_FPS (30)

CSlidingWindowCalc::CSlidingWindowCalc()
	: mIntervalInMs(5000)
	, mTotalSampleCount(0)
	, mTotalStreamSize(0)
	, mStartTickCount(GetTickCount())
	, mRingReadPos(0)
{
	mSampleRingCapability = mIntervalInMs / 1000 * SAMPLE_COUNT_FPS + SAMPLE_COUNT_FPS;
	mSampleRingBuffer = new RateSample[mSampleRingCapability];
	mRingWirtePos = 0;
}

CSlidingWindowCalc::~CSlidingWindowCalc()
{
	Destory();
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
	volatile long pos = 0;
	mTotalSampleCount++;
	mTotalStreamSize += size;


	RateSample *sample = &mSampleRingBuffer[mRingWirtePos];
	sample->sampleSize = size;
	sample->timestamp = GetTickCount();
	sample->streamSize = mTotalStreamSize;

	while (Duration() > mIntervalInMs){
		pos = (mRingReadPos + 1) % mSampleRingCapability;
		InterlockedExchange(&mRingReadPos, pos);
	}

	pos = (mRingWirtePos + 1) % mSampleRingCapability;
	InterlockedExchange(&mRingWirtePos, pos);

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

	avg = SampleSize() / Samples();

	return avg;
}

BOOL CSlidingWindowCalc::MinMaxSample(int32_t &minV, int32_t &maxV) const
{
	uint32_t minSize = 0xffffffff;
	uint32_t maxSize = 0;
	uint64_t size = Samples();
	long readPos = 0;
	InterlockedExchange(&readPos, mRingReadPos);

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

uint64_t CSlidingWindowCalc::SampleSize() const
{
	uint64_t sampleSize = 0;
	long readPos = 0;
	long writePos = 0;
	InterlockedExchange(&readPos, mRingReadPos);
	InterlockedExchange(&writePos, mRingWirtePos);
	writePos = (writePos + mSampleRingCapability - 1) % mSampleRingCapability;

	if (mTotalSampleCount > 2){
		sampleSize = mSampleRingBuffer[writePos].streamSize - mSampleRingBuffer[readPos].streamSize;
	}

	return sampleSize;
}

uint64_t CSlidingWindowCalc::TotalSamples() const
{
	return mTotalSampleCount;
}

uint64_t CSlidingWindowCalc::Samples() const
{
	long size = 150;

	size = mRingWirtePos - mRingReadPos;

	if (size <= 0)
		size = mSampleRingCapability + size;

	return size;
}

uint64_t CSlidingWindowCalc::TotalDuration() const
{
	return (GetTickCount() - mStartTickCount);
}

uint64_t CSlidingWindowCalc::Duration() const
{
	uint64_t durationTime = 1;
	long readPos = 0;
	long writePos = 0;
	InterlockedExchange(&readPos, mRingReadPos);
	InterlockedExchange(&writePos, mRingWirtePos);

	if (mTotalSampleCount > 2){
		durationTime = mSampleRingBuffer[(writePos - 1 + mSampleRingCapability) % mSampleRingCapability].timestamp - mSampleRingBuffer[readPos].timestamp;
	}

	return durationTime;
}
