@echo off
del /F /Q debug\*.*
nmake /NOLOGO /A /B /f Recorder.mak CFG="Recorder - Win32 Debug" 
copy /Y debug\Recorder.exe ..\..\..\10-common\version\debug\win32
@rem copy /Y debug\Recorder.exe ..\..\..\10-common\version\debug\test\unittest\service2\recorder


del /F /Q release\*.*
nmake /NOLOGO /A /B /f Recorder.mak CFG="Recorder - Win32 Release" 
copy /Y release\Recorder.exe ..\..\..\10-common\version\release\win32
@rem copy /Y debug\Recorder.exe ..\..\..\10-common\version\release\test\unittest\service2\recorder
cd..

@if not exist "..\..\10-Common\version\release\test\UnitTest\Service2\recorder\conf/" mkdir "..\..\10-Common\version\release\test\UnitTest\Service2\recorder\conf"

@rem copy /Y conf\reccfg.ini ..\..\10-common\version\debug\test\unittest\service2\recorder\conf
@rem copy /Y conf\recDebugCfg.ini ..\..\10-common\version\release\test\unittest\service2\recorder\conf

echo on