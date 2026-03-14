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

#include "stdafx.h"
#include "AudioGM.h"
#include "DXUT\SDKwavefile.h"
#include "DebugLog.h"

std::vector<THREADINFO> ThreadList;
std::vector<AUDIOINFO> AudioList;

bool isInitialized = false;

IXAudio2* pXAudio2 = NULL;
IXAudio2MasteringVoice* pMasteringVoice = NULL;

// Internal prototype
unsigned int WINAPI BackgroundAudioThread(void* lpThreadParam);

// Public stuff
AUDIOGM_API gmreal_t AUDIOGM_CALLCONV GM_InitAudio(gmreal_t Channels, gmreal_t SampleRate, gmreal_t AudioDeviceIndex) {
	HRESULT Result = 0;
	UINT32 XAUDIO_FLAGS = 0;

	UINT32 DeviceCount = 0;
	XAUDIO2_DEVICE_DETAILS DeviceDetails = {0};
	int idk = 0;

	if (isInitialized)
		return GM_FALSE;

	UINT32  InputChannels = (Channels == 0) ? XAUDIO2_DEFAULT_CHANNELS : Channels,
			InputSampleRate = (SampleRate == 0) ? XAUDIO2_DEFAULT_SAMPLERATE : SampleRate,
			DeviceIndex = (AudioDeviceIndex == 0) ? 0 : AudioDeviceIndex;

#ifdef _DEBUG
#	ifndef DEBUG_FOR_PUBLIC
	XAUDIO_FLAGS = XAUDIO2_DEBUG_ENGINE;
#	endif
#endif

	Result = CoInitializeEx(NULL, COINIT_MULTITHREADED);
	while (Result == RPC_E_CHANGED_MODE) {
		CoUninitialize();
		Result = CoInitializeEx(NULL, COINIT_MULTITHREADED);
	}
	if (FAILED(Result)) {
		DbgPrintf("AudioGM: Failed to initialize COM! Reason: %s\n", COMErrStr(Result));
		return GM_FALSE;
	}

	Result = XAudio2Create(&pXAudio2, XAUDIO_FLAGS, XAUDIO2_DEFAULT_PROCESSOR);
	if (FAILED(Result)) {
		DbgPrintf("AudioGM: Failed to initialize XAudio! Reason: %s\n", XAudioErrStr(Result));
		CoUninitialize();
		return GM_FALSE;
	}

#ifdef _DEBUG
#	ifndef DEBUG_FOR_PUBLIC
	XAUDIO2_DEBUG_CONFIGURATION LogConf = {0};

	LogConf.TraceMask = XAUDIO2_LOG_WARNINGS |
						XAUDIO2_LOG_DETAIL |
						XAUDIO2_LOG_API_CALLS |
						XAUDIO2_LOG_TIMING;
	LogConf.BreakMask = XAUDIO2_LOG_ERRORS;
	LogConf.LogThreadID = true;
	LogConf.LogFileline = false;
	LogConf.LogFunctionName = false;
	LogConf.LogTiming = false;

	pXAudio2->SetDebugConfiguration(&LogConf, NULL);
#	endif
#endif

	Result = pXAudio2->GetDeviceCount(&DeviceCount);
	if (FAILED(Result) || DeviceCount < 0) {
		DbgPrintf("AudioGM: Can't get a total count of audio devices! Perhaps you don't have any?\n");
		SAFE_RELEASE(pXAudio2);
		CoUninitialize();
		return GM_FALSE;
	}

	for (UINT32 i = 0; i < DeviceCount; ++i) {
		Result = pXAudio2->GetDeviceDetails(i, &DeviceDetails);
		if (FAILED(Result)) {
			DbgPrintf("AudioGM: Can't get information about audio device number %d!\n", i);
			SAFE_RELEASE(pXAudio2);
			CoUninitialize();
			return GM_FALSE;
		}
		if ((i == 0 && DeviceDetails.Role != GlobalDefaultDevice) || (i != 0 && DeviceDetails.Role != NotDefaultDevice)) {
#ifdef _DEBUG
			__debugbreak();
#endif
		}
		if (!IsEqualGUID(DeviceDetails.OutputFormat.SubFormat, KSDATAFORMAT_SUBTYPE_PCM)) {
#ifdef _DEBUG
			__debugbreak();
#endif
		}
	}
	Result = pXAudio2->CreateMasteringVoice(&pMasteringVoice, InputChannels, InputSampleRate, 0, DeviceIndex);
	if (FAILED(Result)) {
		DbgPrintf("AudioGM: Failed creating mastering voice! Reason: %s\n", XAudioErrStr(Result));
		SAFE_RELEASE(pXAudio2);
		CoUninitialize();
		return GM_FALSE;
	}

	// Kostil kostilni.
	// TODO: I need to remove ASAP.
	AUDIOINFO AudioInfoTemp = {0};

	AudioInfoTemp.Slot = 0;
	AudioInfoTemp.ThreadSlot = 0;
	strcpy_s(AudioInfoTemp.WavPath, MAX_PATH, "Z:\\This\\File\\Doesnt\\Exist");
	AudioInfoTemp.Volume = 1337; // If your speaker blows up, you know why.
	AudioInfoTemp.Loop = GM_TRUE;
	AudioInfoTemp.Pause = GM_FALSE;
	AudioInfoTemp.Stop = GM_FALSE;
	AudioInfoTemp.Finished = GM_FALSE;

	AudioList.push_back(AudioInfoTemp);

	isInitialized = true;

	return GM_TRUE;
}

AUDIOGM_API gmreal_t AUDIOGM_CALLCONV GM_UnloadAudio(void) {
	if (!isInitialized)
		return GM_FALSE;

	if (pMasteringVoice)
		pMasteringVoice->DestroyVoice();

	SAFE_RELEASE(pXAudio2);
	CoUninitialize();
	isInitialized = false;
	return GM_TRUE;
}

AUDIOGM_API gmreal_t AUDIOGM_CALLCONV GM_GetDeviceCount(void) {
	HRESULT Result = 0;

	UINT32 DeviceCount = 0;

	if (isInitialized)
		return GM_FALSE;

	if (!GM_InitAudio(0, 0, 0)) {
		DbgPrintf("AudioGM: Can't init audio!\n");
		return GM_FALSE;
	}

	Result = pXAudio2->GetDeviceCount(&DeviceCount);
	if (FAILED(Result) || DeviceCount < 0) {
		DbgPrintf("AudioGM: Can't get a total count of audio devices! Perhaps you don't have any?\n");
		GM_UnloadAudio();
		return 0;
	}

	GM_UnloadAudio();
	return DeviceCount;
}

AUDIOGM_API gmstring_t AUDIOGM_CALLCONV GM_GetDeviceName(gmreal_t Index) {
	HRESULT Result = 0;

	gmreal_t TotalCountOfAudioDevices = 0;

	XAUDIO2_DEVICE_DETAILS DeviceDetails = {0};

	LCID lcid = 0;
	wchar_t CurrentLocale[LOCALE_NAME_MAX_LENGTH];

	size_t CDeviceNameCharsConverted = 0;
#	define CDeviceNameLen (256 + 1) * sizeof(char)
	// Can't statically allocate because we are going to return this string to GameMaker.
	LPSTR CDeviceName = NULL;
	CDeviceName = (LPSTR)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, CDeviceNameLen);
	if (CDeviceName == NULL) {
		DbgPrintf("AudioGM: Failed to allocate space for the audio device name!\n");
		return "NULL";
	}

	if (isInitialized)
		SAFE_DELETE(CDeviceName);
		return "NULL";

	TotalCountOfAudioDevices = GM_GetDeviceCount();
	if (Index > TotalCountOfAudioDevices) {
		DbgPrintf("AudioGM: This audio device doesn't exist!\n");
		SAFE_DELETE(CDeviceName);
		return "NULL";
	}

	if (!GM_InitAudio(0, 0, 0)) {
		DbgPrintf("AudioGM: Can't init audio!\n");
		SAFE_DELETE(CDeviceName);
		return "NULL";
	}

	Result = pXAudio2->GetDeviceDetails(Index, &DeviceDetails);
	if (FAILED(Result)) {
		DbgPrintf("AudioGM: Can't get information about audio device number %d!\n", Index);
		GM_UnloadAudio();
		SAFE_DELETE(CDeviceName);
		return "NULL";
	}

	// HACK: I don't know why, but on en-US and ru-RU this works perfectly fine, but on ru-KZ it doesn't convert the device name.
	//       Calling setlocale with ru-KZ seems to fix this.
	lcid = GetThreadLocale();
	if (LCIDToLocaleName(lcid, CurrentLocale, LOCALE_NAME_MAX_LENGTH, 0) == 0) {
		DbgPrintf("AudioGM: Can't set correct locale!\n");
		GM_UnloadAudio();
		SAFE_DELETE(CDeviceName);
		return "NULL";
	}
	_wsetlocale(LC_ALL, CurrentLocale);
	wcstombs_s(&CDeviceNameCharsConverted, (char*)CDeviceName, CDeviceNameLen, DeviceDetails.DisplayName, _TRUNCATE);

	GM_UnloadAudio();
	return (gmstring_t)CDeviceName;
	// Yeah, we have a memory leak, so what?
	//SAFE_DELETE(CDeviceName);
}

AUDIOGM_API gmreal_t AUDIOGM_CALLCONV GM_PlayAudio(gmstring_t WavPath, gmreal_t Volume, gmreal_t Loop) {
	if (!isInitialized)
		return GM_FALSE;

	if (Loop < 0 || Loop > 1) {
		return GM_FALSE;
	}

	// TODO: Check if WavPath is a valid path.

	PAUDIOINFO AudioInfoTemp = (PAUDIOINFO)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(AUDIOINFO));
	if (AudioInfoTemp == NULL) {
		DbgPrintf("AudioGM: Failed to allocate space for audio info!\n");
		return GM_FALSE;
	}

	AudioInfoTemp->Slot = AudioList.size();
	AudioInfoTemp->ThreadSlot = ThreadList.size();
	strcpy_s(AudioInfoTemp->WavPath, MAX_PATH, WavPath);
	AudioInfoTemp->Volume = Volume;
	AudioInfoTemp->Loop = Loop;
	AudioInfoTemp->Pause = GM_FALSE;
	AudioInfoTemp->Stop = GM_FALSE;
	AudioInfoTemp->Finished = GM_FALSE;

	AudioList.push_back(*AudioInfoTemp);

	THREADINFO TempInfo = {0};
	TempInfo.hThread = (HANDLE)_beginthreadex(
							NULL,
							0,
							BackgroundAudioThread,
							(void*)AudioInfoTemp,
							0,
							&TempInfo.dwThreadId
						);
	// TODO: Does XAudio handle thread priority by itself?
	//if (!SetThreadPriority(TempInfo.hThread, ABOVE_NORMAL_PRIORITY_CLASS)) {
	//	DbgPrintf("AudioGM: Failed to set audio's thread priority to [ABOVE_NORMAL_PRIORITY]! Error code: %d\n", GetLastError());
	//	return GM_FALSE;
	//}
	if (TempInfo.hThread == NULL) {
		DbgPrintf("AudioGM: Failed to create an audio thread!\n");
		return GM_FALSE;
	}

	ThreadList.push_back(TempInfo);

	return AudioList.size() - 1;
}

AUDIOGM_API gmreal_t AUDIOGM_CALLCONV GM_SetVolume(gmreal_t AudioSlot, gmreal_t Volume) {
	if (!isInitialized)
		return GM_FALSE;

	if (AudioList.empty())
		return GM_FALSE;

	if (AudioSlot < 0 || AudioSlot > AudioList.size())
		return GM_FALSE;

	AudioList[AudioSlot].Volume = Volume;
#ifdef USE_SLEEP
	Sleep(TIME_FOR_THREADS_TO_SYNC);
#endif
	return GM_TRUE;
}

AUDIOGM_API gmreal_t AUDIOGM_CALLCONV GM_WaitForAudio(gmreal_t AudioSlot) {
	if (!isInitialized)
		return GM_FALSE;

	if (AudioList.empty() || ThreadList.empty())
		return GM_FALSE;

	if (AudioSlot < 0 || AudioSlot > AudioList.size())
		return GM_FALSE;

	if (AudioSlot == 0) {
		// TODO: Detect if audio is looping, if it is, skip to the next audio.
		// TODO 2: Maybe we should use WaitForMultipleObjects instead?
		for (std::vector<THREADINFO>::size_type i = 0; i < ThreadList.size(); i++) {
			DbgPrintf("AudioGM: Waiting for thread %d.\n", i);
			WaitForSingleObject(ThreadList[i].hThread, INFINITE);
			// > > You can call _endthread or _endthreadex explicitly to terminate a thread;
			// > > however, _endthread or _endthreadex is called automatically when the
			// > > thread returns from the routine that's passed as a parameter.
			// >        - MSDN
			// > Well that took too long to figure out.
			// I figured that keeping it in a try/except block does the job.
			// __try {
			// 	CloseHandle(ThreadList[i].hThread);
			// } __except (GetExceptionCode() == EXCEPTION_INVALID_HANDLE) { /* Do nothing. */ }
		}
		return GM_TRUE;
	}
	if (AudioList[AudioSlot].Loop == GM_TRUE) {
		return GM_FALSE;
	}
	DbgPrintf("AudioGM: Waiting for audio %d.\n", AudioSlot);
	WaitForSingleObject(ThreadList[AudioList[AudioSlot].ThreadSlot].hThread, INFINITE);
	return GM_TRUE;
}

AUDIOGM_API gmreal_t AUDIOGM_CALLCONV GM_PauseAndResume(gmreal_t AudioSlot) {
	// TODO: Here's my idea, make a function that will give out the audio list (with name's).
	if (!isInitialized)
		return GM_FALSE;

	if (AudioList.empty())
		return GM_FALSE;

	if (AudioSlot < 0 || AudioSlot > AudioList.size())
		return GM_FALSE;

	AudioList[AudioSlot].Pause = !AudioList[AudioSlot].Pause;
#ifdef USE_SLEEP
	Sleep(TIME_FOR_THREADS_TO_SYNC);
#endif
	return AudioList[AudioSlot].Pause;
}

AUDIOGM_API gmreal_t AUDIOGM_CALLCONV GM_StopAudio(gmreal_t AudioSlot) {
	if (!isInitialized)
		return GM_FALSE;

	if (AudioList.empty())
		return GM_FALSE;

	if (AudioSlot < 0 || AudioSlot > AudioList.size())
		return GM_FALSE;

	AudioList[AudioSlot].Stop = GM_TRUE;
#ifdef USE_SLEEP
	Sleep(TIME_FOR_THREADS_TO_SYNC);
#endif
	return GM_TRUE;
}

// Internal stuff
unsigned int WINAPI BackgroundAudioThread(void* lpThreadParam) {
	HRESULT Result = 0;
	PAUDIOINFO AudioInfo = {0};

	size_t AudioListSlot = 0;

	LPSTR CWavPath = NULL;
	size_t WavPathCharsConverted = 0;
	LPWSTR WavPath[MAX_PATH * sizeof(wchar_t)] = {0};

	CWaveFile wav;
	WAVEFORMATEX* pwfx = NULL;
	DWORD cbWaveSize = 0;
	BYTE* pbWaveData = NULL;

	IXAudio2SourceVoice* pSourceVoice = NULL;
	gmreal_t InitialVolume = 0;
	gmreal_t InitialPause = GM_FALSE;
	XAUDIO2_BUFFER buffer = {0};
	BOOL isRunning = FALSE;
	XAUDIO2_VOICE_STATE state = {0};

	AudioInfo = (PAUDIOINFO)lpThreadParam;
	CWavPath = AudioInfo->WavPath;

	if (!isInitialized)
		return -1;

	AudioListSlot = AudioInfo->Slot;

	mbstowcs_s(&WavPathCharsConverted, (wchar_t*)WavPath, strlen(CWavPath) + 1, CWavPath, _TRUNCATE);

	// > > The rules of COM require CoInitialize or CoInitializeEx to be
	// > > called in any thread that makes calls to XAudio2.
	// > What a stupid rule, seems to work without it.
	// Reverting.
	Result = CoInitializeEx(NULL, COINIT_MULTITHREADED);
	while (Result == RPC_E_CHANGED_MODE) {
		CoUninitialize();
		Result = CoInitializeEx(NULL, COINIT_MULTITHREADED);
	}
	if (FAILED(Result)) {
		DbgPrintf("AudioGM: Failed to initialize COM! Reason: %s\n", COMErrStr(Result));
		return -1;
	}

	Result = wav.Open((LPWSTR)WavPath, NULL, WAVEFILE_READ);
	if (FAILED(Result)) {
		DbgPrintf("AudioGM: Can't open a WAV file!\n");
		CoUninitialize();
		return -1;
	}

	pwfx = wav.GetFormat();
	cbWaveSize = wav.GetSize();
	pbWaveData = (BYTE*)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, cbWaveSize); // new BYTE[cbWaveSize];

	Result = wav.Read(pbWaveData, cbWaveSize, &cbWaveSize);
	if (FAILED(Result)) {
		DbgPrintf("AudioGM: Can't read WAV data!\n");
		SAFE_DELETE(pbWaveData);
		CoUninitialize();
		return -1;
	}

	Result = pXAudio2->CreateSourceVoice(&pSourceVoice, pwfx);
	if (FAILED(Result)) {
		DbgPrintf("AudioGM: Error creating source voice! Reason: %s\n", XAudioErrStr(Result));
		SAFE_DELETE(pbWaveData);
		CoUninitialize();
		return -1;
	}

	InitialVolume = AudioInfo->Volume;
	pSourceVoice->SetVolume(InitialVolume);

	buffer.pAudioData = pbWaveData;
	buffer.Flags = XAUDIO2_END_OF_STREAM;
	buffer.AudioBytes = cbWaveSize;
	if (AudioInfo->Loop) {
		buffer.LoopCount = XAUDIO2_LOOP_INFINITE;
	}

	Result = pSourceVoice->SubmitSourceBuffer(&buffer);
	if (FAILED(Result)) {
		DbgPrintf("AudioGM: Error submitting source buffer! Reason: %s\n", XAudioErrStr(Result));
		pSourceVoice->DestroyVoice();
		SAFE_DELETE(pbWaveData);
		CoUninitialize();
		return -1;
	}

	InitialPause = AudioInfo->Pause;
	Result = pSourceVoice->Start(0);

	isRunning = TRUE;
	while (SUCCEEDED(Result) && isRunning) {
		pSourceVoice->GetState(&state);
		isRunning = (state.BuffersQueued > 0) != 0;

		if (InitialVolume != AudioList[AudioListSlot].Volume) {
			InitialVolume = AudioList[AudioListSlot].Volume;
			DbgPrintf("AudioGM: Setting volume to: %f\n", InitialVolume);
			Result = pSourceVoice->SetVolume(InitialVolume);
		}

		if (InitialPause != AudioList[AudioListSlot].Pause) {
			InitialPause = AudioList[AudioListSlot].Pause;
			if (InitialPause == GM_TRUE) {
				DbgPrintf("AudioGM: Pausing sound %d.\n", AudioListSlot);
				Result = pSourceVoice->Stop(0);
			} else {
				DbgPrintf("AudioGM: Resuming sound %d.\n", AudioListSlot);
				Result = pSourceVoice->Start(0);
			}
		}

		if (AudioList[AudioListSlot].Stop == GM_TRUE) {
			DbgPrintf("AudioGM: Stopping sound %d.\n", AudioListSlot);
			Result = pSourceVoice->Stop(0);
			Result = pSourceVoice->FlushSourceBuffers();
			isRunning = FALSE;
			break;
		}
#ifdef USE_SLEEP
		Sleep(MAIN_SLEEP);
#endif
	}

	pSourceVoice->DestroyVoice();
	SAFE_DELETE(pbWaveData);
	CoUninitialize();
	SAFE_DELETE(AudioInfo);

	AudioList[AudioListSlot].Finished = GM_TRUE;

	return 0;
}

// "function assumed not to throw an exception but does"
// Shush, rundll32 does take exceptions.
#pragma warning (disable: 4297)

// Test stuff
#ifdef ENABLE_TEST
AUDIOGM_API void AUDIOGM_CALLCONV TestAPI(HWND hwndStub, HINSTANCE hInstance, LPTSTR pszCmdLine, int nCmdShow) {
	gmreal_t CurrentID = 0;

	gmstring_t DeviceName = GM_GetDeviceName(0);
	MessageBoxA(NULL, DeviceName, "Default device name", MB_ICONINFORMATION | MB_OK);

	if (!GM_InitAudio(0, 0, 0)) {
		DbgPrintf("AudioGM: Can't init audio!\n");
		throw;
	}

	if (!(CurrentID = GM_PlayAudio("WAVs\\Electro_1.wav", 1.0, GM_FALSE))) {
		DbgPrintf("AudioGM: Can't play audio!\n");
		throw;
	}
	Sleep(1000);
	if (!GM_PlayAudio("WAVs\\MusicMono.wav", 1.0, GM_FALSE)) {
		DbgPrintf("AudioGM: Can't play audio!\n");
		throw;
	}
	Sleep(1000);
	if (!GM_PlayAudio("WAVs\\Techno_1.wav", 1.0, GM_FALSE)) {
		DbgPrintf("AudioGM: Can't play audio!\n");
		throw;
	}
	for (gmreal_t i = 0.0; i < 5.0; i += 0.45) {
		DbgPrintf("AudioGM: Trying to set volume: %f\n", i);
		GM_SetVolume(CurrentID, i);
	}
	for (gmreal_t i = 5.0; i > 0.5; i -= 0.45) {
		DbgPrintf("AudioGM: Trying to set volume: %f\n", i);
		GM_SetVolume(CurrentID, i);
	}
	Sleep(5000);
	GM_PauseAndResume(CurrentID);
	Sleep(2500);
	GM_PauseAndResume(CurrentID);
	GM_WaitForAudio(0);
	GM_UnloadAudio();
}
#endif
