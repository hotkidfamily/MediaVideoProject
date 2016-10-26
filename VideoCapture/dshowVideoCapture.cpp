class DShowVideoCapture
{
public:
	DShowVideoCapture();
	~DShowVideoCapture();
	HRESULT GetDShowInterfaces();
	HRESULT BuildGraph();
	HRESULT EnumCaptureDevices();

private:
	IGraphBuilder *mGraph;
	ICaptureGraphBuilder2 *mGraphBuiler;
	IBaseFilter *mRenderFiler;
	IBaseFilter *mCaptureFilter;

	IBaseFilter *mGrabberFiler;
	ISampleGrabber *mGrabber;

};

DShowVideoCapture::DShowVideoCapture()
{

}

DShowVideoCapture::~DShowVideoCapture()
{

}

HRESULT DShowVideoCapture::GetDShowInterfaces()
{
	HRESULT hr = S_OK;
	return hr;
}

HRESULT DShowVideoCapture::BuildGraph()
{
	HRESULT hr = S_OK;
	return hr;
}

HRESULT DShowVideoCapture::EnumCaptureDevices()
{
	HRESULT hr = S_OK;
	return hr;
}