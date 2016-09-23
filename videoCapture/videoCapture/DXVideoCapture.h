#include "ISampleGrabber.h"
#include "ISampleGrabberCBImpl.h"
#include "VideoWindow.h"

class DXVideoCapture {
public:
	enum {
		SAMPLE_CALLBACK_MODE = 0,
		BUFFER_CALLBACK_MODE = SAMPLE_CALLBACK_MODE + 1,
	};

	DXVideoCapture(void);
	/*Create the Filter Graph Manager and query for the IMediaControl and IMediaEventEx interfaces.*/
	HRESULT initEnv();
	/*Create an instance of the Sample Grabber filter and add it to the filter graph.*/
	HRESULT createSampleGrabberFilter();
	HRESULT setMediaType();
	HRESULT buildFilterGraph(std::wstring videoFile);
	HRESULT connectToNullRenderFilter();
	HRESULT runWithBufferMode();
	HRESULT setSampleCallback(SampleBufferCallback &callback);
	HRESULT runWithCallbackMode();
	HRESULT grabSample(std::wstring bmpFileName);
	void releaseResources();

	HRESULT connectToVideoWindow(IVideoWindow *videoWindow);
	HRESULT NotifyOwnerMessage(HWND hWnd, long msg, LONG_PTR wParam, LONG_PTR lParam);

protected:
	// Connect filter to filter
	HRESULT ConnectFilters(IGraphBuilder *pGraph, IBaseFilter *pSrc, IBaseFilter *pDest);
	// Connect filter to input pin.
	HRESULT ConnectFilters(IGraphBuilder *pGraph, IBaseFilter *pSrc, IPin *pIn);
	// Connect output pin to filter.
	HRESULT ConnectFilters(IGraphBuilder *pGraph, IPin *pOut/* Output pin on the upstream filter.*/, IBaseFilter *pDest/* Downstream filter.*/);

	// Return the first unconnected input pin or output pin.
	HRESULT FindUnconnectedPin(IBaseFilter *pFilter, PIN_DIRECTION PinDir, IPin **ppPin);
	// Match a pin by pin direction and connection state.
	HRESULT MatchPin(IPin *pPin, PIN_DIRECTION direction, BOOL bShouldBeConnected, BOOL *pResult);
	// Query whether a pin is connected to another pin.
	//
	// Note: This function does not return a pointer to the connected pin.
	HRESULT IsPinConnected(IPin *pPin, BOOL *pResult);
	// Query whether a pin has a specified direction (input / output)
	HRESULT IsPinDirection(IPin *pPin, PIN_DIRECTION dir, BOOL *pResult);
	// Release the format block for a media type.
	void FreeMediaType(AM_MEDIA_TYPE& mt);

	

private:
	/* call back */
	ISampleGrabberCBImpl *sample_cb;

	/* graph */
	IGraphBuilder *pGraphBuilder = NULL;
	IMediaControl *pMediaControl = NULL;
	IMediaEventEx *pMediaEvent = NULL;

	/* sample grabber interfaces */
	IBaseFilter *pSampleGrabber = NULL;
	ISampleGrabber *pGrabber = NULL;

	/* filter graph */
	IBaseFilter *pSourceF = NULL;
	IEnumPins *pEnum = NULL;
	IPin *pPin = NULL;

	/* null render filter */
	IBaseFilter *pNullF = NULL;

	/* buffer */
	BYTE *pBuffer;

	/* window */
	DXVideoWindows *videoWindow;
};