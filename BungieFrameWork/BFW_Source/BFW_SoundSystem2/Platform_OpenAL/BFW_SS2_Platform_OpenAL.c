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

// ======================================================================
// defines
// ======================================================================
#define SScMaxSoundChannels				16

#define SScVolume_Scale					3

#define SScVolume_Max					DSBVOLUME_MAX
#define SScVolume_Min					(DSBVOLUME_MIN / SScVolume_Scale)
#define SScPan_Left						(DSBPAN_LEFT / SScVolume_Scale)
#define SScPan_Center					DSBPAN_CENTER
#define SScPan_Right					(DSBPAN_RIGHT / SScVolume_Scale)

// at 2.816k bytes per packet to decompress
#define SScNumPacketsToDecompress		500

#define SScZeroSound					(0.050f)

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
}

// ----------------------------------------------------------------------
void
SS2rPlatform_SoundChannel_Pause(
	SStSoundChannel				*inSoundChannel)
{
}

// ----------------------------------------------------------------------
void
SS2rPlatform_SoundChannel_Play(
	SStSoundChannel				*inSoundChannel)
{
}

// ----------------------------------------------------------------------
void
SS2rPlatform_SoundChannel_Resume(
	SStSoundChannel				*inSoundChannel)
{
}

// ----------------------------------------------------------------------
UUtBool
SS2rPlatform_SoundChannel_SetSoundData(
	SStSoundChannel				*inSoundChannel,
	SStSoundData				*inSoundData)
{
	return UUcFalse;
}

// ----------------------------------------------------------------------
void
SS2rPlatform_SoundChannel_SetPan(
	SStSoundChannel				*inSoundChannel,
	UUtUns32					inPanFlags,
	float						inPan)
{
}

// ----------------------------------------------------------------------
void
SS2rPlatform_SoundChannel_SetPitch(
	SStSoundChannel				*inSoundChannel,
	float						inPitch)
{
}

// ----------------------------------------------------------------------
void
SS2rPlatform_SoundChannel_SetVolume(
	SStSoundChannel				*inSoundChannel,
	float						inVolume)
{
}

// ----------------------------------------------------------------------
void
SS2rPlatform_SoundChannel_Silence(
	SStSoundChannel				*inSoundChannel)
{
}

// ----------------------------------------------------------------------
void
SS2rPlatform_SoundChannel_Stop(
	SStSoundChannel				*inSoundChannel)
{
}

// ----------------------------------------------------------------------
void
SS2rPlatform_SoundChannel_Terminate(
	SStSoundChannel				*inSoundChannel)
{
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
	*outNumChannels = 16;
	return UUcError_None;
}

UUtError
SS2rPlatform_InitializeThread(
	void)
{
}

// ----------------------------------------------------------------------
void
SS2rPlatform_TerminateThread(
	void)
{
}

// ----------------------------------------------------------------------
void
SS2rPlatform_Terminate(
	void)
{
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
