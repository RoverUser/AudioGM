// AudioGM: XAudio2 wrapper for GameMaker.
// 
// Copyright (C) 2025 NTKRNLMP
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with this program. If not, see <https://www.gnu.org/licenses/>.

#ifndef AUDIOGM_H
#  define AUDIOGM_H

#  ifdef AUDIOGM_EXPORTS
#    define AUDIOGM_API extern "C" __declspec(dllexport)
#  else
#    define AUDIOGM_API extern "C" __declspec(dllimport)
#  endif
#  define AUDIOGM_CALLCONV __cdecl

// GameMaker types:
#  define GM_TRUE 1.0
#  define GM_FALSE 0.0
typedef double gmreal_t;
typedef const char* gmstring_t;
typedef void* gm_ptr; // wtf is this for

/////////////////////////////////////////////
// Docs:
// /*++
// Routine Description:
//     Loads XAudio 2 DLL.
// Arguments:
//     Channels - Number of channels, default is XAUDIO2_DEFAULT_CHANNELS (Desktop stereo speakers on PC, 5.1 surround on Xbox 360) if set to 0.
//     SampleRate - Sample rate, default is XAUDIO2_DEFAULT_SAMPLERATE (44100 on PC, 48000 on Xbox 360) if set to 0.
//     AudioDeviceIndex - Identifier of the device to receive the output audio, set to 0 to use the global default audio device.
// Return Value:
//     Returns FALSE if XAudio 2 was already initialized, or if something has gone horribly wrong.
// --*/
// double GM_InitAudio(double Channels, double SampleRate, double AudioDeviceIndex);
//
// /*++
// Routine Description:
//     Unloads XAudio 2 DLL
// Arguments:
//     None.
// Return Value:
//     Returns FALSE only if the XAudio 2 instance hasn't initialized yet.
// --*/
// double GM_UnloadAudio()
//
// /*++
// Routine Description:
//     Get a total count of audio devices in the system.
//
//     This function can only be called without XAudio 2 initialized.
// Arguments:
//     None.
// Return Value:
//     Returns the correct amount on success, 0 if the XAudio 2 instance has already been
//     initialized, or if it can't get the count.
// --*/
// double GM_GetDeviceCount()
//
// /*++
// Routine Description:
//     Get audio device's name.
// Arguments:
//     Audio device index (get one from GM_GetDeviceCount);
// Return Value:
//     Returns the correct name on success, "NULL" if the XAudio 2 instance has already been initialized,
//     or if it can't get the name.
// --*/
// string GM_GetDeviceName(double Index)
//
// /*++
// Routine Description:
//     Play's audio, duh, default volume is 1.
//     WARNING: DO NOT SET VOLUME HIGHER THAN 10, IT BLEW UP MY SPEAKERS.
// Arguments:
//     File - Path to the audio file.
//     Volume - Initial Volume.
//     Loop - Whether to loop the audio or not.
// Return Value:
//     Returns the audio slot on success, FALSE if XAudio 2 hasn't initialized yet, if Loop is not TRUE or
//     FALSE, or if the system has ran out of memory.
// --*/
// double GM_PlayAudio(string File, double Volume, double Loop)
//
// /*++
// Routine Description:
//     Set's the volume.
// Arguments:
//     AudioSlot - Audio that you want to crank up.
//     Volume - Volume, duh.
// Return Value:
//     Returns TRUE on success, FALSE if XAudio 2 hasn't initialized yet, if AudioSlot is a negative number
//     or it exceeds the amount of audios in the internal database, or if it didn't find any active audios
//     in the internal database.
// --*/
// double GM_SetVolume(double AudioSlot, double Volume)
//
// /*++
// Routine Description:
//     Wait's for an audio thread to finish.
//     If AudioSlot is 0, waits for every single audio in the internal database to finish.
//     WARNING: IF YOU EXPERIENCE DEADLOCKS WHEN USING THIS FUNCTION, CONTACT IVAN!
// Arguments:
//     AudioSlot - Audio that you want to wait for.
// Return Value:
//     Returns TRUE on success, FALSE if XAudio 2 hasn't initialized yet, if AudioSlot is a negative number
//     or it exceeds the amount of audios in the internal database, if it didn't find any active audios
//     or threads in the internal database, or if the audio is looped.
// --*/
// double GM_WaitForAudio(double AudioSlot)
//
// /*++
// Routine Description:
//     Pauses or resumes specified audio.
// Arguments:
//     AudioSlot - Audio that you want to pause/resume.
// Return Value:
//     Returns TRUE if it's paused, FALSE if it's resumed.
//     Can also return FALSE if XAudio 2 hasn't initialized yet, if AudioSlot is a negative number or it
//     exceeds the amount of audios in the internal database, or if it didn't find any active audios or
//     threads in the internal database.
//     (Who the fuck designed this API? Oh, right, it's me.)
// --*/
// double GM_PauseAndResume(double AudioSlot)
//
// /*++
// Routine Description:
//     Stops an audio, forever, duh.
// Arguments:
//     AudioSlot - Audio that you want to stop.
// Return Value:
//     Returns TRUE on success, FALSE if XAudio 2 hasn't initialized yet, if AudioSlot is a negative number
//     or it exceeds the amount of audios in the internal database, or if it didn't find any active audios
//     or threads in the internal database.
// --*/
// double GM_StopAudio(double AudioSlot)
//
AUDIOGM_API gmreal_t AUDIOGM_CALLCONV GM_InitAudio(gmreal_t Channels, gmreal_t SampleRate, gmreal_t AudioDeviceIndex);
AUDIOGM_API gmreal_t AUDIOGM_CALLCONV GM_UnloadAudio(void);
AUDIOGM_API gmreal_t AUDIOGM_CALLCONV GM_GetDeviceCount(void);
AUDIOGM_API gmstring_t AUDIOGM_CALLCONV GM_GetDeviceName(gmreal_t Index);
AUDIOGM_API gmreal_t AUDIOGM_CALLCONV GM_PlayAudio(gmstring_t WavPath, gmreal_t Volume, gmreal_t Loop);
AUDIOGM_API gmreal_t AUDIOGM_CALLCONV GM_SetVolume(gmreal_t AudioSlot, gmreal_t Volume);
AUDIOGM_API gmreal_t AUDIOGM_CALLCONV GM_WaitForAudio(gmreal_t AudioSlot);
AUDIOGM_API gmreal_t AUDIOGM_CALLCONV GM_PauseAndResume(gmreal_t AudioSlot);
AUDIOGM_API gmreal_t AUDIOGM_CALLCONV GM_StopAudio(gmreal_t AudioSlot);
#  include "AudioGM_private.h"

#endif // AUDIOGM_H
