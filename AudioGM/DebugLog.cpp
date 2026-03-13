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
#include "DebugLog.h"

#ifdef _DEBUG
// NOTICE: Copied from some site (not StackOverflow), forgot to credit. Oops.
void __cdecl DbgPrintf(const char *format, ...) {
	char buf[4096], *p = buf;
	va_list args;
	int n;

	va_start(args, format);
	n = _vsnprintf_s(p, 4096, sizeof buf - 3, format, args);
	va_end(args);

	p += (n < 0) ? sizeof buf - 3 : n;

	while ( p > buf && isspace(p[-1]) )
		*--p = '\0';

	*p++ = '\r';
	*p++ = '\n';
	*p = '\0';

	OutputDebugStringA(buf);
}

const char* COMErrStr(HRESULT Result) {
	switch (Result) {
		case E_INVALIDARG:
			return "Invalid argument!";
		case E_OUTOFMEMORY:
			return "Out of memory!";
		case E_UNEXPECTED:
			return "E_UNEXPECTED!";
		case RPC_E_CHANGED_MODE:
			return "Incompatible concurrency model!";
		case S_FALSE:
			return "Already initialized!";
		default:
			return "Unknown error!";
	}
}

const char* XAudioErrStr(HRESULT Result) {
	switch (Result) {
		case XAUDIO2_E_INVALID_CALL:
			return "Invalid call!";
#ifdef _XBOX
		case XAUDIO2_E_XMA_DECODER_ERROR:
			return "XMA Hardware suffered an unrecoverable error!";
#endif
		case XAUDIO2_E_XAPO_CREATION_FAILED:
			return "Effect failed to instantiate!";
		case XAUDIO2_E_DEVICE_INVALIDATED:
			return "Audio device became unavailable mid-way! Perhaps you unplugged it?";
		// This is not documented?!?
		case REGDB_E_CLASSNOTREG:
			return "Class is not registered! Did you install XAudio 2.7?";
		default:
			return "Unknown error!";
	}
}
#endif
