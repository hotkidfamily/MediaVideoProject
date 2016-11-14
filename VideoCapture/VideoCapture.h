#ifndef __VIDEOCAPTURE_H__
#define __VIDEOCAPTURE_H__

#include "IVideoCapture.h"
#include "logger.h"
#include "dshowVideoCapture.h"
#include "mediafoundationVideoCapture.h"


class IVideoCaptureDelegate : 
	public IVideoCapture
{
public:
	virtual ~IVideoCaptureDelegate();
	virtual void RegisterCallback(VideoCaptureCallback *);
	virtual HRESULT GetDeviceList(VECT &);
	virtual HRESULT StartCaptureWithParam(OPEN_DEVICE_PARAM );
	virtual HRESULT StopCapture();
	virtual HRESULT ShowPropertyWindow(HWND parentWindowHandle);
	virtual HRESULT UnRegisterCallback();

	bool InitContext();
	bool UnInitContext();

protected:

private:
	DShowVideoCapture *mDShowCapture;
	MediaFoundationVideoCapture *mMFCapture;
};

#endif //__VIDEOCAPTURE_H__
