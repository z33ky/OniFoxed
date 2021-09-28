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
	alSourcePlay(inSoundChannel->pd.source);
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
	else if (inSoundChannel->flags & SScSoundChannelFlag_Stereo && SScBitsPerSample ==  16)
	{
		format = AL_FORMAT_STEREO16;
	}
	else
	{
		return UUcFalse;
	}
	
	//TODO: decompress data?
	alBufferData(inSoundChannel->pd.buffer, format, inSoundData->data, inSoundData->num_bytes, SScSamplesPerSecond);
	
	alSourcei(inSoundChannel->pd.source, AL_BUFFER, inSoundChannel->pd.buffer);
	
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
	//looks like Oni does mixing itself, so we only use one "source" anyways?
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
	
	ALCint numMono, numStereo;
	alcGetIntegerv(SSgDevice, ALC_MONO_SOURCES, 1, &numMono);
	alcGetIntegerv(SSgDevice, ALC_STEREO_SOURCES, 1, &numStereo);
	*outNumChannels = UUmMin(numMono, numStereo);
	if (*outNumChannels == 0) {
		*outNumChannels = SScMaxSoundChannels;
	}
	SSgContext = alcCreateContext(SSgDevice, NULL);
	
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
