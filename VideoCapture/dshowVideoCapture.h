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

typedef struct tagVideoInfo{
	int pinIndex;
	CMediaType *meidaType;
	LONGLONG MinFrameInterval;
	LONGLONG MaxFrameInterval;
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
	bool Runing();
	HRESULT ReleaseDShowInterfaces();
	HRESULT FindFilterByIndex(int, IBaseFilter *&);
	HRESULT BuildGraph();
	void ShowDShowError(HRESULT hr);
	STDMETHODIMP SampleCB(double SampleTime, IMediaSample *pSample);
	HRESULT FindSultablePin(CComPtr<IPin> &pOutPin);
	HRESULT FindVideoConfigByStreamConfig(CComPtr<IAMStreamConfig> &pConfig);

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