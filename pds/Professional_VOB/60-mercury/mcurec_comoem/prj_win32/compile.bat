echo off
echo OEM Recorder mcu.dll build begin

if exist mcu.dep (
	del mcu.dep
)

	echo build clean MinDependency
	nmake /NOLOGO /A /B /f mcu.mak CLEAN ALL CFG="mcu - Win32 Release MinDependency" /x nmakeError.log > ..\..\..\10-common\version\compileinfo\mcuoem_r.txt
	copy /Y ReleaseMinDependency\mcu.dll ..\..\..\10-common\lib\release\win32\mcu.dll


echo mcu.dll build over
echo on


