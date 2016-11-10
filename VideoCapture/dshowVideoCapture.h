#ifndef __DSHOWVIDEOCAPTURE_H__
#define __DSHOWVIDEOCAPTURE_H__

#include <comdef.h>
#include "dshowutil.h"
#include "mtype.h"
#include "ISampleGrabber.h"

typedef struct tagCameraDevDesc{
	STRING name;
	STRING clsid;
	STRING path;

	tagCameraDevDesc(VARIANT name, VARIANT path, VARIANT clsid){
		_bstr_t bName(name);
		_bstr_t bPath(path);
		_bstr_t bclsid(clsid);

		this->name.append(bName);
		this->path.append(bPath);
		this->clsid.append(bclsid);
	}
}CAMERADESC;

typedef std::list<CAMERADESC> CAMERALIST;

class DShowVideoCapture
{
public:
	DShowVideoCapture();
	~DShowVideoCapture();
	HRESULT GetDShowInterfaces();
	HRESULT ReleaseDShowInterfaces();
	HRESULT BuildGraph(int, int, int, int);
	HRESULT EnumCaptureDevices();
	HRESULT GetDevices(std::vector<const TCHAR*> );

private:
	void dshowInfo(HRESULT);
	HRESULT RemoveFiltersFromGraph();
	bool Runing();
	HRESULT Stop();
	HRESULT Start();
	HRESULT findFilterByIndex(int);

private:
	IGraphBuilder *mGraph;
	ICaptureGraphBuilder2 *mGraphBuiler;
	IMediaControl *mMediaControl;
	IMediaEventEx *mMediaEventEx;
	IVideoWindow *mVideoWindow;
	IAMDroppedFrames *mCaptureStatus;
	IAMVideoControl *mVideoControl;
	IBaseFilter *mCaptureFilter;

	IBaseFilter *mGrabberFiler;
	ISampleGrabber *mGrabber;

	CAMERALIST camlist;
};

#endif //__DSHOWVIDEOCAPTURE_H__