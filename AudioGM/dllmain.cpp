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

BOOL APIENTRY DllMain(
	HMODULE	hModule,
	DWORD	fdwReason,
	LPVOID	lpReserved
) {
	switch (fdwReason) {
		case DLL_PROCESS_ATTACH:
			if (!DisableThreadLibraryCalls(hModule)) {
				return FALSE;
			}
		case DLL_PROCESS_DETACH:
			if (lpReserved != nullptr)
				break; // Do not cleanup if process was terminated.

			// Perform any necessary cleanup.
			// TODO: Maybe we should cleanup XAudio 2 here?
			break;
	}
	return TRUE;
}
