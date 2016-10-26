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
	HRESULT BuildGraph();
	HRESULT EnumCaptureDevices();

private:
	IGraphBuilder *mGraph;
	ICaptureGraphBuilder2 *mGraphBuiler;
	IBaseFilter *mRenderFiler;
	IBaseFilter *mCaptureFilter;

	IBaseFilter *mGrabberFiler;
	ISampleGrabber *mGrabber;
	
};

#endif //__DSHOWVIDEOCAPTURE_H__