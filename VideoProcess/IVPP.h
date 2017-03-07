#pragma once

#ifdef VIDEOPROCESS_EXPORTS
#define VIDEOPROCESS_API __declspec(dllexport)
#else
#define VIDEOPROCESS_API __declspec(dllimport)
#endif

typedef struct tagframedesc{
	int width;
	int height;
	DWORD pixelFormat;
	int colorSpace;
	int colorRange;

	tagframedesc(){
		width = 0;
		height = 0;
		pixelFormat = 0;
		colorSpace = 0;
		colorRange = 1; /*0~255*/
	}

	inline int Width() { return abs(width); }
	inline int Height() { return abs(height); }

	bool operator == (struct tagframedesc &in_desc){
		return ((width == in_desc.width)
			&& (height == in_desc.height)
			&& (pixelFormat == in_desc.pixelFormat)
			&& (colorSpace == in_desc.colorSpace)
			&& (colorRange == in_desc.colorRange));
	}
}FRAMECOLORDESC;

typedef struct IVPPContext{
	FRAMECOLORDESC inDesc;
	FRAMECOLORDESC outDesc;
	int32_t flags;
}IVPPPARAMETER, *PIVPPPARAMETER;

class IVPP {
protected:
	~IVPP(){};

public:
	virtual BOOL InitContext(IVPPPARAMETER) = 0;
	virtual BOOL DeinitContext() = 0;

	virtual BOOL ProcessFrame(const VideoSampleBuffer *, VideoSampleBuffer *) = 0;
};

class IVPPFactory
{
protected:
	~IVPPFactory(){};

public:
	virtual BOOL CreateVPPObj(IVPP *&) = 0;
	virtual void DestoryVPPObj(IVPP *) = 0;
};

#ifdef __cplusplus
extern "C" {
#endif

	VIDEOPROCESS_API BOOL GetVPPFactoryObj(IVPPFactory *&);
	VIDEOPROCESS_API BOOL ReleaseVPPFactoryObj(IVPPFactory *);

#ifdef __cplusplus
}
#endif
