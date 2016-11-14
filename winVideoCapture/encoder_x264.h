#include "x264.h"

class CLibx264{
public:
	CLibx264();
	~CLibx264();

protected:

private:
	x264_param_t mCodecParams;
	x264_t *mCodecHandle;
};