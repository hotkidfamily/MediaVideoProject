//------------------------------------------------------------------------------
// File: MType.cpp
//
// Desc: DirectShow base classes - implements a class that holds and 
//       manages media type information.
//
// Copyright (c) 1992-2001 Microsoft Corporation.  All rights reserved.
//------------------------------------------------------------------------------


// helper class that derived pin objects can use to compare media
// types etc. Has same data members as the struct AM_MEDIA_TYPE defined
// in the streams IDL file, but also has (non-virtual) functions

//#include <streams.h>
#include <DShow.h>
#include <mmreg.h>
#include <dvdmedia.h>
#include "videoinputlog.h"
#include "mtype.h"

typedef struct tagGuidPair{
	GUID id;
	char * name;
}GUIDSTRING;

#define tostr(x) (#x)

GUIDSTRING supportList[] = {
	{ MEDIASUBTYPE_None, tostr(MEDIASUBTYPE_None) },
	{ MEDIATYPE_Video, tostr(MEDIATYPE_Video) },
	{ MEDIATYPE_Audio, tostr(MEDIATYPE_Audio) },
	{ MEDIATYPE_Text, tostr(MEDIATYPE_Text) },
	{ MEDIATYPE_Midi, tostr(MEDIATYPE_Midi) },
	{ MEDIATYPE_Stream, tostr(MEDIATYPE_Stream) },
	{ MEDIATYPE_Interleaved, tostr(MEDIATYPE_Interleaved) },
	{ MEDIATYPE_File, tostr(MEDIATYPE_File) },
	{ MEDIATYPE_ScriptCommand, tostr(MEDIATYPE_ScriptCommand) },
	{ MEDIATYPE_AUXLine21Data, tostr(MEDIATYPE_AUXLine21Data) },
// 	{ MEDIATYPE_AUXTeletextPage, tostr(MEDIATYPE_AUXTeletextPage) },
// 	{ MEDIATYPE_CC_CONTAINER, tostr(MEDIATYPE_CC_CONTAINER) },
	{ MEDIATYPE_DTVCCData, tostr(MEDIATYPE_DTVCCData) },
	{ MEDIATYPE_MSTVCaption, tostr(MEDIATYPE_MSTVCaption) },
	{ MEDIATYPE_VBI, tostr(MEDIATYPE_VBI) },
// 	{ MEDIASUBTYPE_DVB_SUBTITLES, tostr(MEDIASUBTYPE_DVB_SUBTITLES) },
// 	{ MEDIASUBTYPE_ISDB_CAPTIONS, tostr(MEDIASUBTYPE_ISDB_CAPTIONS) },
// 	{ MEDIASUBTYPE_ISDB_SUPERIMPOSE, tostr(MEDIASUBTYPE_ISDB_SUPERIMPOSE) },
	{ MEDIATYPE_Timecode, tostr(MEDIATYPE_Timecode) },
	{ MEDIATYPE_LMRT, tostr(MEDIATYPE_LMRT) },
	{ MEDIATYPE_URL_STREAM, tostr(MEDIATYPE_URL_STREAM) },
	{ MEDIASUBTYPE_CLPL, tostr(MEDIASUBTYPE_CLPL) },
	{ MEDIASUBTYPE_YUYV, tostr(MEDIASUBTYPE_YUYV) },
	{ MEDIASUBTYPE_IYUV, tostr(MEDIASUBTYPE_IYUV) },
	{ MEDIASUBTYPE_YVU9, tostr(MEDIASUBTYPE_YVU9) },
	{ MEDIASUBTYPE_Y411, tostr(MEDIASUBTYPE_Y411) },
	{ MEDIASUBTYPE_Y41P, tostr(MEDIASUBTYPE_Y41P) },
	{ MEDIASUBTYPE_YUY2, tostr(MEDIASUBTYPE_YUY2) },
	{ MEDIASUBTYPE_YVYU, tostr(MEDIASUBTYPE_YVYU) },
	{ MEDIASUBTYPE_UYVY, tostr(MEDIASUBTYPE_UYVY) },
	{ MEDIASUBTYPE_Y211, tostr(MEDIASUBTYPE_Y211) },
	{ MEDIASUBTYPE_CLJR, tostr(MEDIASUBTYPE_CLJR) },
	{ MEDIASUBTYPE_IF09, tostr(MEDIASUBTYPE_IF09) },
	{ MEDIASUBTYPE_CPLA, tostr(MEDIASUBTYPE_CPLA) },
	{ MEDIASUBTYPE_MJPG, tostr(MEDIASUBTYPE_MJPG) },
	{ MEDIASUBTYPE_TVMJ, tostr(MEDIASUBTYPE_TVMJ) },
	{ MEDIASUBTYPE_WAKE, tostr(MEDIASUBTYPE_WAKE) },
	{ MEDIASUBTYPE_CFCC, tostr(MEDIASUBTYPE_CFCC) },
	{ MEDIASUBTYPE_IJPG, tostr(MEDIASUBTYPE_IJPG) },
	{ MEDIASUBTYPE_Plum, tostr(MEDIASUBTYPE_Plum) },
	{ MEDIASUBTYPE_DVCS, tostr(MEDIASUBTYPE_DVCS) },
	{ MEDIASUBTYPE_H264, tostr(MEDIASUBTYPE_H264) },
	{ MEDIASUBTYPE_DVSD, tostr(MEDIASUBTYPE_DVSD) },
	{ MEDIASUBTYPE_MDVF, tostr(MEDIASUBTYPE_MDVF) },
	{ MEDIASUBTYPE_RGB1, tostr(MEDIASUBTYPE_RGB1) },
	{ MEDIASUBTYPE_RGB4, tostr(MEDIASUBTYPE_RGB4) },
	{ MEDIASUBTYPE_RGB8, tostr(MEDIASUBTYPE_RGB8) },
	{ MEDIASUBTYPE_RGB565, tostr(MEDIASUBTYPE_RGB565) },
	{ MEDIASUBTYPE_RGB555, tostr(MEDIASUBTYPE_RGB555) },
	{ MEDIASUBTYPE_RGB24, tostr(MEDIASUBTYPE_RGB24) },
	{ MEDIASUBTYPE_RGB32, tostr(MEDIASUBTYPE_RGB32) },
	{ MEDIASUBTYPE_ARGB1555, tostr(MEDIASUBTYPE_ARGB1555) },
	{ MEDIASUBTYPE_ARGB4444, tostr(MEDIASUBTYPE_ARGB4444) },
	{ MEDIASUBTYPE_ARGB32, tostr(MEDIASUBTYPE_ARGB32) },
	{ MEDIASUBTYPE_A2R10G10B10, tostr(MEDIASUBTYPE_A2R10G10B10) },
	{ MEDIASUBTYPE_A2B10G10R10, tostr(MEDIASUBTYPE_A2B10G10R10) },
	{ MEDIASUBTYPE_AYUV, tostr(MEDIASUBTYPE_AYUV) },
	{ MEDIASUBTYPE_AI44, tostr(MEDIASUBTYPE_AI44) },
	{ MEDIASUBTYPE_IA44, tostr(MEDIASUBTYPE_IA44) },
	{ MEDIASUBTYPE_RGB32_D3D_DX7_RT, tostr(MEDIASUBTYPE_RGB32_D3D_DX7_RT) },
	{ MEDIASUBTYPE_RGB16_D3D_DX7_RT, tostr(MEDIASUBTYPE_RGB16_D3D_DX7_RT) },
	{ MEDIASUBTYPE_ARGB32_D3D_DX7_RT, tostr(MEDIASUBTYPE_ARGB32_D3D_DX7_RT) },
	{ MEDIASUBTYPE_ARGB4444_D3D_DX7_RT, tostr(MEDIASUBTYPE_ARGB4444_D3D_DX7_RT) },
	{ MEDIASUBTYPE_ARGB1555_D3D_DX7_RT, tostr(MEDIASUBTYPE_ARGB1555_D3D_DX7_RT) },
	{ MEDIASUBTYPE_RGB32_D3D_DX9_RT, tostr(MEDIASUBTYPE_RGB32_D3D_DX9_RT) },
	{ MEDIASUBTYPE_RGB16_D3D_DX9_RT, tostr(MEDIASUBTYPE_RGB16_D3D_DX9_RT) },
	{ MEDIASUBTYPE_ARGB32_D3D_DX9_RT, tostr(MEDIASUBTYPE_ARGB32_D3D_DX9_RT) },
	{ MEDIASUBTYPE_ARGB4444_D3D_DX9_RT, tostr(MEDIASUBTYPE_ARGB4444_D3D_DX9_RT) },
	{ MEDIASUBTYPE_ARGB1555_D3D_DX9_RT, tostr(MEDIASUBTYPE_ARGB1555_D3D_DX9_RT) },
	{ MEDIASUBTYPE_YV12, tostr(MEDIASUBTYPE_YV12) },
	{ MEDIASUBTYPE_NV12, tostr(MEDIASUBTYPE_NV12) },
//	{ MEDIASUBTYPE_NV11, tostr(MEDIASUBTYPE_NV11) },
// 	{ MEDIASUBTYPE_P208, tostr(MEDIASUBTYPE_P208) },
// 	{ MEDIASUBTYPE_P210, tostr(MEDIASUBTYPE_P210) },
// 	{ MEDIASUBTYPE_P216, tostr(MEDIASUBTYPE_P216) },
// 	{ MEDIASUBTYPE_P010, tostr(MEDIASUBTYPE_P010) },
// 	{ MEDIASUBTYPE_P016, tostr(MEDIASUBTYPE_P016) },
// 	{ MEDIASUBTYPE_Y210, tostr(MEDIASUBTYPE_Y210) },
// 	{ MEDIASUBTYPE_Y216, tostr(MEDIASUBTYPE_Y216) },
// 	{ MEDIASUBTYPE_P408, tostr(MEDIASUBTYPE_P408) },
	{ MEDIASUBTYPE_NV24, tostr(MEDIASUBTYPE_NV24) },
// 	{ MEDIASUBTYPE_420O, tostr(MEDIASUBTYPE_420O) },
	{ MEDIASUBTYPE_IMC1, tostr(MEDIASUBTYPE_IMC1) },
	{ MEDIASUBTYPE_IMC2, tostr(MEDIASUBTYPE_IMC2) },
	{ MEDIASUBTYPE_IMC3, tostr(MEDIASUBTYPE_IMC3) },
	{ MEDIASUBTYPE_IMC4, tostr(MEDIASUBTYPE_IMC4) },
	{ MEDIASUBTYPE_S340, tostr(MEDIASUBTYPE_S340) },
	{ MEDIASUBTYPE_S342, tostr(MEDIASUBTYPE_S342) },
	{ MEDIASUBTYPE_Overlay, tostr(MEDIASUBTYPE_Overlay) },
	{ MEDIASUBTYPE_MPEG1Packet, tostr(MEDIASUBTYPE_MPEG1Packet) },
	{ MEDIASUBTYPE_MPEG1Payload, tostr(MEDIASUBTYPE_MPEG1Payload) },
	{ MEDIASUBTYPE_MPEG1AudioPayload, tostr(MEDIASUBTYPE_MPEG1AudioPayload) },
	{ MEDIATYPE_MPEG1SystemStream, tostr(MEDIATYPE_MPEG1SystemStream) },
	{ MEDIASUBTYPE_MPEG1System, tostr(MEDIASUBTYPE_MPEG1System) },
	{ MEDIASUBTYPE_MPEG1VideoCD, tostr(MEDIASUBTYPE_MPEG1VideoCD) },
	{ MEDIASUBTYPE_MPEG1Video, tostr(MEDIASUBTYPE_MPEG1Video) },
	{ MEDIASUBTYPE_MPEG1Audio, tostr(MEDIASUBTYPE_MPEG1Audio) },
	{ MEDIASUBTYPE_Avi, tostr(MEDIASUBTYPE_Avi) },
	{ MEDIASUBTYPE_Asf, tostr(MEDIASUBTYPE_Asf) },
	{ MEDIASUBTYPE_QTMovie, tostr(MEDIASUBTYPE_QTMovie) },
	{ MEDIASUBTYPE_QTRpza, tostr(MEDIASUBTYPE_QTRpza) },
	{ MEDIASUBTYPE_QTSmc, tostr(MEDIASUBTYPE_QTSmc) },
	{ MEDIASUBTYPE_QTRle, tostr(MEDIASUBTYPE_QTRle) },
	{ MEDIASUBTYPE_QTJpeg, tostr(MEDIASUBTYPE_QTJpeg) },
	{ MEDIASUBTYPE_PCMAudio_Obsolete, tostr(MEDIASUBTYPE_PCMAudio_Obsolete) },
	{ MEDIASUBTYPE_PCM, tostr(MEDIASUBTYPE_PCM) },
	{ MEDIASUBTYPE_WAVE, tostr(MEDIASUBTYPE_WAVE) },
	{ MEDIASUBTYPE_AU, tostr(MEDIASUBTYPE_AU) },
	{ MEDIASUBTYPE_AIFF, tostr(MEDIASUBTYPE_AIFF) },
	{ MEDIASUBTYPE_dvsd, tostr(MEDIASUBTYPE_dvsd) },
	{ MEDIASUBTYPE_dvhd, tostr(MEDIASUBTYPE_dvhd) },
	{ MEDIASUBTYPE_dvsl, tostr(MEDIASUBTYPE_dvsl) },
	{ MEDIASUBTYPE_dv25, tostr(MEDIASUBTYPE_dv25) },
	{ MEDIASUBTYPE_dv50, tostr(MEDIASUBTYPE_dv50) },
	{ MEDIASUBTYPE_dvh1, tostr(MEDIASUBTYPE_dvh1) },
	{ MEDIASUBTYPE_Line21_BytePair, tostr(MEDIASUBTYPE_Line21_BytePair) },
	{ MEDIASUBTYPE_Line21_GOPPacket, tostr(MEDIASUBTYPE_Line21_GOPPacket) },
	{ MEDIASUBTYPE_Line21_VBIRawData, tostr(MEDIASUBTYPE_Line21_VBIRawData) },
	{ MEDIASUBTYPE_708_608Data, tostr(MEDIASUBTYPE_708_608Data) },
	{ MEDIASUBTYPE_DtvCcData, tostr(MEDIASUBTYPE_DtvCcData) },
//	{ MEDIASUBTYPE_CC_CONTAINER, tostr(MEDIASUBTYPE_CC_CONTAINER) },
	{ MEDIASUBTYPE_TELETEXT, tostr(MEDIASUBTYPE_TELETEXT) },
// 	{ MEDIASUBTYPE_VBI, tostr(MEDIASUBTYPE_VBI) },
	{ MEDIASUBTYPE_WSS, tostr(MEDIASUBTYPE_WSS) },
// 	{ MEDIASUBTYPE_XDS, tostr(MEDIASUBTYPE_XDS) },
	{ MEDIASUBTYPE_VPS, tostr(MEDIASUBTYPE_VPS) },
	{ MEDIASUBTYPE_DRM_Audio, tostr(MEDIASUBTYPE_DRM_Audio) },
	{ MEDIASUBTYPE_IEEE_FLOAT, tostr(MEDIASUBTYPE_IEEE_FLOAT) },
	{ MEDIASUBTYPE_DOLBY_AC3_SPDIF, tostr(MEDIASUBTYPE_DOLBY_AC3_SPDIF) },
	{ MEDIASUBTYPE_RAW_SPORT, tostr(MEDIASUBTYPE_RAW_SPORT) },
	{ MEDIASUBTYPE_SPDIF_TAG_241h, tostr(MEDIASUBTYPE_SPDIF_TAG_241h) },
	{ MEDIASUBTYPE_DssVideo, tostr(MEDIASUBTYPE_DssVideo) },
	{ MEDIASUBTYPE_DssAudio, tostr(MEDIASUBTYPE_DssAudio) },
	{ MEDIASUBTYPE_VPVideo, tostr(MEDIASUBTYPE_VPVideo) },
	{ MEDIASUBTYPE_VPVBI, tostr(MEDIASUBTYPE_VPVBI) },
	{ FORMAT_None, tostr(FORMAT_None) },
	{ FORMAT_VideoInfo, tostr(FORMAT_VideoInfo) },
	{ FORMAT_VideoInfo2, tostr(FORMAT_VideoInfo2) },
	{ FORMAT_WaveFormatEx, tostr(FORMAT_WaveFormatEx) },
	{ FORMAT_MPEGVideo, tostr(FORMAT_MPEGVideo) },
	{ FORMAT_MPEGStreams, tostr(FORMAT_MPEGStreams) },
	{ FORMAT_DvInfo, tostr(FORMAT_DvInfo) },
	{ FORMAT_525WSS, tostr(FORMAT_525WSS) },
};

#undef tostr

CMediaType::~CMediaType(){
    FreeMediaType(*this);
}


CMediaType::CMediaType()
{
    InitMediaType();
}


CMediaType::CMediaType(const GUID * type)
{
    InitMediaType();
    majortype = *type;
}


// copy constructor does a deep copy of the format block

CMediaType::CMediaType(const AM_MEDIA_TYPE& rt, __out_opt HRESULT* phr)
{
    HRESULT hr = CopyMediaType(this, &rt);
    if (FAILED(hr) && (NULL != phr)) {
        *phr = hr;
    }
}


CMediaType::CMediaType(const CMediaType& rt, __out_opt HRESULT* phr)
{
    HRESULT hr = CopyMediaType(this, &rt);
    if (FAILED(hr) && (NULL != phr)) {
        *phr = hr;
    }
}


// this class inherits publicly from AM_MEDIA_TYPE so the compiler could generate
// the following assignment operator itself, however it could introduce some
// memory conflicts and leaks in the process because the structure contains
// a dynamically allocated block (pbFormat) which it will not copy correctly

CMediaType&
CMediaType::operator=(const AM_MEDIA_TYPE& rt)
{
    Set(rt);
    return *this;
}

CMediaType&
CMediaType::operator=(const CMediaType& rt)
{
    *this = (AM_MEDIA_TYPE &) rt;
    return *this;
}

BOOL
CMediaType::operator == (const CMediaType& rt) const
{
    // I don't believe we need to check sample size or
    // temporal compression flags, since I think these must
    // be represented in the type, subtype and format somehow. They
    // are pulled out as separate flags so that people who don't understand
    // the particular format representation can still see them, but
    // they should duplicate information in the format block.

    return ((IsEqualGUID(majortype,rt.majortype) == TRUE) &&
        (IsEqualGUID(subtype,rt.subtype) == TRUE) &&
        (IsEqualGUID(formattype,rt.formattype) == TRUE) &&
        (cbFormat == rt.cbFormat) &&
        ( (cbFormat == 0) ||
          pbFormat != NULL && rt.pbFormat != NULL &&
          (memcmp(pbFormat, rt.pbFormat, cbFormat) == 0)));
}


BOOL
CMediaType::operator != (const CMediaType& rt) const
{
    /* Check to see if they are equal */

    if (*this == rt) {
        return FALSE;
    }
    return TRUE;
}


HRESULT
CMediaType::Set(const CMediaType& rt)
{
    return Set((AM_MEDIA_TYPE &) rt);
}


HRESULT
CMediaType::Set(const AM_MEDIA_TYPE& rt)
{
    if (&rt != this) {
        FreeMediaType(*this);
        HRESULT hr = CopyMediaType(this, &rt);
        if (FAILED(hr)) {
            return E_OUTOFMEMORY;
        }
    }

    return S_OK;    
}


BOOL
CMediaType::IsValid() const
{
    return (!IsEqualGUID(majortype,GUID_NULL));
}


void
CMediaType::SetType(const GUID* ptype)
{
    majortype = *ptype;
}


void
CMediaType::SetSubtype(const GUID* ptype)
{
    subtype = *ptype;
}

BOOL 
CMediaType::subTypeEqual(const GUID type)
{
	return (IsEqualGUID(*Subtype(), type) > 0);
}


BOOL 
CMediaType::isVideoInfoHeader() 
{
	return IsEqualGUID(formattype, FORMAT_VideoInfo);
}


BOOL 
CMediaType::isVideoInfo2Header() 
{
	return IsEqualGUID(formattype, FORMAT_VideoInfo2);
}


VIDEOINFOHEADER* 
CMediaType::VideoInfoHeader() 
{ 
	if(isVideoInfoHeader())
		return reinterpret_cast<VIDEOINFOHEADER*>(pbFormat);
	else
		return NULL;
}


VIDEOINFOHEADER2* 
CMediaType::VideoInfo2Header() 
{ 
	if(isVideoInfo2Header())
		return reinterpret_cast<VIDEOINFOHEADER2*>(pbFormat);
	else
		return NULL;
}


BITMAPINFOHEADER *
CMediaType::BitmapHeader() 
{
	BITMAPINFOHEADER *pVh = NULL;

	if( isVideoInfoHeader() ){
		pVh = &(VideoInfoHeader()->bmiHeader);
	}else if(isVideoInfo2Header()){
		pVh = &(VideoInfo2Header()->bmiHeader);
	}

	return pVh;
}


REFERENCE_TIME 
CMediaType::AvgReferenceTime()
{
	REFERENCE_TIME rt = 0;

	if( isVideoInfoHeader() ){
		rt = VideoInfoHeader()->AvgTimePerFrame;
	}else if(isVideoInfo2Header()){
		rt = VideoInfo2Header()->AvgTimePerFrame;
	}

	return rt;
}


double 
CMediaType::FrameRate()
{
	double fps = 0.1;

	REFERENCE_TIME rt = AvgReferenceTime();
	if(rt){
		fps = 10000000.0/rt;
	}

	return fps;
}


void
CMediaType::SetAvgReferenceTime(REFERENCE_TIME rt)
{
	if( isVideoInfoHeader() ){
		VideoInfoHeader()->AvgTimePerFrame = rt;
	}else {
		VideoInfo2Header()->AvgTimePerFrame = rt;
	}
}


ULONG
CMediaType::GetSampleSize() const {
    if (IsFixedSize()) {
        return lSampleSize;
    } else {
        return 0;
    }
}


void
CMediaType::SetSampleSize(ULONG sz) {
    if (sz == 0) {
        SetVariableSize();
    } else {
        bFixedSizeSamples = TRUE;
        lSampleSize = sz;
    }
}


void
CMediaType::SetVariableSize() {
    bFixedSizeSamples = FALSE;
}


void
CMediaType::SetTemporalCompression(BOOL bCompressed) {
    bTemporalCompression = bCompressed;
}


BOOL
CMediaType::SetFormat(__in_bcount(cb) BYTE * pformat, ULONG cb)
{
    if (NULL == AllocFormatBuffer(cb))
	return(FALSE);

    assert(pbFormat);
    memcpy(pbFormat, pformat, cb);
    return(TRUE);
}


// set the type of the media type format block, this type defines what you
// will actually find in the format pointer. For example FORMAT_VideoInfo or
// FORMAT_WaveFormatEx. In the future this may be an interface pointer to a
// property set. Before sending out media types this should be filled in.

void
CMediaType::SetFormatType(const GUID *pformattype)
{
    formattype = *pformattype;
}


// reset the format buffer

void CMediaType::ResetFormatBuffer()
{
    if (cbFormat) {
        CoTaskMemFree((PVOID)pbFormat);
    }
    cbFormat = 0;
    pbFormat = NULL;
}


// allocate length bytes for the format and return a read/write pointer
// If we cannot allocate the new block of memory we return NULL leaving
// the original block of memory untouched (as does ReallocFormatBuffer)

BYTE*
CMediaType::AllocFormatBuffer(ULONG length)
{
    assert(length);

    // do the types have the same buffer size

    if (cbFormat == length) {
        return pbFormat;
    }

    // allocate the new format buffer

    BYTE *pNewFormat = (PBYTE)CoTaskMemAlloc(length);
    if (pNewFormat == NULL) {
        if (length <= cbFormat) return pbFormat; //reuse the old block anyway.
        return NULL;
    }

    // delete the old format

    if (cbFormat != 0) {
        assert(pbFormat);
        CoTaskMemFree((PVOID)pbFormat);
    }

    cbFormat = length;
    pbFormat = pNewFormat;
    return pbFormat;
}


// reallocate length bytes for the format and return a read/write pointer
// to it. We keep as much information as we can given the new buffer size
// if this fails the original format buffer is left untouched. The caller
// is responsible for ensuring the size of memory required is non zero

BYTE*
CMediaType::ReallocFormatBuffer(ULONG length)
{
    assert(length);

    // do the types have the same buffer size

    if (cbFormat == length) {
        return pbFormat;
    }

    // allocate the new format buffer

    BYTE *pNewFormat = (PBYTE)CoTaskMemAlloc(length);
    if (pNewFormat == NULL) {
        if (length <= cbFormat) return pbFormat; //reuse the old block anyway.
        return NULL;
    }

    // copy any previous format (or part of if new is smaller)
    // delete the old format and replace with the new one

    if (cbFormat != 0) {
        assert(pbFormat);
        memcpy(pNewFormat,pbFormat,min(length,cbFormat));
        CoTaskMemFree((PVOID)pbFormat);
    }

    cbFormat = length;
    pbFormat = pNewFormat;
    return pNewFormat;
}

// initialise a media type structure

void CMediaType::InitMediaType()
{
    ZeroMemory((PVOID)this, sizeof(*this));
    lSampleSize = 1;
    bFixedSizeSamples = TRUE;
}


// a partially specified media type can be passed to IPin::Connect
// as a constraint on the media type used in the connection.
// the type, subtype or format type can be null.
BOOL
CMediaType::IsPartiallySpecified(void) const
{
    if ((majortype == GUID_NULL) ||
        (formattype == GUID_NULL)) {
            return TRUE;
    } else {
        return FALSE;
    }
}

BOOL
CMediaType::MatchesPartial(const CMediaType* ppartial) const
{
    if ((ppartial->majortype != GUID_NULL) &&
        (majortype != ppartial->majortype)) {
            return FALSE;
    }
    if ((ppartial->subtype != GUID_NULL) &&
        (subtype != ppartial->subtype)) {
            return FALSE;
    }

    if (ppartial->formattype != GUID_NULL) {
        // if the format block is specified then it must match exactly
        if (formattype != ppartial->formattype) {
            return FALSE;
        }
        if (cbFormat != ppartial->cbFormat) {
            return FALSE;
        }
        if ((cbFormat != 0) &&
            (memcmp(pbFormat, ppartial->pbFormat, cbFormat) != 0)) {
                return FALSE;
        }
    }

    return TRUE;

}

// this also comes in useful when using the IEnumMediaTypes interface so
// that you can copy a media type, you can do nearly the same by creating
// a CMediaType object but as soon as it goes out of scope the destructor
// will delete the memory it allocated (this takes a copy of the memory)

AM_MEDIA_TYPE * WINAPI CreateMediaType(AM_MEDIA_TYPE const *pSrc)
{
    assert(pSrc);

    // Allocate a block of memory for the media type

    AM_MEDIA_TYPE *pMediaType =
        (AM_MEDIA_TYPE *)CoTaskMemAlloc(sizeof(AM_MEDIA_TYPE));

    if (pMediaType == NULL) {
        return NULL;
    }
    // Copy the variable length format block

    HRESULT hr = CopyMediaType(pMediaType,pSrc);
    if (FAILED(hr)) {
        CoTaskMemFree((PVOID)pMediaType);
        return NULL;
    }

    return pMediaType;
}


//  Copy 1 media type to another

HRESULT WINAPI CopyMediaType(__out AM_MEDIA_TYPE *pmtTarget, const AM_MEDIA_TYPE *pmtSource)
{
    //  We'll leak if we copy onto one that already exists - there's one
    //  case we can check like that - copying to itself.
    assert(pmtSource != pmtTarget);
    *pmtTarget = *pmtSource;
    if (pmtSource->cbFormat != 0) {
        assert(pmtSource->pbFormat != NULL);
        pmtTarget->pbFormat = (PBYTE)CoTaskMemAlloc(pmtSource->cbFormat);
        if (pmtTarget->pbFormat == NULL) {
            pmtTarget->cbFormat = 0;
            return E_OUTOFMEMORY;
        } else {
            CopyMemory((PVOID)pmtTarget->pbFormat, (PVOID)pmtSource->pbFormat,
                       pmtTarget->cbFormat);
        }
    }
    if (pmtTarget->pUnk != NULL) {
        pmtTarget->pUnk->AddRef();
    }

    return S_OK;
}

// general purpose function to delete a heap allocated AM_MEDIA_TYPE structure
// which is useful when calling IEnumMediaTypes::Next as the interface
// implementation allocates the structures which you must later delete
// the format block may also be a pointer to an interface to release

void WINAPI DeleteMediaType(__inout_opt AM_MEDIA_TYPE *pmt)
{
	// allow NULL pointers for coding simplicity

	if (pmt == NULL) {
		return;
	}

	FreeMediaType(*pmt);
	CoTaskMemFree((PVOID)pmt);
}

//  Free an existing media type (ie free resources it holds)
void WINAPI FreeMediaType(__inout AM_MEDIA_TYPE& mt)
{
    if (mt.cbFormat != 0) {
        CoTaskMemFree((PVOID)mt.pbFormat);

        // Strictly unnecessary but tidier
        mt.cbFormat = 0;
        mt.pbFormat = NULL;
    }
    if (mt.pUnk != NULL) {
        mt.pUnk->Release();
        mt.pUnk = NULL;
    }
}

#if 0
//  Initialize a media type from a WAVEFORMATEX

STDAPI CreateAudioMediaType(
    const WAVEFORMATEX *pwfx,
    __out AM_MEDIA_TYPE *pmt,
    BOOL bSetFormat
)
{
    pmt->majortype            = MEDIATYPE_Audio;
    if (pwfx->wFormatTag == WAVE_FORMAT_EXTENSIBLE) {
        pmt->subtype = ((PWAVEFORMATEXTENSIBLE)pwfx)->SubFormat;
    } else {
        pmt->subtype              = FOURCCMap(pwfx->wFormatTag);
    }
    pmt->formattype           = FORMAT_WaveFormatEx;
    pmt->bFixedSizeSamples    = TRUE;
    pmt->bTemporalCompression = FALSE;
    pmt->lSampleSize          = pwfx->nBlockAlign;
    pmt->pUnk                 = NULL;
    if (bSetFormat) {
        if (pwfx->wFormatTag == WAVE_FORMAT_PCM) {
            pmt->cbFormat         = sizeof(WAVEFORMATEX);
        } else {
            pmt->cbFormat         = sizeof(WAVEFORMATEX) + pwfx->cbSize;
        }
        pmt->pbFormat             = (PBYTE)CoTaskMemAlloc(pmt->cbFormat);
        if (pmt->pbFormat == NULL) {
            return E_OUTOFMEMORY;
        }
        if (pwfx->wFormatTag == WAVE_FORMAT_PCM) {
            CopyMemory(pmt->pbFormat, pwfx, sizeof(PCMWAVEFORMAT));
            ((WAVEFORMATEX *)pmt->pbFormat)->cbSize = 0;
        } else {
            CopyMemory(pmt->pbFormat, pwfx, pmt->cbFormat);
        }
    }
    return S_OK;
}
#endif


void
CMediaType::ShowAMMediaType()
{
#if 1
	PrintMessage("    majortype\t%s", GUIDtoStr(*Type()));
	PrintMessage("    subtype\t%s",GUIDtoStr(*Subtype()));
	PrintMessage("    bFixedSizeSamples\t%d\n", bFixedSizeSamples);
	PrintMessage("    bTemporalCompression\t%d\n", bTemporalCompression);
	PrintMessage("    lSampleSize\t%lu\n", lSampleSize);
	PrintMessage("    formattype\t%s",GUIDtoStr(*FormatType()));
	PrintMessage("    pUnk\t%x\n", pUnk);
	PrintMessage("    cbFormat\t%lu\n", cbFormat);
	PrintMessage("    pbFormat\t%x\n", pbFormat);

	if (IsEqualGUID(formattype, FORMAT_VideoInfo)) {
		VIDEOINFOHEADER *v = (VIDEOINFOHEADER *)pbFormat;
		PrintMessage("      rcSource: left %ld top %ld right %ld bottom %ld\n",
			v->rcSource.left, v->rcSource.top, v->rcSource.right, v->rcSource.bottom);
		PrintMessage("      rcTarget: left %ld top %ld right %ld bottom %ld\n",
			v->rcTarget.left, v->rcTarget.top, v->rcTarget.right, v->rcTarget.bottom);
		PrintMessage("      dwBitRate: %lu\n", v->dwBitRate);
		PrintMessage("      dwBitErrorRate: %lu\n", v->dwBitErrorRate);
		PrintMessage("      AvgTimePerFrame: %lld\n", v->AvgTimePerFrame);
		PrintBitmapHeader(&v->bmiHeader);
	} else if (IsEqualGUID(formattype, FORMAT_VideoInfo2)) {
		VIDEOINFOHEADER2 *v = (VIDEOINFOHEADER2 *) pbFormat;
		PrintMessage("      rcSource: left %ld top %ld right %ld bottom %ld\n",
			v->rcSource.left, v->rcSource.top, v->rcSource.right, v->rcSource.bottom);
		PrintMessage("      rcTarget: left %ld top %ld right %ld bottom %ld\n",
			v->rcTarget.left, v->rcTarget.top, v->rcTarget.right, v->rcTarget.bottom);
		PrintMessage("      dwBitRate: %lu\n", v->dwBitRate);
		PrintMessage("      dwBitErrorRate: %lu\n", v->dwBitErrorRate);
		PrintMessage("      AvgTimePerFrame: %lld\n", v->AvgTimePerFrame);
		PrintMessage("      dwInterlaceFlags: %lu\n", v->dwInterlaceFlags);
		PrintMessage("      dwCopyProtectFlags: %lu\n", v->dwCopyProtectFlags);
		PrintMessage("      dwPictAspectRatioX: %lu\n", v->dwPictAspectRatioX);
		PrintMessage("      dwPictAspectRatioY: %lu\n", v->dwPictAspectRatioY);
		//        PrintMessage("      dwReserved1: %lu\n", v->u.dwReserved1); /* mingw-w64 is buggy and doesn't name unnamed unions */
		PrintMessage("      dwReserved2: %lu\n", v->dwReserved2);
		PrintBitmapHeader(&v->bmiHeader);
	} else if (IsEqualGUID(formattype, FORMAT_WaveFormatEx)) {
		WAVEFORMATEX *fx = (WAVEFORMATEX *) pbFormat;
		PrintMessage("      wFormatTag: %u\n", fx->wFormatTag);
		PrintMessage("      nChannels: %u\n", fx->nChannels);
		PrintMessage("      nSamplesPerSec: %lu\n", fx->nSamplesPerSec);
		PrintMessage("      nAvgBytesPerSec: %lu\n", fx->nAvgBytesPerSec);
		PrintMessage("      nBlockAlign: %u\n", fx->nBlockAlign);
		PrintMessage("      wBitsPerSample: %u\n", fx->wBitsPerSample);
		PrintMessage("      cbSize: %u\n", fx->cbSize);
	}
#endif
}


char *CMediaType::GUIDtoStr(GUID id)
{
	int i =0;
	for(; i<ARRAYSIZE(supportList); i++){
		if(IsEqualGUID(supportList[i].id , id)){
			break;
		}
	}

	return supportList[i].name;
}

void CMediaType::PrintGUID(GUID id)
{
	const uint32_t *d = (const uint32_t *) &id.Data1;
	const uint16_t *w = (const uint16_t *) &id.Data2;
	const uint8_t  *c = (const uint8_t  *) &id.Data4;

	log(Info, "           \t0x%08x 0x%04x 0x%04x %02x%02x%02x%02x%02x%02x%02x%02x",
		d[0], w[0], w[1],
		c[0], c[1], c[2], c[3], c[4], c[5], c[6], c[7]);
}


void 
CMediaType::PrintBitmapHeader(BITMAPINFOHEADER *bmpHeader)
{
	PrintMessage("      BITMAPINFOHEADER\n");
	PrintMessage("            biSize:\t %lu\n", bmpHeader->biSize);
	PrintMessage("            biWidth:\t %ld\n", bmpHeader->biWidth);
	PrintMessage("            biHeight:\t %ld\n", bmpHeader->biHeight);
	PrintMessage("            biPlanes:\t %d\n", bmpHeader->biPlanes);
	PrintMessage("            biBitCount:\t %d\n", bmpHeader->biBitCount);
	PrintMessage("            biCompression:\t %x\n", bmpHeader->biCompression);
	PrintMessage("            biSizeImage:\t %lu\n", bmpHeader->biSizeImage);
	PrintMessage("            biXPelsPerMeter:\t %lu\n", bmpHeader->biXPelsPerMeter);
	PrintMessage("            biYPelsPerMeter:\t %lu\n", bmpHeader->biYPelsPerMeter);
	PrintMessage("            biClrUsed:\t %lu\n", bmpHeader->biClrUsed);
	PrintMessage("            biClrImportant:\t %lu\n", bmpHeader->biClrImportant);
}

void 
CMediaType::PrintMessage(const char* format, ...)
{
	char buffer[4096] = "\0";
	va_list vt;
	va_start(vt, format);
	vsprintf_s(buffer, 4096, format, vt);
	va_end(vt);

	log(Info, "%s", buffer);
}

// eliminate very many spurious warnings from MS compiler
#pragma warning(disable:4514)
