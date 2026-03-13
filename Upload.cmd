@echo off

set BIN_FOLDER=C:\GameMakerProjects\Example1\datafiles\Data

copy Win32\Debug\AudioGM.dll %BIN_FOLDER%\AudioGMd_x86.dll
copy Win32\Release\AudioGM.dll %BIN_FOLDER%\AudioGM_x86.dll
copy x64\Debug\AudioGM.dll %BIN_FOLDER%\AudioGMd_x86_64.dll
copy x64\Release\AudioGM.dll %BIN_FOLDER%\AudioGM_x86_64.dll
copy AudioGM\AudioGM.h %BIN_FOLDER%\AudioGM.h
