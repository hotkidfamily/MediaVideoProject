#pragma once

#ifdef VIDEOPROCESS_EXPORTS
#define VIDEOPROCESS_API __declspec(dllexport)
#else
#define VIDEOPROCESS_API __declspec(dllimport)
#endif

class IVPP {
public:
	virtual BOOL open() = 0;
	virtual BOOL close() = 0;

	virtual BOOL process() = 0;

protected:
	~IVPP(){};
};

extern "C" {

	VIDEOPROCESS_API IVPP *GetVPPObj();
	VIDEOPROCESS_API void ReleaseVPPObj();

}
