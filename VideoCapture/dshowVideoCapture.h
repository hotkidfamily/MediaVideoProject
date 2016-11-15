#ifndef __DSHOWVIDEOCAPTURE_H__
#define __DSHOWVIDEOCAPTURE_H__

#include <comdef.h>

#include "dshowutil.h"
#include "mtype.h"
#include "ISampleGrabber.h"
#include "ISampleGrabberCBImpl.h"
#include "SlidingWindowCalc.h"
#include "IVideoCapture.h"

typedef struct tagCameraDevDesc{
	int32_t index;
	STRING name;
	STRING path;

	tagCameraDevDesc(){}

	tagCameraDevDesc(VARIANT name, VARIANT path, int32_t index=0){
		_bstr_t bName(name);
		_bstr_t bPath(path);

		this->name.append(bName);
		this->path.append(bPath);
		this->index = index;
	}

	void reset(VARIANT name, VARIANT path, int32_t index = 0){
		_bstr_t bName(name);
		_bstr_t bPath(path);

		this->name.append(bName);
		this->path.append(bPath);
		this->index = index;
	}

	// true: equal
	bool operator ==(const struct tagCameraDevDesc &desc){
		return !( (this->name.compare(desc.name))
					&& (this->path.compare(desc.path)) );
	}
}CAMERADESC;

typedef std::vector<CAMERADESC> CAMERALIST;

typedef struct tagFrameBility{
	enum ability{
		SU_RES = 1 << 15,
		SU_FPS = 1 << 14,
		SU_RES_RATIO = 1 << 13,
		SU_FPS_SMALL = 1 << 12,

		SU_RES_LARGE = 1 << 11,
		SU_FPS_LARGE = 1<< 10,

		SU_RES_LARGE_INAREA = 1 << 9,
	};
	int32_t Proirity;
	int32_t Ability;
	SIZE ImageSize;
	LONGLONG MinFrameInterval;
	LONGLONG MaxFrameInterval;
	CMediaType MediaType;
	tagFrameBility(){
		ZeroMemory(this, sizeof(tagFrameBility));
	}
}FRAMEABILITY, *PFRAMEABILITY;

class DShowVideoCapture : 
	public ISampleGrabberCBImpl
{
public:
	DShowVideoCapture();
	~DShowVideoCapture();
	HRESULT Stop();
	HRESULT Start(OPEN_DEVICE_PARAM);
	HRESULT EnumCaptureDevices();
	HRESULT GetDevicesName(VECT &);
	HRESULT ShowCapturePropertyWindow();
	HRESULT RegisterCallback(VideoCaptureCallback *cb);
	HRESULT UnregisterCallback();
	HRESULT GetDShowInterfaces();

private:
	HRESULT RemoveFiltersFromGraph();
	BOOL Runing();
	HRESULT ReleaseDShowInterfaces();
	HRESULT FindFilterByIndex(int, IBaseFilter *&);
	HRESULT BuildGraph();
	void ShowDShowError(HRESULT hr);
	STDMETHODIMP SampleCB(double SampleTime, IMediaSample *pSample);
	HRESULT FindMediaTypeInPin(CComPtr<IPin> &pOutPin);
	inline BOOL IsFormatSupport(CMediaType &, FRAMEABILITY&);
	HRESULT SaveGraphFile(IGraphBuilder*, TCHAR* path);

private:
	IGraphBuilder *mGraph;
	ICaptureGraphBuilder2 *mGraphBuiler;
	IMediaControl *mMediaControl;
	IMediaEventEx *mMediaEventEx;
	IAMDroppedFrames *mDropFrameStatus;

	long mCapFrames;
	long mDropFrames;

	ISampleGrabber *mGrabber;
	IBaseFilter *mCaptureFilter;
	IBaseFilter *mGrabberFiler;

	CMediaType mWorkMediaType;

	VideoCaptureCallback *mcb;

	OPEN_DEVICE_PARAM mWorkParams;
	CAMERALIST mCameraList;
	CSlidingWindowCalc mFpsStats;
};

#endif //__DSHOWVIDEOCAPTURE_H__