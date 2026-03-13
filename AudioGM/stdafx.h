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

#pragma once

#include "targetver.h"

// CRT's memory leak detection
#if defined(DEBUG) || defined(_DEBUG)
#  include <crtdbg.h>
#endif

// Standard C++ library includes
#include <vector>

// Standard C library includes
#include <string.h>
#include <limits.h>
#include <float.h>
#include <math.h>
#include <wchar.h>
#include <assert.h>
#include <locale.h>
#ifdef _DEBUG
#  include <stdio.h>
#  include <stdarg.h>
#  include <ctype.h>
#endif

// Windows includes
#include <windows.h>
#include <initguid.h>
#include <shlobj.h>
#include <mmsystem.h>
#include <process.h>

// DirectX includes
#include <dxsdkver.h>

#if ( _DXSDK_PRODUCT_MAJOR < 9 || _DXSDK_BUILD_MAJOR < 1949 )
#  error The installed DXSDK is out of date.
#endif

#include <dxerr.h> // HRESULT translation for Direct3D and other APIs 
#include <XAudio2.h>

//#include "DXUT\mystring.h"
