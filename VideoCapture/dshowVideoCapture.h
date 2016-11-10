#ifndef __DSHOWVIDEOCAPTURE_H__
#define __DSHOWVIDEOCAPTURE_H__

#include <comdef.h>

#include "IVideoCapture.h"
#include "dshowutil.h"
#include "mtype.h"
#include "ISampleGrabber.h"
#include "video.h"

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
	// false: not equal
	bool operator ==(const struct tagCameraDevDesc &desc){
		return !( (this->name.compare(desc.name))
					&& (this->path.compare(desc.path)) );
	}
}CAMERADESC;

typedef std::vector<CAMERADESC> CAMERALIST;

class DShowVideoCapture
{
public:
	DShowVideoCapture();
	~DShowVideoCapture();
	HRESULT Stop();
	HRESULT Start(OPEN_DEVICE_PARAM);
	HRESULT EnumCaptureDevices();
	HRESULT GetDevices(std::vector<const TCHAR*> );
	HRESULT Repaint(HDC hdc);
	HRESULT UpdateVideoWindow(HWND hWnd, const LPRECT prc);
	HRESULT ShowCapturePropertyWindow();

private:
	void dshowInfo(HRESULT);
	HRESULT RemoveFiltersFromGraph();
	bool Runing();
	HRESULT GetDShowInterfaces();
	HRESULT ReleaseDShowInterfaces();
	HRESULT findFilterByIndex(int, IBaseFilter *&);
	HRESULT BuildGraph();

private:
	IGraphBuilder *mGraph;
	ICaptureGraphBuilder2 *mGraphBuiler;
	IMediaControl *mMediaControl;
	IMediaEventEx *mMediaEventEx;
	BaseVideoRenderer *mRender;
	IAMDroppedFrames *mCaptureStatus;
	IAMVideoControl *mVideoControl;
	IBaseFilter *mCaptureFilter;

	IBaseFilter *mGrabberFiler;
	ISampleGrabber *mGrabber;


	OPEN_DEVICE_PARAM workParams;
	CAMERALIST camlist;
};

#endif //__DSHOWVIDEOCAPTURE_H__