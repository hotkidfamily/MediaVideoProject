#ifndef __IVIDEOCAPTURE_H__
#define __IVIDEOCAPTURE_H__

#ifdef VIDEOCAPTURE_EXPORTS
#define VIDEOCAPTURE_API __declspec(dllexport)
#else
#define VIDEOCAPTURE_API __declspec(dllimport)
#endif

#include <stdint.h>
#include <vector>

// capture output format
#undef FMT
#define FMT(X,Y) PIXEL_FORMAT_##X=Y
enum CPPixelFormat{
	FMT(I420, '024I'),
	FMT(RGB565, 0xe436eb7c),
	FMT(RGB555, 0xe436eb7b),
	FMT(RGB24, 0xe436eb7d),
	FMT(RGB32, 0xe436eb7e),
	FMT(ARGB, 0x773c9ac0),
	FMT(YV12, '21VY'), //YVU 4:2:0 planar == YUV420p
	FMT(NV12, '21VN'), //YUV 4:2:0, with one y plane and one packed u+v
	FMT(IYUV, 'VYUI'),
	FMT(MJPEG, 'GPJM'),
	FMT(YUYV, 'VYUY'),
	FMT(YVU9, '9UVY'),
	FMT(Y411, '114Y'),
	FMT(Y41P, 'P14Y'),
	FMT(YUY2, '2YUY'), //YUV 4:2:2, with y+u+y+v packed
	FMT(YVYU, 'UYVY'), //YUV 4:2:2, with Y+V+Y+U packed top bottom
	FMT(UYVY, 'YVYU'),
	FMT(Y211, '112Y'),
	FMT(NV11, '11VN'),
	FMT(420O, 'O024'),
	FMT(AYUV, 0x56555941)
};
#undef FMT

//---------------------------------------------------------
// 
//
//---------------------------------------------------------
typedef std::vector<const TCHAR*> VECT;

typedef struct tagDevParam{
	int32_t index;
	int32_t width;
	int32_t height;
	double fps; // number? 
	HWND parentWindow;
	HWND attachWindow;
	DWORD pixelFormatInFourCC;

	tagDevParam(){
		ZeroMemory(this, sizeof(struct tagDevParam));
	}
}OPEN_DEVICE_PARAM;

//---------------------------------------------------------
// video call back interface
//---------------------------------------------------------
typedef enum tagEventIndex
{
	OPENSUCCESS_EVENT,
	PLUGINOUT_EVENT,
}EVENT_INDEX;

typedef struct tagEventParams{
	void *param1;
	void *param2;
}EVENT_CONTEXT;

typedef struct tagFrameDesc{
	int32_t cbSize;
	int32_t width;
	int32_t height;
	int32_t dataSize;
	uint8_t *dataPtr;
	DWORD pixelFormatInFourCC;
	int32_t lineSize;
	int64_t ptsStart;
	int64_t ptsEnd;
	int64_t frameStartIdx;
	int64_t frameEndIdx;
	tagFrameDesc(){
		ZeroMemory(this, sizeof(struct tagFrameDesc));
		cbSize = sizeof(struct tagFrameDesc);
	}
}FRAME_DESC;

class VideoCaptureCallback
{
protected:
	~VideoCaptureCallback(){};

public:
	virtual void OnFrame(FRAME_DESC) = 0;
	virtual void OnEvent(EVENT_INDEX, EVENT_CONTEXT) = 0;
};

//---------------------------------------------------------
// capture interface 
//---------------------------------------------------------
class IVideoCapture
{
protected:
	virtual ~IVideoCapture(){};

public:	
	// step 0. register call back function, free call back yourself
	virtual void RegisterCallback(VideoCaptureCallback *) = 0;
	// step 1, get device list
	virtual HRESULT GetDeviceList(VECT &) = 0;
	// step 2, start capture
	virtual HRESULT StartCaptureWithParam(OPEN_DEVICE_PARAM&) = 0;
	
	// step 3.x other feature support: show property setting window
	virtual HRESULT ShowPropertyWindow(HWND parentWindowHandle) = 0;

	// step 4, close capture
	virtual HRESULT StopCapture() = 0;
	// step 5, release callback
	virtual HRESULT UnRegisterCallback() = 0;
};


#ifdef __cplusplus
extern "C" {
#endif

	typedef void(*PVIDEO_CAPTURE_LOG_CALLBACK_FUNC)(int, const char* format, va_list);
	VIDEOCAPTURE_API void SetLogCallback(PVIDEO_CAPTURE_LOG_CALLBACK_FUNC);

	VIDEOCAPTURE_API IVideoCapture *GetVideoCaptureObj();
	VIDEOCAPTURE_API void ReleaseVideoCaptureObj(IVideoCapture *);

#ifdef __cplusplus
}
#endif

#endif //__IVIDEOCAPTURE_H__