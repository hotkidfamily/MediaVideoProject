#pragma once
class CSampleBuffer
{
public:
	CSampleBuffer();
	~CSampleBuffer();

	BOOL reset(int32_t dataSize, uint8_t *bufferPtr, int32_t capacity, DWORD pixelFormatInFourCC){
		this->bOccupy = FALSE;
		this->pts = 0;
		this->dataSize = dataSize; 
		this->capacity = capacity;
		this->pixelFormat = pixelFormatInFourCC;
		this->bufferPtr = bufferPtr;
	}

private:
	BOOL bOccupy;
	int64_t pts;
	int32_t pixelFormat;
	int32_t capacity;
	int32_t dataSize;
	uint8_t *bufferPtr;
};

