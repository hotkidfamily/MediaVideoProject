#include "stdafx.h"
#include "dxUtils.h"

typedef struct tagGuidPair{
	GUID id;
	TCHAR * name;
}GUIDSTRING;

#define tostr(x) TEXT(#x)

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
		{ MEDIATYPE_AUXTeletextPage, tostr(MEDIATYPE_AUXTeletextPage) },
		{ MEDIATYPE_CC_CONTAINER, tostr(MEDIATYPE_CC_CONTAINER) },
		{ MEDIATYPE_DTVCCData, tostr(MEDIATYPE_DTVCCData) },
		{ MEDIATYPE_MSTVCaption, tostr(MEDIATYPE_MSTVCaption) },
		{ MEDIATYPE_VBI, tostr(MEDIATYPE_VBI) },
		{ MEDIASUBTYPE_DVB_SUBTITLES, tostr(MEDIASUBTYPE_DVB_SUBTITLES) },
		{ MEDIASUBTYPE_ISDB_CAPTIONS, tostr(MEDIASUBTYPE_ISDB_CAPTIONS) },
		{ MEDIASUBTYPE_ISDB_SUPERIMPOSE, tostr(MEDIASUBTYPE_ISDB_SUPERIMPOSE) },
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
		{ MEDIASUBTYPE_NV11, tostr(MEDIASUBTYPE_NV11) },
		{ MEDIASUBTYPE_P208, tostr(MEDIASUBTYPE_P208) },
		{ MEDIASUBTYPE_P210, tostr(MEDIASUBTYPE_P210) },
		{ MEDIASUBTYPE_P216, tostr(MEDIASUBTYPE_P216) },
		{ MEDIASUBTYPE_P010, tostr(MEDIASUBTYPE_P010) },
		{ MEDIASUBTYPE_P016, tostr(MEDIASUBTYPE_P016) },
		{ MEDIASUBTYPE_Y210, tostr(MEDIASUBTYPE_Y210) },
		{ MEDIASUBTYPE_Y216, tostr(MEDIASUBTYPE_Y216) },
		{ MEDIASUBTYPE_P408, tostr(MEDIASUBTYPE_P408) },
		{ MEDIASUBTYPE_NV24, tostr(MEDIASUBTYPE_NV24) },
		{ MEDIASUBTYPE_420O, tostr(MEDIASUBTYPE_420O) },
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
		{ MEDIASUBTYPE_CC_CONTAINER, tostr(MEDIASUBTYPE_CC_CONTAINER) },
		{ MEDIASUBTYPE_TELETEXT, tostr(MEDIASUBTYPE_TELETEXT) },
		{ MEDIASUBTYPE_VBI, tostr(MEDIASUBTYPE_VBI) },
		{ MEDIASUBTYPE_WSS, tostr(MEDIASUBTYPE_WSS) },
		{ MEDIASUBTYPE_XDS, tostr(MEDIASUBTYPE_XDS) },
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
		{ CLSID_CaptureGraphBuilder, tostr(CLSID_CaptureGraphBuilder) },
		{ CLSID_CaptureGraphBuilder2, tostr(CLSID_CaptureGraphBuilder2) },
		{ CLSID_ProtoFilterGraph, tostr(CLSID_ProtoFilterGraph) },
		{ CLSID_SystemClock, tostr(CLSID_SystemClock) },
		{ CLSID_FilterMapper, tostr(CLSID_FilterMapper) },
		{ CLSID_FilterGraph, tostr(CLSID_FilterGraph) },
		{ CLSID_FilterGraphNoThread, tostr(CLSID_FilterGraphNoThread) },
		{ CLSID_FilterGraphPrivateThread, tostr(CLSID_FilterGraphPrivateThread) },
		{ CLSID_MPEG1Doc, tostr(CLSID_MPEG1Doc) },
		{ CLSID_FileSource, tostr(CLSID_FileSource) },
		{ CLSID_MPEG1PacketPlayer, tostr(CLSID_MPEG1PacketPlayer) },
		{ CLSID_MPEG1Splitter, tostr(CLSID_MPEG1Splitter) },
		{ CLSID_CMpegVideoCodec, tostr(CLSID_CMpegVideoCodec) },
		{ CLSID_CMpegAudioCodec, tostr(CLSID_CMpegAudioCodec) },
		{ CLSID_TextRender, tostr(CLSID_TextRender) },
		{ CLSID_InfTee, tostr(CLSID_InfTee) },
		{ CLSID_AviSplitter, tostr(CLSID_AviSplitter) },
		{ CLSID_AviReader, tostr(CLSID_AviReader) },
		{ CLSID_VfwCapture, tostr(CLSID_VfwCapture) },
		{ CLSID_CaptureProperties, tostr(CLSID_CaptureProperties) },
		{ CLSID_FGControl, tostr(CLSID_FGControl) },
		{ CLSID_MOVReader, tostr(CLSID_MOVReader) },
		{ CLSID_QuickTimeParser, tostr(CLSID_QuickTimeParser) },
		{ CLSID_QTDec, tostr(CLSID_QTDec) },
		{ CLSID_AVIDoc, tostr(CLSID_AVIDoc) },
		{ CLSID_VideoRenderer, tostr(CLSID_VideoRenderer) },
		{ CLSID_Colour, tostr(CLSID_Colour) },
		{ CLSID_Dither, tostr(CLSID_Dither) },
		{ CLSID_ModexRenderer, tostr(CLSID_ModexRenderer) },
		{ CLSID_AudioRender, tostr(CLSID_AudioRender) },
		{ CLSID_AudioProperties, tostr(CLSID_AudioProperties) },
		{ CLSID_DSoundRender, tostr(CLSID_DSoundRender) },
		{ CLSID_AudioRecord, tostr(CLSID_AudioRecord) },
		{ CLSID_AudioInputMixerProperties, tostr(CLSID_AudioInputMixerProperties) },
		{ CLSID_AVIDec, tostr(CLSID_AVIDec) },
		{ CLSID_AVIDraw, tostr(CLSID_AVIDraw) },
		{ CLSID_ACMWrapper, tostr(CLSID_ACMWrapper) },
		{ CLSID_AsyncReader, tostr(CLSID_AsyncReader) },
		{ CLSID_URLReader, tostr(CLSID_URLReader) },
		{ CLSID_PersistMonikerPID, tostr(CLSID_PersistMonikerPID) },
		{ CLSID_AVICo, tostr(CLSID_AVICo) },
		{ CLSID_FileWriter, tostr(CLSID_FileWriter) },
		{ CLSID_AviDest, tostr(CLSID_AviDest) },
		{ CLSID_AviMuxProptyPage, tostr(CLSID_AviMuxProptyPage) },
		{ CLSID_AviMuxProptyPage1, tostr(CLSID_AviMuxProptyPage1) },
		{ CLSID_AVIMIDIRender, tostr(CLSID_AVIMIDIRender) },
		{ CLSID_WMAsfReader, tostr(CLSID_WMAsfReader) },
		{ CLSID_WMAsfWriter, tostr(CLSID_WMAsfWriter) },
		{ CLSID_MPEG2Demultiplexer, tostr(CLSID_MPEG2Demultiplexer) },
		{ CLSID_MPEG2Demultiplexer_NoClock, tostr(CLSID_MPEG2Demultiplexer_NoClock) },
		{ CLSID_MMSPLITTER, tostr(CLSID_MMSPLITTER) },
		{ CLSID_StreamBufferSink, tostr(CLSID_StreamBufferSink) },
		{ CLSID_SBE2Sink, tostr(CLSID_SBE2Sink) },
		{ CLSID_StreamBufferSource, tostr(CLSID_StreamBufferSource) },
		{ CLSID_StreamBufferConfig, tostr(CLSID_StreamBufferConfig) },
		{ CLSID_StreamBufferPropertyHandler, tostr(CLSID_StreamBufferPropertyHandler) },
		{ CLSID_StreamBufferThumbnailHandler, tostr(CLSID_StreamBufferThumbnailHandler) },
		{ CLSID_Mpeg2VideoStreamAnalyzer, tostr(CLSID_Mpeg2VideoStreamAnalyzer) },
		{ CLSID_StreamBufferRecordingAttributes, tostr(CLSID_StreamBufferRecordingAttributes) },
		{ CLSID_StreamBufferComposeRecording, tostr(CLSID_StreamBufferComposeRecording) },
		{ CLSID_SBE2File, tostr(CLSID_SBE2File) },
		{ CLSID_DVVideoCodec, tostr(CLSID_DVVideoCodec) },
		{ CLSID_DVVideoEnc, tostr(CLSID_DVVideoEnc) },
		{ CLSID_DVSplitter, tostr(CLSID_DVSplitter) },
		{ CLSID_DVMux, tostr(CLSID_DVMux) },
		{ CLSID_SeekingPassThru, tostr(CLSID_SeekingPassThru) },
		{ CLSID_Line21Decoder, tostr(CLSID_Line21Decoder) },
		{ CLSID_Line21Decoder2, tostr(CLSID_Line21Decoder2) },
		{ CLSID_CCAFilter, tostr(CLSID_CCAFilter) },
		{ CLSID_OverlayMixer, tostr(CLSID_OverlayMixer) },
		{ CLSID_VBISurfaces, tostr(CLSID_VBISurfaces) },
		{ CLSID_WSTDecoder, tostr(CLSID_WSTDecoder) },
		{ CLSID_MjpegDec, tostr(CLSID_MjpegDec) },
		{ CLSID_MJPGEnc, tostr(CLSID_MJPGEnc) },
		{ CLSID_SystemDeviceEnum, tostr(CLSID_SystemDeviceEnum) },
		{ CLSID_CDeviceMoniker, tostr(CLSID_CDeviceMoniker) },
		{ CLSID_VideoInputDeviceCategory, tostr(CLSID_VideoInputDeviceCategory) },
		{ CLSID_CVidCapClassManager, tostr(CLSID_CVidCapClassManager) },
		{ CLSID_LegacyAmFilterCategory, tostr(CLSID_LegacyAmFilterCategory) },
		{ CLSID_CQzFilterClassManager, tostr(CLSID_CQzFilterClassManager) },
		{ CLSID_VideoCompressorCategory, tostr(CLSID_VideoCompressorCategory) },
		{ CLSID_CIcmCoClassManager, tostr(CLSID_CIcmCoClassManager) },
		{ CLSID_AudioCompressorCategory, tostr(CLSID_AudioCompressorCategory) },
		{ CLSID_CAcmCoClassManager, tostr(CLSID_CAcmCoClassManager) },
		{ CLSID_AudioInputDeviceCategory, tostr(CLSID_AudioInputDeviceCategory) },
		{ CLSID_CWaveinClassManager, tostr(CLSID_CWaveinClassManager) },
		{ CLSID_AudioRendererCategory, tostr(CLSID_AudioRendererCategory) },
		{ CLSID_CWaveOutClassManager, tostr(CLSID_CWaveOutClassManager) },
		{ CLSID_MidiRendererCategory, tostr(CLSID_MidiRendererCategory) },
		{ CLSID_CMidiOutClassManager, tostr(CLSID_CMidiOutClassManager) },
		{ CLSID_TransmitCategory, tostr(CLSID_TransmitCategory) },
		{ CLSID_DeviceControlCategory, tostr(CLSID_DeviceControlCategory) },
		{ CLSID_ActiveMovieCategories, tostr(CLSID_ActiveMovieCategories) },
		{ CLSID_DVDHWDecodersCategory, tostr(CLSID_DVDHWDecodersCategory) },
		{ CLSID_MediaEncoderCategory, tostr(CLSID_MediaEncoderCategory) },
		{ CLSID_MediaMultiplexerCategory, tostr(CLSID_MediaMultiplexerCategory) },
		{ CLSID_FilterMapper2, tostr(CLSID_FilterMapper2) },
		{ CLSID_MemoryAllocator, tostr(CLSID_MemoryAllocator) },
		{ CLSID_MediaPropertyBag, tostr(CLSID_MediaPropertyBag) },
		{ CLSID_DvdGraphBuilder, tostr(CLSID_DvdGraphBuilder) },
		{ CLSID_DVDNavigator, tostr(CLSID_DVDNavigator) },
		{ CLSID_DVDState, tostr(CLSID_DVDState) },
		{ CLSID_SmartTee, tostr(CLSID_SmartTee) },
		{ CLSID_DtvCcFilter, tostr(CLSID_DtvCcFilter) },
		{ CLSID_CaptionsFilter, tostr(CLSID_CaptionsFilter) },
		{ CLSID_SubtitlesFilter, tostr(CLSID_SubtitlesFilter) },
		{ CLSID_DirectShowPluginControl, tostr(CLSID_DirectShowPluginControl) },
		{ FORMAT_None, tostr(FORMAT_None) },
		{ FORMAT_VideoInfo, tostr(FORMAT_VideoInfo) },
		{ FORMAT_VideoInfo2, tostr(FORMAT_VideoInfo2) },
		{ FORMAT_WaveFormatEx, tostr(FORMAT_WaveFormatEx) },
		{ FORMAT_MPEGVideo, tostr(FORMAT_MPEGVideo) },
		{ FORMAT_MPEGStreams, tostr(FORMAT_MPEGStreams) },
		{ FORMAT_DvInfo, tostr(FORMAT_DvInfo) },
		{ FORMAT_525WSS, tostr(FORMAT_525WSS) },
		{ CLSID_DirectDrawProperties, tostr(CLSID_DirectDrawProperties) },
		{ CLSID_PerformanceProperties, tostr(CLSID_PerformanceProperties) },
		{ CLSID_QualityProperties, tostr(CLSID_QualityProperties) },
		{ IID_IBaseVideoMixer, tostr(IID_IBaseVideoMixer) },
		{ IID_IDirectDrawVideo, tostr(IID_IDirectDrawVideo) },
		{ IID_IQualProp, tostr(IID_IQualProp) },
		{ CLSID_VPObject, tostr(CLSID_VPObject) },
		{ IID_IVPObject, tostr(IID_IVPObject) },
		{ IID_IVPControl, tostr(IID_IVPControl) },
		{ CLSID_VPVBIObject, tostr(CLSID_VPVBIObject) },
		{ IID_IVPVBIObject, tostr(IID_IVPVBIObject) },
		{ IID_IVPConfig, tostr(IID_IVPConfig) },
		{ IID_IVPNotify, tostr(IID_IVPNotify) },
		{ IID_IVPNotify2, tostr(IID_IVPNotify2) },
		{ IID_IVPVBIConfig, tostr(IID_IVPVBIConfig) },
		{ IID_IVPVBINotify, tostr(IID_IVPVBINotify) },
		{ IID_IMixerPinConfig, tostr(IID_IMixerPinConfig) },
		{ IID_IMixerPinConfig2, tostr(IID_IMixerPinConfig2) },
		{ CLSID_DirectDraw, tostr(CLSID_DirectDraw) },
		{ CLSID_DirectDrawClipper, tostr(CLSID_DirectDrawClipper) },
		{ IID_IDirectDraw, tostr(IID_IDirectDraw) },
		{ IID_IDirectDraw2, tostr(IID_IDirectDraw2) },
		{ IID_IDirectDrawSurface, tostr(IID_IDirectDrawSurface) },
		{ IID_IDirectDrawSurface2, tostr(IID_IDirectDrawSurface2) },
		{ IID_IDirectDrawSurface3, tostr(IID_IDirectDrawSurface3) },
		{ IID_IDirectDrawSurface4, tostr(IID_IDirectDrawSurface4) },
		{ IID_IDirectDrawSurface7, tostr(IID_IDirectDrawSurface7) },
		{ IID_IDirectDrawPalette, tostr(IID_IDirectDrawPalette) },
		{ IID_IDirectDrawClipper, tostr(IID_IDirectDrawClipper) },
		{ IID_IDirectDrawColorControl, tostr(IID_IDirectDrawColorControl) },
		{ IID_IDDVideoPortContainer, tostr(IID_IDDVideoPortContainer) },
		{ IID_IDirectDrawKernel, tostr(IID_IDirectDrawKernel) },
		{ IID_IDirectDrawSurfaceKernel, tostr(IID_IDirectDrawSurfaceKernel) },
		{ CLSID_ModexProperties, tostr(CLSID_ModexProperties) },
		{ IID_IFullScreenVideo, tostr(IID_IFullScreenVideo) },
		{ IID_IFullScreenVideoEx, tostr(IID_IFullScreenVideoEx) },
		{ CLSID_DVDecPropertiesPage, tostr(CLSID_DVDecPropertiesPage) },
		{ CLSID_DVEncPropertiesPage, tostr(CLSID_DVEncPropertiesPage) },
		{ CLSID_DVMuxPropertyPage, tostr(CLSID_DVMuxPropertyPage) },
		{ IID_IAMDirectSound, tostr(IID_IAMDirectSound) },
		{ IID_IMpegAudioDecoder, tostr(IID_IMpegAudioDecoder) },
		{ IID_IAMLine21Decoder, tostr(IID_IAMLine21Decoder) },
		{ IID_IAMWstDecoder, tostr(IID_IAMWstDecoder) },
		{ CLSID_WstDecoderPropertyPage, tostr(CLSID_WstDecoderPropertyPage) },
		{ FORMAT_AnalogVideo, tostr(FORMAT_AnalogVideo) },
		{ MEDIATYPE_AnalogVideo, tostr(MEDIATYPE_AnalogVideo) },
		{ MEDIASUBTYPE_AnalogVideo_NTSC_M, tostr(MEDIASUBTYPE_AnalogVideo_NTSC_M) },
		{ MEDIASUBTYPE_AnalogVideo_PAL_B, tostr(MEDIASUBTYPE_AnalogVideo_PAL_B) },
		{ MEDIASUBTYPE_AnalogVideo_PAL_D, tostr(MEDIASUBTYPE_AnalogVideo_PAL_D) },
		{ MEDIASUBTYPE_AnalogVideo_PAL_G, tostr(MEDIASUBTYPE_AnalogVideo_PAL_G) },
		{ MEDIASUBTYPE_AnalogVideo_PAL_H, tostr(MEDIASUBTYPE_AnalogVideo_PAL_H) },
		{ MEDIASUBTYPE_AnalogVideo_PAL_I, tostr(MEDIASUBTYPE_AnalogVideo_PAL_I) },
		{ MEDIASUBTYPE_AnalogVideo_PAL_M, tostr(MEDIASUBTYPE_AnalogVideo_PAL_M) },
		{ MEDIASUBTYPE_AnalogVideo_PAL_N, tostr(MEDIASUBTYPE_AnalogVideo_PAL_N) },
		{ MEDIASUBTYPE_AnalogVideo_PAL_N_COMBO, tostr(MEDIASUBTYPE_AnalogVideo_PAL_N_COMBO) },
		{ MEDIASUBTYPE_AnalogVideo_SECAM_B, tostr(MEDIASUBTYPE_AnalogVideo_SECAM_B) },
		{ MEDIASUBTYPE_AnalogVideo_SECAM_D, tostr(MEDIASUBTYPE_AnalogVideo_SECAM_D) },
		{ MEDIASUBTYPE_AnalogVideo_SECAM_G, tostr(MEDIASUBTYPE_AnalogVideo_SECAM_G) },
		{ MEDIASUBTYPE_AnalogVideo_SECAM_H, tostr(MEDIASUBTYPE_AnalogVideo_SECAM_H) },
		{ MEDIASUBTYPE_AnalogVideo_SECAM_K, tostr(MEDIASUBTYPE_AnalogVideo_SECAM_K) },
		{ MEDIASUBTYPE_AnalogVideo_SECAM_K1, tostr(MEDIASUBTYPE_AnalogVideo_SECAM_K1) },
		{ MEDIASUBTYPE_AnalogVideo_SECAM_L, tostr(MEDIASUBTYPE_AnalogVideo_SECAM_L) },
		{ MEDIATYPE_AnalogAudio, tostr(MEDIATYPE_AnalogAudio) },
		{ FORMAT_CAPTIONED_H264VIDEO, tostr(FORMAT_CAPTIONED_H264VIDEO) },
		{ FORMAT_CC_CONTAINER, tostr(FORMAT_CC_CONTAINER) },
		{ CAPTION_FORMAT_ATSC, tostr(CAPTION_FORMAT_ATSC) },
		{ CAPTION_FORMAT_DVB, tostr(CAPTION_FORMAT_DVB) },
		{ CAPTION_FORMAT_DIRECTV, tostr(CAPTION_FORMAT_DIRECTV) },
		{ CAPTION_FORMAT_ECHOSTAR, tostr(CAPTION_FORMAT_ECHOSTAR) },
		{ FORMAT_CAPTIONED_MPEG2VIDEO, tostr(FORMAT_CAPTIONED_MPEG2VIDEO) },
		{ TIME_FORMAT_NONE, tostr(TIME_FORMAT_NONE) },
		{ TIME_FORMAT_FRAME, tostr(TIME_FORMAT_FRAME) },
		{ TIME_FORMAT_BYTE, tostr(TIME_FORMAT_BYTE) },
		{ TIME_FORMAT_SAMPLE, tostr(TIME_FORMAT_SAMPLE) },
		{ TIME_FORMAT_FIELD, tostr(TIME_FORMAT_FIELD) },
		{ TIME_FORMAT_MEDIA_TIME, tostr(TIME_FORMAT_MEDIA_TIME) },
		{ AMPROPSETID_Pin, tostr(AMPROPSETID_Pin) },
		{ PIN_CATEGORY_CAPTURE, tostr(PIN_CATEGORY_CAPTURE) },
		{ PIN_CATEGORY_PREVIEW, tostr(PIN_CATEGORY_PREVIEW) },
		{ PIN_CATEGORY_ANALOGVIDEOIN, tostr(PIN_CATEGORY_ANALOGVIDEOIN) },
		{ PIN_CATEGORY_VBI, tostr(PIN_CATEGORY_VBI) },
		{ PIN_CATEGORY_VIDEOPORT, tostr(PIN_CATEGORY_VIDEOPORT) },
		{ PIN_CATEGORY_NABTS, tostr(PIN_CATEGORY_NABTS) },
		{ PIN_CATEGORY_EDS, tostr(PIN_CATEGORY_EDS) },
		{ PIN_CATEGORY_TELETEXT, tostr(PIN_CATEGORY_TELETEXT) },
		{ PIN_CATEGORY_CC, tostr(PIN_CATEGORY_CC) },
		{ PIN_CATEGORY_STILL, tostr(PIN_CATEGORY_STILL) },
		{ PIN_CATEGORY_TIMECODE, tostr(PIN_CATEGORY_TIMECODE) },
		{ PIN_CATEGORY_VIDEOPORT_VBI, tostr(PIN_CATEGORY_VIDEOPORT_VBI) },
		{ LOOK_UPSTREAM_ONLY, tostr(LOOK_UPSTREAM_ONLY) },
		{ LOOK_DOWNSTREAM_ONLY, tostr(LOOK_DOWNSTREAM_ONLY) },
		{ CLSID_TVTunerFilterPropertyPage, tostr(CLSID_TVTunerFilterPropertyPage) },
		{ CLSID_CrossbarFilterPropertyPage, tostr(CLSID_CrossbarFilterPropertyPage) },
		{ CLSID_TVAudioFilterPropertyPage, tostr(CLSID_TVAudioFilterPropertyPage) },
		{ CLSID_VideoProcAmpPropertyPage, tostr(CLSID_VideoProcAmpPropertyPage) },
		{ CLSID_CameraControlPropertyPage, tostr(CLSID_CameraControlPropertyPage) },
		{ CLSID_AnalogVideoDecoderPropertyPage, tostr(CLSID_AnalogVideoDecoderPropertyPage) },
		{ CLSID_VideoStreamConfigPropertyPage, tostr(CLSID_VideoStreamConfigPropertyPage) },
		{ CLSID_AudioRendererAdvancedProperties, tostr(CLSID_AudioRendererAdvancedProperties) },
		{ CLSID_VideoMixingRenderer, tostr(CLSID_VideoMixingRenderer) },
		{ CLSID_VideoRendererDefault, tostr(CLSID_VideoRendererDefault) },
		{ CLSID_AllocPresenter, tostr(CLSID_AllocPresenter) },
		{ CLSID_AllocPresenterDDXclMode, tostr(CLSID_AllocPresenterDDXclMode) },
		{ CLSID_VideoPortManager, tostr(CLSID_VideoPortManager) },
		{ CLSID_VideoMixingRenderer9, tostr(CLSID_VideoMixingRenderer9) },
		{ CLSID_EnhancedVideoRenderer, tostr(CLSID_EnhancedVideoRenderer) },
		{ CLSID_MFVideoMixer9, tostr(CLSID_MFVideoMixer9) },
		{ CLSID_MFVideoPresenter9, tostr(CLSID_MFVideoPresenter9) },
		{ CLSID_EVRTearlessWindowPresenter9, tostr(CLSID_EVRTearlessWindowPresenter9) },
		{ CLSID_EVRPlaybackPipelineOptimizer, tostr(CLSID_EVRPlaybackPipelineOptimizer) },
		{ EVRConfig_ForceBob, tostr(EVRConfig_ForceBob) },
		{ EVRConfig_AllowDropToBob, tostr(EVRConfig_AllowDropToBob) },
		{ EVRConfig_ForceThrottle, tostr(EVRConfig_ForceThrottle) },
		{ EVRConfig_AllowDropToThrottle, tostr(EVRConfig_AllowDropToThrottle) },
		{ EVRConfig_ForceHalfInterlace, tostr(EVRConfig_ForceHalfInterlace) },
		{ EVRConfig_AllowDropToHalfInterlace, tostr(EVRConfig_AllowDropToHalfInterlace) },
		{ EVRConfig_ForceScaling, tostr(EVRConfig_ForceScaling) },
		{ EVRConfig_AllowScaling, tostr(EVRConfig_AllowScaling) },
		{ EVRConfig_ForceBatching, tostr(EVRConfig_ForceBatching) },
		{ EVRConfig_AllowBatching, tostr(EVRConfig_AllowBatching) },
		{ CLSID_NetworkProvider, tostr(CLSID_NetworkProvider) },
		{ CLSID_ATSCNetworkProvider, tostr(CLSID_ATSCNetworkProvider) },
		{ CLSID_ATSCNetworkPropertyPage, tostr(CLSID_ATSCNetworkPropertyPage) },
		{ CLSID_DVBSNetworkProvider, tostr(CLSID_DVBSNetworkProvider) },
		{ CLSID_DVBTNetworkProvider, tostr(CLSID_DVBTNetworkProvider) },
		{ CLSID_DVBCNetworkProvider, tostr(CLSID_DVBCNetworkProvider) },
		{ DSATTRIB_UDCRTag, tostr(DSATTRIB_UDCRTag) },
		{ DSATTRIB_PicSampleSeq, tostr(DSATTRIB_PicSampleSeq) },
		{ DSATTRIB_OptionalVideoAttributes, tostr(DSATTRIB_OptionalVideoAttributes) },
		{ DSATTRIB_CC_CONTAINER_INFO, tostr(DSATTRIB_CC_CONTAINER_INFO) },
		{ DSATTRIB_TRANSPORT_PROPERTIES, tostr(DSATTRIB_TRANSPORT_PROPERTIES) },
		{ DSATTRIB_PBDATAG_ATTRIBUTE, tostr(DSATTRIB_PBDATAG_ATTRIBUTE) },
		{ DSATTRIB_CAPTURE_STREAMTIME, tostr(DSATTRIB_CAPTURE_STREAMTIME) },
		{ DSATTRIB_DSHOW_STREAM_DESC, tostr(DSATTRIB_DSHOW_STREAM_DESC) },
		{ DSATTRIB_SAMPLE_LIVE_STREAM_TIME, tostr(DSATTRIB_SAMPLE_LIVE_STREAM_TIME) },
		{ UUID_UdriTagTables, tostr(UUID_UdriTagTables) },
		{ UUID_WMDRMTagTables, tostr(UUID_WMDRMTagTables) },
		{ CLSID_DShowTVEFilter, tostr(CLSID_DShowTVEFilter) },
		{ CLSID_TVEFilterTuneProperties, tostr(CLSID_TVEFilterTuneProperties) },
		{ CLSID_TVEFilterCCProperties, tostr(CLSID_TVEFilterCCProperties) },
		{ CLSID_TVEFilterStatsProperties, tostr(CLSID_TVEFilterStatsProperties) },
		{ CLSID_IVideoEncoderProxy, tostr(CLSID_IVideoEncoderProxy) },
		{ CLSID_ICodecAPIProxy, tostr(CLSID_ICodecAPIProxy) },
		{ CLSID_IVideoEncoderCodecAPIProxy, tostr(CLSID_IVideoEncoderCodecAPIProxy) },
		{ ENCAPIPARAM_BITRATE, tostr(ENCAPIPARAM_BITRATE) },
		{ ENCAPIPARAM_PEAK_BITRATE, tostr(ENCAPIPARAM_PEAK_BITRATE) },
		{ ENCAPIPARAM_BITRATE_MODE, tostr(ENCAPIPARAM_BITRATE_MODE) },
		{ ENCAPIPARAM_SAP_MODE, tostr(ENCAPIPARAM_SAP_MODE) },
		{ CODECAPI_CHANGELISTS, tostr(CODECAPI_CHANGELISTS) },
		{ CODECAPI_VIDEO_ENCODER, tostr(CODECAPI_VIDEO_ENCODER) },
		{ CODECAPI_AUDIO_ENCODER, tostr(CODECAPI_AUDIO_ENCODER) },
		{ CODECAPI_SETALLDEFAULTS, tostr(CODECAPI_SETALLDEFAULTS) },
		{ CODECAPI_ALLSETTINGS, tostr(CODECAPI_ALLSETTINGS) },
		{ CODECAPI_SUPPORTSEVENTS, tostr(CODECAPI_SUPPORTSEVENTS) },
		{ CODECAPI_CURRENTCHANGELIST, tostr(CODECAPI_CURRENTCHANGELIST) },
		{ CLSID_SBE2MediaTypeProfile, tostr(CLSID_SBE2MediaTypeProfile) },
		{ CLSID_SBE2FileScan, tostr(CLSID_SBE2FileScan) },
		{ CODECAPI_AVDecMmcssClass, tostr(CODECAPI_AVDecMmcssClass) },
};

#undef tostr

typedef struct tagFourCCStr {
	DWORD fourCC;
	TCHAR *fourCCName;
	VIDEOFORMAT type;
}FOURCCSTRING;

FOURCCSTRING supportFourCCList[] = {
	UNUSED_FOURCC_SUPPORT,
	{ BI_RGB, TEXT("RGB"), VideoOutputType_RGB24 },
	{ '2BGR', TEXT("RGB2"), VideoOutputType_None },
	{ '4BGR', TEXT("RGB4"), VideoOutputType_None },
	{ 'ABGR', TEXT("RGBA"), VideoOutputType_None },
	{ '024I', TEXT("I420"), VideoOutputType_I420 },
	{ 'VUYI', TEXT("IYUV"), VideoOutputType_None },
	{ '21VY', TEXT("YV12"), VideoOutputType_YV12 },
	{ 'UYVY', TEXT("YVYU"), VideoOutputType_YVYU },
	{ '2YUY', TEXT("YUY2"), VideoOutputType_YUY2 },
	{ 'YVYU', TEXT("UYVY"), VideoOutputType_UYVY },
	{ 'CYDH', TEXT("HDYC"), VideoOutputType_HDYC },
	{ 'V4PM', TEXT("MP4V"), VideoOutputType_None },
	{ '2S4M', TEXT("M4S2"), VideoOutputType_None },
	{ '462H', TEXT("H264"), VideoOutputType_H264 },
	{ 'GPJM', TEXT("MJPG"), VideoOutputType_MJPG },
};
#undef UNUSED_FOURCC

dxUtils::dxUtils()
{
}

dxUtils::~dxUtils()
{
}

BITMAPINFOHEADER *dxUtils::getBmpHeader(AM_MEDIA_TYPE *pmt)
{
	BITMAPINFOHEADER *pvh = NULL;
	if (pmt->formattype == FORMAT_VideoInfo){
		if (pmt->cbFormat == sizeof(VIDEOINFOHEADER)){
			pvh = &(reinterpret_cast<VIDEOINFOHEADER*>(pmt->pbFormat)->bmiHeader);
		}
	}
	else if (pmt->formattype == FORMAT_VIDEOINFO2){
		if (pmt->cbFormat == sizeof(VIDEOINFOHEADER2)){
			pvh = &(reinterpret_cast<VIDEOINFOHEADER2*>(pmt->pbFormat)->bmiHeader);
		}
	}

	return pvh;
}

double dxUtils::calcFps(REFERENCE_TIME rt){
	double fps = 0;
	if (rt){
		fps = 10000000.0 / rt;
	}
	return fps;
}

REFERENCE_TIME *dxUtils::getFrameInterval(AM_MEDIA_TYPE *pmt)
{
	REFERENCE_TIME *time = NULL;
	if (pmt->formattype == FORMAT_VideoInfo){
		if (pmt->cbFormat == sizeof(VIDEOINFOHEADER)){
			time = &(reinterpret_cast<VIDEOINFOHEADER*>(pmt->pbFormat)->AvgTimePerFrame);
		}
	}
	else if (pmt->formattype == FORMAT_VIDEOINFO2){
		if (pmt->cbFormat == sizeof(VIDEOINFOHEADER2)){
			time = &(reinterpret_cast<VIDEOINFOHEADER2*>(pmt->pbFormat)->AvgTimePerFrame);
		}
	}

	return time;
}

TCHAR* dxUtils::guid2str(GUID id)
{
	GUIDSTRING *idstr = &supportList[0];
	for (int i = 0; i < ARRAYSIZE(supportList); i++){
		if (id == supportList[i].id){
			idstr = &(supportList[i]);
			break;
		}
	}
	return idstr->name;
}

TCHAR *dxUtils::fourCCStr(DWORD fourCC)
{
	FOURCCSTRING *idstr = &supportFourCCList[0];
	for (int i = 0; i < ARRAYSIZE(supportFourCCList); i++){
		if (fourCC == supportFourCCList[i].fourCC){
			idstr = &supportFourCCList[i];
			break;
		}
	}

	return idstr->fourCCName;
}

TCHAR *dxUtils::getInfo(GUID id, DWORD fourCC)
{
	TCHAR * name = guid2str(id);
	if ((name == supportList[0].name) && (fourCC != supportFourCCList[0].fourCC)){
		name = fourCCStr(fourCC);
	}

	return name;
}

void dxUtils::FreeMediaType(AM_MEDIA_TYPE& mt)
{
	if (mt.cbFormat != 0)
	{
		CoTaskMemFree((PVOID)mt.pbFormat);
		mt.cbFormat = 0;
		mt.pbFormat = NULL;
	}
	if (mt.pUnk != NULL)
	{
		// pUnk should not be used.
		mt.pUnk->Release();
		mt.pUnk = NULL;
	}
}

VIDEOFORMAT dxUtils::getVideoTypeByFourCC(DWORD fourCC)
{
	VIDEOFORMAT format = VideoOutputType_None;
	for (int i = 0; i < ARRAYSIZE(supportFourCCList); i++){
		if (supportFourCCList[i].fourCC == fourCC){
			format = supportFourCCList[i].type;
		}
	}
	return format;
}

VIDEOFORMAT dxUtils::getVideoTypeByGuid(GUID id)
{
	VIDEOFORMAT format = VideoOutputType_None;
	format = getVideoTypeByFourCC(id.Data1);

	return format;
}

VIDEOFORMAT dxUtils::getVideoType(AM_MEDIA_TYPE *mt)
{
	VIDEOFORMAT format = VideoOutputType_None;
	BITMAPINFOHEADER *pVh = dxUtils::getBmpHeader(mt);

	if (mt->subtype == MEDIASUBTYPE_None && pVh)
		format = getVideoTypeByFourCC(pVh->biCompression);
	else 
		format = getVideoTypeByGuid(mt->subtype);

	return format;
}
