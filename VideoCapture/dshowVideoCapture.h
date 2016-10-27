#ifndef __DSHOWVIDEOCAPTURE_H__
#define __DSHOWVIDEOCAPTURE_H__

#include "dshowutil.h"
#include "mtype.h"
#include "ISampleGrabber.h"

class DShowVideoCapture
{
public:
	DShowVideoCapture();
	~DShowVideoCapture();
	HRESULT GetDShowInterfaces();
	HRESULT ReleaseDShowInterfaces();
	HRESULT BuildGraph();
	HRESULT EnumCaptureDevices();

private:
	void dshowInfo(HRESULT);

private:
	IGraphBuilder *mGraph;
	ICaptureGraphBuilder2 *mGraphBuiler;
	IMediaControl *mMediaControl;
	IMediaEventEx *mMediaEventEx;
	IVideoWindow *mVideoWindow;
	IBaseFilter *mRenderFiler;
	IBaseFilter *mCaptureFilter;

	IBaseFilter *mGrabberFiler;
	ISampleGrabber *mGrabber;
	
};

#endif //__DSHOWVIDEOCAPTURE_H__