#ifndef __VIDEOCAPTURE_H__
#define __VIDEOCAPTURE_H__

#include "IVideoCapture.h"
#include "dshowVideoCapture.h"
#include "WMFVideoCapture.h"
#include "filesVideoCapture.h"

typedef struct tagFrameBility{
	enum ability{
		SU_RES = 1 << 15,
		SU_FPS = 1 << 14,
		SU_RES_RATIO = 1 << 13,
		SU_FPS_SMALL = 1 << 12,

		SU_RES_LARGE = 1 << 11,
		SU_FPS_LARGE = 1 << 10,

		SU_RES_LARGE_INAREA = 1 << 9,
		SU_RES_SMALL_INAREA = 1 << 8,
	};

	int32_t Priority;
	int32_t Ability;
	SIZE ImageSize;
	LONGLONG MinFrameInterval;
	LONGLONG MaxFrameInterval;
	CMediaType MediaType;
	tagFrameBility(){
		ZeroMemory(this, sizeof(tagFrameBility));
	}
}FRAMEABILITY, *PFRAMEABILITY;

typedef enum tagSupportCaptureDevice{
	CAP_DEV_NONE,
	CAP_DEV_DSHOW,
	CAP_DEV_FILE,
	CAP_DEV_WMF,
}CAPSUPPORTDEV;

BOOL IsFormatSupport(const GUID *id, FRAMEABILITY & bility);

class IVideoCaptureDelegate : 
	public IVideoCapture
{
public:
	IVideoCaptureDelegate();
	virtual ~IVideoCaptureDelegate();
	virtual void RegisterCallback(VideoCaptureCallback *);
	virtual HRESULT GetDeviceList(VECT &);
	virtual HRESULT StartCaptureWithParam(CAPTURECONFIG &);
	virtual BOOL GetFrame(CSampleBuffer *&pSample);
	virtual BOOL ReleaseFrame(CSampleBuffer *&pSample);
	virtual HRESULT StopCapture();
	virtual HRESULT ShowPropertyWindow(HWND parentWindowHandle);
	virtual HRESULT UnRegisterCallback();

	bool InitContext();
	bool UnInitContext();

protected:

private:
	CAPSUPPORTDEV mDevice;
	DShowVideoCapture *mDShowCapture;
	FilesVideoCapture *mFilesCapture;

	CClock mBaseClock;
};

#endif //__VIDEOCAPTURE_H__
