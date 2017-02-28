#ifndef __DSHOWVIDEOCAPTURE_H__
#define __DSHOWVIDEOCAPTURE_H__

#include "dshowutil.h"
#include "mtype.h"
#include "ISampleGrabber.h"
#include "ISampleGrabberCBImpl.h"
#include "IVideoCapture.h"
#include "SampleBufferManager.h"

typedef struct tagCameraDevDesc{
	int32_t index;
	STRING name;
	STRING path;

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

	// true: equal, path maybe empty
	bool operator ==(const struct tagCameraDevDesc &desc){
		return ( !(this->name.compare(desc.name))
					&& !(this->path.compare(desc.path)) );
	}
}CAMERADESC;

typedef std::vector<CAMERADESC> CAMERALIST;

class DShowVideoCapture : 
	public ISampleGrabberCBImpl
{
public:
	DShowVideoCapture();
	~DShowVideoCapture();
	HRESULT Stop();
	HRESULT Start(CAPTURECONFIG&);
	HRESULT EnumCaptureDevices();
	HRESULT GetDevicesName(VECT &);
	HRESULT ShowCapturePropertyWindow();
	HRESULT RegisterCallback(VideoCaptureCallback *cb);
	HRESULT UnregisterCallback();
	HRESULT GetDShowInterfaces();
	BOOL GetFrame(CSampleBuffer *&pSample);
	BOOL ReleaseFrame(CSampleBuffer *&pSample);

private:
	HRESULT RemoveFiltersFromGraph();
	BOOL Runing();
	HRESULT ReleaseDShowInterfaces();
	HRESULT FindCaptureFilterByIndex(int, IBaseFilter *&);
	HRESULT BuildGraph();
	void ShowDShowError(HRESULT hr);
	STDMETHODIMP SampleCB(double SampleTime, IMediaSample *pSample);
	HRESULT FindMediaTypeInPinOrStreamConfig(CComPtr<IPin> &pOutPin, CMediaType &, IAMStreamConfig *);
	HRESULT SaveGraphFile(IGraphBuilder*, TCHAR* path);
	HRESULT GetFilterFriendlyName(IBaseFilter * &filter, STRING name);

private:

	BOOL mbMapTimeToLocal;
	CClock mBaseClock;

	DWORD mGraphRegisterHandler;
	IGraphBuilder *mGraph;
	ICaptureGraphBuilder2 *mGraphBuiler;
	IMediaControl *mMediaControl;
	IMediaEventEx *mMediaEventEx;

	ISampleGrabber *mVideoGrabber;
	IBaseFilter *mCaptureFilter;
	IBaseFilter *mGrabberFiler;

	IAMDroppedFrames *mDropFrameStatus;
	long mCapFrames;
	long mDropFrames;

	CMediaType mWorkMediaType;

	VideoCaptureCallback *mcb;

	CAPTURECONFIG mWorkParams;
	CAMERALIST mCameraList;
	CSlidingWindowCalc mFpsStats;
	CSampleBufferManager mBufferManager;
};

#endif //__DSHOWVIDEOCAPTURE_H__