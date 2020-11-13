@echo off

SETLOCAL ENABLEDELAYEDEXPANSION


for %%i in (x86) do (
 for %%j in (Debug Release) do (
	set destDir=..\Package\%%i-%%j\
	set srcDir=..\Output\%%i\%%j\

	echo [info] init %%i %%j env to !destDir!
	
	IF not EXIST !destDir! (
		mkdir !destDir!
	)

	xcopy !srcDir!*.dll  !destDir! /y /i /r /D
	xcopy !srcDir!*.exe  !destDir! /y /i /r /D
	)
)
