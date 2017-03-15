#include "stdafx.h"
#include "IAudioRender.h"
#include "SyncRender.h"
#include "D3DSoundRender.h"

D3DSoundRender::D3DSoundRender()
	: mRenderEvent(nullptr)
	, mRenderThreadHandle(INVALID_HANDLE_VALUE)
	, mRenderThreadId(0)
	, mRenderThreadRuning(FALSE)

	, mCurRenderInterval(0)
	, mCurPtsInterval(0)
	, mLastRender(0)
	, mLastPts(0)

	, mFirstRender(TRUE)
	, mDropFrameCount(0)
{
}

D3DSoundRender::~D3DSoundRender()
{
}


BOOL D3DSoundRender::InitRender(const AudioRenderConfig &config)
{
	HRESULT hr = S_OK;

	//Init DirectSound
	if (FAILED(DirectSoundCreate8(NULL, &m_pDS, NULL))){
		goto fail;
	}

	if (FAILED(m_pDS->SetCooperativeLevel(config.hWnd, DSSCL_NORMAL))){
		goto fail;
	}


	DSBUFFERDESC dsbd;
	ZeroMemory(&dsbd, sizeof(dsbd));
	dsbd.dwSize = sizeof(dsbd);
	dsbd.dwFlags = DSBCAPS_GLOBALFOCUS | DSBCAPS_CTRLPOSITIONNOTIFY | DSBCAPS_GETCURRENTPOSITION2;
	dsbd.dwBufferBytes = MAX_AUDIO_BUF*BUFFERNOTIFYSIZE;
	//WAVE Header
	dsbd.lpwfxFormat = (WAVEFORMATEX*)malloc(sizeof(WAVEFORMATEX));
	dsbd.lpwfxFormat->wFormatTag = WAVE_FORMAT_PCM;
	/* format type */
	(dsbd.lpwfxFormat)->nChannels = config.channels;
	/* number of channels (i.e. mono, stereo...) */
	(dsbd.lpwfxFormat)->nSamplesPerSec = config.sampleRate;
	/* sample rate */
	(dsbd.lpwfxFormat)->nAvgBytesPerSec = config.sampleRate*(config.bitsPerSample>>3)*config.channels;
	/* for buffer estimation */
	(dsbd.lpwfxFormat)->nBlockAlign = (config.bitsPerSample>>3)*config.channels;
	/* block size of data */
	(dsbd.lpwfxFormat)->wBitsPerSample = config.bitsPerSample;
	/* number of bits per sample of mono data */
	(dsbd.lpwfxFormat)->cbSize = 0;


	if (FAILED(hr = m_pDS->CreateSoundBuffer(&dsbd, &m_pDSBuffer, NULL))){
		goto fail;
	}

	if (FAILED(hr = m_pDSBuffer->QueryInterface(IID_IDirectSoundBuffer8, (LPVOID*)&m_pDSBuffer8))){
		goto fail;
	}

	if (FAILED(m_pDSBuffer8->QueryInterface(IID_IDirectSoundNotify, (LPVOID*)&m_pDSNotify))){
		return FALSE;
	}

	for (int32_t i = 0; i < MAX_AUDIO_BUF; i++){
		m_pDSPosNotify[i].dwOffset = i*BUFFERNOTIFYSIZE;
		m_event[i] = ::CreateEvent(NULL, false, false, NULL);
		m_pDSPosNotify[i].hEventNotify = m_event[i];
	}
	m_pDSNotify->SetNotificationPositions(MAX_AUDIO_BUF, m_pDSPosNotify);
	m_pDSNotify->Release();

	m_pDSBuffer8->SetCurrentPosition(0);
	m_pDSBuffer8->Play(0, 0, DSBPLAY_LOOPING);

fail:
	return SUCCEEDED(hr);
}

BOOL D3DSoundRender::DeinitRender()
{
	mRenderThreadRuning = FALSE;
	if (mRenderThreadHandle != INVALID_HANDLE_VALUE){
		SetEvent(mRenderEvent);
		WaitForSingleObject(mRenderThreadHandle, INFINITE);
		mRenderThreadHandle = INVALID_HANDLE_VALUE;
	}

	for (int32_t i = 0; i < MAX_AUDIO_BUF; i++){
		CloseHandle(m_event[i]);
		m_event[i] = nullptr;
	}

	if (mRenderEvent){
		CloseHandle(mRenderEvent);
		mRenderEvent = nullptr;
	}
	// Release DirectSound interfaces
	SAFE_RELEASE(m_pDSBuffer8);
	SAFE_RELEASE(m_pDSBuffer);

	return TRUE;
}

BOOL D3DSoundRender::DrawStatus()
{
	HRESULT hr = S_OK;
	int32_t minInputSample = 0, maxInputSample = 0;
	int32_t minRenderSample = 0, maxRenderSample = 0;

	if (mInputStatis.Samples() > 2 && mRenderStatis.Samples() > 2){
		mInputStatis.MinMaxSample(minInputSample, maxInputSample);
		mRenderStatis.MinMaxSample(minRenderSample, maxRenderSample);
		
	}

	return TRUE;
}

BOOL D3DSoundRender::UpdateRenderStatis()
{
	DWORD renderBefore = timeGetTime();

	if (mLastRender){
		mCurRenderInterval = renderBefore - mLastRender;
		mRenderStatis.AppendSample(mCurRenderInterval);
	}

	mLastRender = renderBefore;

	return FALSE;
}


DWORD D3DSoundRender::RenderLoop()
{
	HRESULT hr = S_OK;
	DWORD dwRet = WAIT_OBJECT_0;

	while (mRenderThreadRuning){
		dwRet = WaitForMultipleObjects(MAX_AUDIO_BUF, m_event, FALSE, 5);
		if ((dwRet >= WAIT_OBJECT_0) && (dwRet <= WAIT_OBJECT_0 + 3)){
			if (!mRenderThreadRuning){
				logger(Info, "Render thread exit\n");
				break;
			} else{
				UpdateRenderStatis();
				DrawStatus();
				// feed buffer
			}
		} else if (dwRet == WAIT_TIMEOUT){
			continue;
		} else {
			logger(Info, "Render thread exit, error Code %x\n", GetLastError());
			break;
		}
	}

	return 0;
}

HRESULT D3DSoundRender::UpdateRenderSurface(AudioSampleBuffer *&frame)
{
	HRESULT hr = E_FAIL;
	LPVOID pbData = NULL;
	LPVOID pbData2 = NULL;
	DWORD dwLength = 0;
	DWORD dwLength2 = 0;

	if (FAILED(hr = m_pDSBuffer8->Lock(0, BUFFERNOTIFYSIZE, &pbData, &dwLength, &pbData2, &dwLength2, 0L)))
		return hr;

	// Copy the memory to it.
	//memcpy(pbData, pbWavData, g_dwBufferBytes);

	// Unlock the buffer, we don't need it anymore.
	m_pDSBuffer8->Unlock(pbData, BUFFERNOTIFYSIZE, NULL, 0);
	return hr;
};

BOOL D3DSoundRender::UpdatePushStatis(AudioSampleBuffer *&frame)
{
	if (mLastPts){
		mCurPtsInterval = frame->pts - mLastPts;
		mInputStatis.AppendSample((int32_t)mCurPtsInterval);
	}
	mLastPts = frame->pts;

	return TRUE;
}

BOOL D3DSoundRender::PushFrame(AudioSampleBuffer *inframe)
{
	HRESULT hr = E_FAIL;
	AudioSampleBuffer *frame = inframe;
	
	if (!frame){
		return FALSE;
	}

	if (mFirstRender){
		mRenderClock.Reset();
		mFirstRender = FALSE;
	}

	if (mRenderClock.PushFrame(frame)){
		hr = UpdateRenderSurface(frame);

		UpdatePushStatis(frame);

		SetEvent(mRenderEvent);
	} else{
		mDropFrameCount++;
	}

	return hr != S_OK;
}

