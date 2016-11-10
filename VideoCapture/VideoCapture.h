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
	virtual void RegisterCallback(VideoCaptureCallback *);
	virtual HRESULT GetDeviceList(std::vector<const TCHAR*> &);
	virtual HRESULT StartCaptureWithParam(OPEN_DEVICE_PARAM );
	virtual HRESULT StopCapture();
	virtual HRESULT ShowPropertyWindow(HWND parentWindowHandle);

protected:

private:
	VideoCaptureCallback *mCaptureCallback;
	DShowVideoCapture *mDShowCapture;
	MediaFoundationVideoCapture *mMFCapture;
};

#endif //__VIDEOCAPTURE_H__
