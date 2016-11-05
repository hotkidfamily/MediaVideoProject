#ifndef __VIDEOCAPTURE_H__
#define __VIDEOCAPTURE_H__

#include "IVideoCapture.h"
#include "dshowVideoCapture.h"
#include "mediafoundationVideoCapture.h"
#include "logger.h"

class IVideoCaptureDelegate: public IVideoCapture
{
public:
	virtual ~IVideoCaptureDelegate();
	virtual void RegisterVideoCaptureCallback(VideoCaptureCallback *);
	virtual HRESULT GetDeviceList(std::vector<char*> &);
	virtual HRESULT StartCaptureByIndexWithResolutionAndFramerate(int, int, int, int);
	virtual HRESULT StopCapture();
	virtual HRESULT ShowPropertyWindow(HWND parentWindowHandle);

private:
	VideoCaptureCallback *mCaptureCallback;
	DShowVideoCapture *mDShowCapture;
	MediaFoundationVideoCapture *mMFCapture;
};

#endif //__VIDEOCAPTURE_H__
