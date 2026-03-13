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

#ifndef DEBUGLOG_H
#  define DEBUGLOG_H

#  ifdef _DEBUG
void __cdecl DbgPrintf(const char *format, ...);
const char* COMErrStr(HRESULT Result);
const char* XAudioErrStr(HRESULT Result);
#  else
#    define DbgPrintf(format, ...)
#  endif

#endif // DEBUGLOG_H
