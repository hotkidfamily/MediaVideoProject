#ifndef __IVIDEOCAPTURE_H__
#define __IVIDEOCAPTURE_H__

enum VideoCaptureEvent
{
	OPENSUCCESS_EVENT,
	LOSTCONNECT_EVENT,
	PLUGINOUT_EVENT,
};

typedef struct tagCameraDevDesc{
	std::string name;
	std::string clsid;
	std::string path;
}CAMERADESC;

typedef std::list<CAMERADESC> CAMERALIST;

class VideoCaptureCallback
{
public:
	virtual void OnFrame(uint8_t *frame, int64_t frameSize, int64_t timestamp) = 0;
	virtual void OnEvent(VideoCaptureEvent) = 0;
};

class IVideoCapture
{
public:
	virtual ~IVideoCapture(){};
	virtual void RegisterVideoCaptureCallback(VideoCaptureCallback *) = 0;
	virtual HRESULT GetDeviceList(std::list<CAMERADESC> &) = 0;
	virtual HRESULT StartCaptureByIndexWithResolutionAndFramerate(int, int, int, int) = 0;
	virtual HRESULT StopCapture() = 0;
	virtual HRESULT ShowPropertyWindow(HWND parentWindowHandle) = 0;
};


#endif //__IVIDEOCAPTURE_H__