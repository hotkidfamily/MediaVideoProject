#ifndef __VIDEOCAPTURE_H__
#define __VIDEOCAPTURE_H__

#include "IVideoCapture.h"
#include "dshowVieoCapture.h"
#include "mediafoundationVideoCapture.h"
#include "logger.h"

class IVideoCaptureDelegate: public IVideoCapture
{
public:
	virtual ~IVideoCaptureDelegate();
	virtual void RegisterVideoCaptureCallback(VideoCaptureCallback *);
	virtual HRESULT GetDeviceList(std::list<CAMERADESC> &);
	virtual HRESULT StartCaptureByIndexWithResolutionAndFramerate(int, int, int, int);
	virtual HRESULT StopCapture();
	virtual HRESULT ShowPropertyWindow(HWND parentWindowHandle);

private:
	VideoCaptureCallback *mCaptureCallback;
};

#endif //__VIDEOCAPTURE_H__
