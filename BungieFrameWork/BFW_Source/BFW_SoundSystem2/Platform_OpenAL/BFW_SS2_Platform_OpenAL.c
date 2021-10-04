// ======================================================================
// BFW_SS2_Platform_OpenAL.c
// ======================================================================

// ======================================================================
// includes
// ======================================================================

#include "bfw_math.h"

#include "BFW.h"
#include "BFW_SoundSystem2.h"
#include "BFW_SS2_Private.h"
#include "BFW_SS2_Platform.h"
#include "BFW_SS2_IMA.h"
#include "BFW_WindowManager.h"

#include "BFW_Console.h"

#include "AL/al.h"
#include "AL/alc.h"
#include <math.h>

#include <libavutil/frame.h>
#include <libavutil/mem.h>

#include <libavcodec/avcodec.h>

// ======================================================================
// defines
// ======================================================================
#define SScMaxSoundChannels				16

// ======================================================================
// globals
// ======================================================================
/*
static LPDIRECTSOUND			SSgDirectSound;
static LPDIRECTSOUNDBUFFER		SSgPrimaryBuffer;
static LPDIRECTSOUND3DLISTENER	SSgDS3DListener;
//static LPDIRECTSOUND3DBUFFER	SSgReverbBuffer;
//static LPKSPROPERTYSET			SSgReverbPropertySet;
static UUtUns32					SSgNumChannels;

static DSCAPS					SSgDSCaps;

static D3DVALUE					SSgDistanceFactor;
static D3DVALUE					SSgRolloffFactor;

static HANDLE					SSgEventThread;
static UUtUns32					SSgEventThreadID;

static UUtMemory_Array			*SSgSoundChannelList;
static UUtBool					SSgUpdate_Run;
*/
static ALCdevice* SSgDevice = NULL;
static ALCcontext* SSgContext = NULL;

// ======================================================================
// functions
// ======================================================================
// ----------------------------------------------------------------------
void
SS2rPlatform_GetDebugNeeds(
	UUtUns32					*outNumLines)
{
	*outNumLines = 0;
	//TODO: stubbed
}

// ----------------------------------------------------------------------
void
SS2rPlatform_ShowDebugInfo_Overall(
	IMtPoint2D					*inDest)
{
	//TODO: stubbed
}

// ----------------------------------------------------------------------
void
SS2rPlatform_PerformanceStartFrame(
	void)
{
	//TODO: stubbed
}

// ----------------------------------------------------------------------
void
SS2rPlatform_PerformanceEndFrame(
	void)
{
	//TODO: stubbed
}

// ======================================================================
#if 0
#pragma mark -
#endif
// ======================================================================
// ----------------------------------------------------------------------
void
SS2rPlatform_SoundChannel_Initialize(
	SStSoundChannel				*inSoundChannel)
{
SS2rPlatform_InitializeThread();
	inSoundChannel->pd.buffer = 0;
	alGenBuffers(1, &inSoundChannel->pd.buffer);
	inSoundChannel->pd.source = 0;
	alGenSources(1, &inSoundChannel->pd.source);
	//TODO: needed?
	alSourcef(inSoundChannel->pd.source, AL_PITCH, 1);
	alSourcef(inSoundChannel->pd.source, AL_GAIN, 1.0f);
	alSource3f(inSoundChannel->pd.source, AL_POSITION, 0, 0, 0);
	alSource3f(inSoundChannel->pd.source, AL_VELOCITY, 0, 0, 0);
	alSourcei(inSoundChannel->pd.source, AL_LOOPING, AL_FALSE);
}

// ----------------------------------------------------------------------
void
SS2rPlatform_SoundChannel_Pause(
	SStSoundChannel				*inSoundChannel)
{
	alSourcePause(inSoundChannel->pd.source);
}

// ----------------------------------------------------------------------
void
SS2rPlatform_SoundChannel_Play(
	SStSoundChannel				*inSoundChannel)
{
	alSourcei(inSoundChannel->pd.source, AL_LOOPING, SSiSoundChannel_IsLooping(inSoundChannel) == UUcTrue ? AL_TRUE : AL_FALSE);
	alSourcePlay(inSoundChannel->pd.source);
	SSiSoundChannel_SetPlaying(inSoundChannel, UUcTrue);
}

// ----------------------------------------------------------------------
void
SS2rPlatform_SoundChannel_Resume(
	SStSoundChannel				*inSoundChannel)
{
	alSourcePlay(inSoundChannel->pd.source);
}

// ----------------------------------------------------------------------
UUtBool
SS2rPlatform_SoundChannel_SetSoundData(
	SStSoundChannel				*inSoundChannel,
	SStSoundData				*inSoundData)
{
	ALenum format;
	unsigned channels = SSrSound_GetNumChannels(inSoundData);
	if (inSoundChannel->flags & SScSoundChannelFlag_Mono && SScBitsPerSample == 8)
	{
		format = AL_FORMAT_MONO8;
	}
	else if (inSoundChannel->flags & SScSoundChannelFlag_Mono && SScBitsPerSample == 16)
	{
		format = AL_FORMAT_MONO16;
	}
	else if (inSoundChannel->flags & SScSoundChannelFlag_Stereo && SScBitsPerSample == 8)
	{
		format = AL_FORMAT_STEREO8;
	}
	else if (inSoundChannel->flags & SScSoundChannelFlag_Stereo && SScBitsPerSample == 16)
	{
		format = AL_FORMAT_STEREO16;
	}
	else
	{
		return UUcFalse;
	}
	
	av_log_set_level(AV_LOG_DEBUG);
	const AVCodec *codec = avcodec_find_decoder(AV_CODEC_ID_ADPCM_MS);
	if (!codec) {
		return UUcFalse;
	}
	AVCodecContext *c = avcodec_alloc_context3(codec);
	if (!c) {
		return UUcFalse;
	}
	c->codec_id = AV_CODEC_ID_ADPCM_MS;
	c->codec_type = AVMEDIA_TYPE_AUDIO;
	c->channels = channels;
	c->sample_rate = SScSamplesPerSecond;
	c->sample_fmt = AV_SAMPLE_FMT_S16;
	c->channel_layout = 0;
	c->bit_rate = 128000; //(c->sample_rate * BitsPerSample) / c->channels;
	if (avcodec_open2(c, codec, NULL) < 0) {
		return UUcFalse;
	}

	AVCodecParameters *par = avcodec_parameters_alloc();
	avcodec_parameters_from_context(par, c);
	par->block_align = (channels == 2) ? 1024 : 512;
	avcodec_parameters_to_context(c, par);
	//avcodec_parameters_free(par);

	AVPacket *pkt = av_packet_alloc();
	if (!pkt) {
		return UUcFalse;
	}

	//TODO: apparently only a single "frame" should be sent per packet
	char *frame = inSoundData->data;
	char *frames_end = frame + inSoundData->num_bytes;
	const size_t framesz = par->block_align;
	AVFrame *decoded_frame = av_frame_alloc();
	if (!decoded_frame) {
		return UUcFalse;
	}
	//FIXME: slight overallocation
	void *decoded = malloc((inSoundData->num_bytes + framesz) * 4);
	size_t total = 0;
	while(1) {
		int ret = avcodec_receive_frame(c, decoded_frame);
		if (ret == -11) //EAGAIN
		{
			if (frame == frames_end) {
				ret = avcodec_send_packet(c, NULL);
				if (ret != 0) {
					return UUcFalse;
				}
			} else {
				size_t left = frames_end - frame;
				size_t sz = (framesz <= left) ? framesz : left;
				//FIXME: can we not allocate a new buffer for every frame??
				void *avbuf = av_memdup(frame, sz);
				if (!avbuf) {
					return UUcFalse;
				}
				ret = av_packet_from_data(pkt, avbuf, sz);
				if (ret != 0) {
					av_free(avbuf);
					return UUcFalse;
				}
				ret = avcodec_send_packet(c, pkt);
				av_packet_unref(pkt);
				if (ret != 0) {
					return UUcFalse;
				}
				frame += sz;
			}
			continue;
		}
		if (ret == AVERROR_EOF) {
			break;
		}
		if (ret < 0) {
			return UUcFalse;
		}
		int data_size = av_samples_get_buffer_size(
			NULL,
			decoded_frame->channels,
			decoded_frame->nb_samples,
			decoded_frame->format,
			1
		);
		memcpy(decoded + total, decoded_frame->extended_data[0], data_size);
		av_frame_unref(decoded_frame);
		total += data_size;
	}
	av_frame_free(decoded_frame);
	
	alSourcei(inSoundChannel->pd.source, AL_BUFFER, 0);
	alBufferData(inSoundChannel->pd.buffer, format, decoded, total, SScSamplesPerSecond);
	alSourcei(inSoundChannel->pd.source, AL_BUFFER, inSoundChannel->pd.buffer);
	free(decoded);
	
	avcodec_free_context(&c);
	av_packet_free(pkt);
	
	return UUcTrue;
}

// ----------------------------------------------------------------------
void
SS2rPlatform_SoundChannel_SetPan(
	SStSoundChannel				*inSoundChannel,
	UUtUns32					inPanFlags,
	float						inPan)
{
	// https://github.com/kcat/openal-soft/issues/194#issuecomment-392520073
	//TODO: https://stackoverflow.com/q/20950606
	//alSourcef(inSoundChannel->pd.source, AL_SOURCE_DISTANCE_MODEL, AL_NONE); (context fn, not buffer)
	alSourcef(inSoundChannel->pd.source, AL_ROLLOFF_FACTOR, 0.0f);
	alSourcei(inSoundChannel->pd.source, AL_SOURCE_RELATIVE, AL_TRUE);
	switch (inPanFlags)
	{
		case SScPanFlag_Left:
			inPan = -inPan;
		break;
		
		case SScPanFlag_Right:
		break;
		
		case SScPanFlag_None:
		default:
			inPan = 0.0f;
		break;
	}
	alSource3f(inSoundChannel->pd.source, AL_POSITION, inPan, 0, -sqrtf(1.0f - inPan*inPan));
}

// ----------------------------------------------------------------------
void
SS2rPlatform_SoundChannel_SetPitch(
	SStSoundChannel				*inSoundChannel,
	float						inPitch)
{
	alSourcef(inSoundChannel->pd.source, AL_PITCH, inPitch);
}

// ----------------------------------------------------------------------
void
SS2rPlatform_SoundChannel_SetVolume(
	SStSoundChannel				*inSoundChannel,
	float						inVolume)
{
	alSourcef(inSoundChannel->pd.source, AL_GAIN, inVolume);
}

// ----------------------------------------------------------------------
void
SS2rPlatform_SoundChannel_Silence(
	SStSoundChannel				*inSoundChannel)
{
	//Mac also does nothing
}

// ----------------------------------------------------------------------
void
SS2rPlatform_SoundChannel_Stop(
	SStSoundChannel				*inSoundChannel)
{
	alSourceStop(inSoundChannel->pd.source);
	SSiSoundChannel_SetPlaying(inSoundChannel, UUcFalse);
}

// ----------------------------------------------------------------------
void
SS2rPlatform_SoundChannel_Terminate(
	SStSoundChannel				*inSoundChannel)
{
	alDeleteSources(1, &inSoundChannel->pd.source);
	inSoundChannel->pd.source = 0;
	alDeleteBuffers(1, &inSoundChannel->pd.buffer);
	inSoundChannel->pd.buffer = 0;
}

// ======================================================================
#if 0
#pragma mark -
#endif
// ======================================================================
// ----------------------------------------------------------------------
UUtError
SS2rPlatform_Initialize(
	UUtWindow					inWindow,			/* only used by Win32 */
	UUtUns32					*outNumChannels,
	UUtBool						inUseSound)
{
	*outNumChannels = 0;
	
	SSgDevice = alcOpenDevice(NULL);
	
	//FIXME: the number of channels will be split across mono/stereo
	ALCint attrs[] = {
		//ALC_FREQUENCY, SScSamplesPerSecond,
		ALC_MONO_SOURCES, SScMaxSoundChannels * 2,
		ALC_STEREO_SOURCES, SScMaxSoundChannels * 2,
		0
	};
	SSgContext = alcCreateContext(SSgDevice, &attrs);
	
	ALCint numMono, numStereo;
	alcGetIntegerv(SSgDevice, ALC_MONO_SOURCES, 1, &numMono);
	alcGetIntegerv(SSgDevice, ALC_STEREO_SOURCES, 1, &numStereo);
	*outNumChannels = UUmMin(numMono, numStereo);
	if (*outNumChannels == 0) {
		*outNumChannels = SScMaxSoundChannels;
	}
	
	return UUcError_None;
}

UUtError
SS2rPlatform_InitializeThread(
	void)
{

	if (!alcMakeContextCurrent(SSgContext)) {
	}
}

// ----------------------------------------------------------------------
void
SS2rPlatform_TerminateThread(
	void)
{
	alcMakeContextCurrent(NULL);
	alcDestroyContext(SSgContext);
}

// ----------------------------------------------------------------------
void
SS2rPlatform_Terminate(
	void)
{
	alcCloseDevice(SSgDevice);
}

// ======================================================================
#if 0
#pragma mark -
#endif
// ======================================================================
// ----------------------------------------------------------------------
void
SSrDeleteGuard(
	SStGuard					*inGuard)
{
}

// ----------------------------------------------------------------------
void
SSrCreateGuard(
	SStGuard					**inGuard)
{
}

// ----------------------------------------------------------------------
void
SSrReleaseGuard(
	SStGuard					*inGuard)
{
}

// ----------------------------------------------------------------------
void
SSrWaitGuard(
	SStGuard					*inGuard)
{
}
