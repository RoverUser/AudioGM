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

#ifndef AUDIOGM_PRIVATE_H
#  define AUDIOGM_PRIVATE_H

//#  define LibraryName TEXT("AudioGM")
#  define ENABLE_TEST
#  define USE_SLEEP

#  ifdef ENABLE_TEST
AUDIOGM_API void AUDIOGM_CALLCONV TestAPI(HWND hwndStub, HINSTANCE hInstance, LPTSTR pszCmdLine, int nCmdShow);
#  endif

#  ifdef _DEBUG
//#    define DEBUG_FOR_PUBLIC
#  endif

#  ifdef USE_SLEEP
#    ifdef _DEBUG
#      define MAIN_SLEEP 1
#    else
#      define MAIN_SLEEP 10
#    endif
#    define TIME_FOR_THREADS_TO_SYNC 150
#  endif

typedef struct ThreadInfo {
	unsigned int dwThreadId;
	HANDLE hThread;
} THREADINFO, *PTHREADINFO;

// NOTICE: KEEP IN SYNC WITH THE KOSTIL IN THE INIT FUNCTION!!!
// NOTICE: KEEP IN SYNC WITH THE KOSTIL IN THE INIT FUNCTION!!!
// NOTICE: KEEP IN SYNC WITH THE KOSTIL IN THE INIT FUNCTION!!!
typedef struct AudioInfo {
	size_t Slot;
	size_t ThreadSlot;
	char WavPath[MAX_PATH];
	gmreal_t Volume;
	gmreal_t Loop;
	gmreal_t Pause;
	gmreal_t Stop;
	gmreal_t Finished;
} AUDIOINFO, *PAUDIOINFO;

#endif // AUDIOGM_PRIVATE_H
