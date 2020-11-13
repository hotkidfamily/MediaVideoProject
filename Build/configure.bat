
SETLOCAL ENABLEDELAYEDEXPANSION

@echo off

for %%i in (x86) do (
 for %%j in (Debug Release) do (
	set destDir=..\Output\%%i\%%j\

	echo [info] init %%i %%j env to !destDir!

	xcopy ..\3rd\ffmpeg2.7.1\bin\*.dll  !destDir! /y /i /r /D
	xcopy ..\3rd\libx264\libx264-146.dll* !destDir! /y /i /r /D
	xcopy ..\3rd\directx-jun2010\bin\release\D3DX9_43.dll* !destDir! /y /i /r /D
	)
)
